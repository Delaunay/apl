/*
    This file is part of GNU APL, a free implementation of the
    ISO/IEC Standard 13751, "Programming Language APL, Extended"

    Copyright (C) 2008-2015  Dr. Jürgen Sauermann

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sched.h>
#include <signal.h>

#include "Common.hh"
#include "Parallel.hh"
#include "SystemVariable.hh"
#include "Thread_context.hh"
#include "UserPreferences.hh"

#if !PARALLEL_ENABLED
#define sem_init(x, y, z)   /* NO-OP */
#endif // PARALLEL_ENABLED

const char * Parallel_job_list_base::started_loc = 0;

#if CORE_COUNT_WANTED == 0
bool Parallel::run_parallel = false;
#else
bool Parallel::run_parallel = true;
#endif

//=============================================================================
void
Parallel::init(bool logit)
{
#if PARALLEL_ENABLED

   // init global semaphores
   //
   __sem_init(print_sema,          /* shared */ 0, /* value */ 1);
   __sem_init(pthread_create_sema, /* shared */ 0, /* value */ 0);

   // compute number of cores available and set total_CPU_count accordingly
   //
   init_all_CPUs(logit);

   // initialize thread contexts
   //
   Thread_context::init_parallel(get_max_core_count(), logit);

   // create threads...
   //
   Thread_context::get_context(CNUM_MASTER)->thread = pthread_self();
   for (int w = CNUM_WORKER1; w < get_max_core_count(); ++w)
       {
         Thread_context * tctx = Thread_context::get_context(CoreNumber(w));
         const int result = pthread_create(&(tctx->thread), /* attr */ 0,
                                             worker_main, tctx);
         if (result)
            {
              CERR << "pthread_create() failed at " << LOC
                   << " : " << strerror(result) << endl;
              Thread_context::set_active_core_count(CCNT_1);
              return;
            }

         // wait until new thread has reached its work loop
         sem_wait(pthread_create_sema);
       }

   // bind threads to cores
   //
   loop(c, get_max_core_count())
       Thread_context::get_context(CoreNumber(c))
                       ->bind_to_cpu(all_CPUs[c], logit);

   if (logit)   Thread_context::print_all(CERR);

   // the threads above start in state locked. Wake them up...
   //
#if CORE_COUNT_WANTED == -3
   set_core_count(CCNT_1, logit);
#else
   set_core_count(get_max_core_count(), logit);
#endif

#else // not PARALLEL_ENABLED

   Thread_context::init_sequential(logit);
   all_CPUs.push_back(CPU_0);

#endif // PARALLEL_ENABLED
}

sem_t __print_sema;
sem_t __pthread_create_sema;
sem_t * Parallel::print_sema          = &__print_sema;
sem_t * Parallel::pthread_create_sema = &__pthread_create_sema;

//=============================================================================
bool
Parallel::set_core_count(CoreCount new_count, bool logit)
{
   // this function is called from ⎕SYL[26]←
   //
   if (new_count < CCNT_0)                 return true;   // error
   if (new_count > get_max_core_count())   return true;   // error

   run_parallel = new_count != CCNT_0;
   if (new_count == CCNT_0)   new_count = CCNT_1;

   if (new_count == Thread_context::get_active_core_count())
      {
        Log(LOG_Parallel || logit)
           {
             CERR <<
                "Parallel::set_core_count(): keeping current core count of "
                  << Thread_context::get_active_core_count() << endl;
             Thread_context::print_all(CERR);
           }

         return false;
      }

   if (new_count > Thread_context::get_active_core_count())
      {
        Log(LOG_Parallel || logit)
           CERR << "Parallel::set_core_count(): increasing core count from "
                << Thread_context::get_active_core_count()
                << " to " << new_count << endl;

        lock_pool(logit);
        Thread_context::set_active_core_count(new_count);
        for (int c = 1; c < new_count; ++c)
            Thread_context::get_context(CoreNumber(c))->job_number =
                            Thread_context::get_master().job_number;
        unlock_pool(logit);

        Log(LOG_Parallel || logit)   Thread_context::print_all(CERR);
      }
   else
      {
        Log(LOG_Parallel || logit)
           CERR << "Parallel::set_core_count(): decreasing core count from "
                << Thread_context::get_active_core_count()
                << " to " << new_count << endl;
        lock_pool(logit);
        Thread_context::set_active_core_count(new_count);
        unlock_pool(logit);

        Log(LOG_Parallel || logit)   Thread_context::print_all(CERR);
      }

   return false;   // no error
}
//-----------------------------------------------------------------------------
void
Parallel::init_all_CPUs(bool logit)
{
   // determine the max number of cores. We first handle the ./configure cases
   // that can live without pthread_getaffinity_np() and friends
   //
int count = CORE_COUNT_WANTED;

#if CORE_COUNT_WANTED == 0

   run_parallel = false;
   all_CPUs.push_back(CPU_0);
   return;

#elif CORE_COUNT_WANTED == 1

   run_parallel = true;
   all_CPUs.push_back(CPU_0);
   return;

#elif CORE_COUNT_WANTED == -2   // --cc N

#elif ! HAVE_AFFINITY_NP

   if (uprefs.requested_cc < 1)   // serial or 1 core
      {
        run_parallel = uprefs.requested_cc > 0;
        all_CPUs.push_back(CPU_0);
        return;
      }
   else
      {
        count = uprefs.requested_cc;
      }
#endif

   // at this point count is -1, -3, or > 1. Figure how many cores we have.
   //
#if HAVE_AFFINITY_NP

cpu_set_t CPUs;
   CPU_ZERO(&CPUs);

const int err = pthread_getaffinity_np(pthread_self(), sizeof(CPUs), &CPUs);
   if (err)
      {
        CERR << "pthread_getaffinity_np() failed with error "
             << err << endl;
        all_CPUs.push_back(CPU_0);
        return;
      }

   {
     const size_t CPU_count = CPU_COUNT(&CPUs);
     loop(c, 8*sizeof(cpu_set_t))
         {
           if (CPU_ISSET(c, &CPUs))
              {
                all_CPUs.push_back(CPU_Number(c));
                if (all_CPUs.size() == CPU_count)   break;   // all CPUs found
              }
         }
   }

   if (all_CPUs.size() == 0)
      {
        CERR << "*** no cores detected, assuming at least one! "
             << err << endl;
        all_CPUs.push_back(CPU_0);
        return;
      }

   // at this point we know the max. number of CPUs and can map cases
   // -1 and -3 to the max. number of CPUs.
   //
   if (count < 0)   count = all_CPUs.size();

   // if there are more CPUs than requested then limit all_CPUs accordingly
   if (all_CPUs.size() > size_t(count))   all_CPUs.resize(count);

   Log(LOG_Parallel || logit)
      {
        CERR << "detected " << all_CPUs.size() << " cores:";
        loop(cc, all_CPUs.size())   CERR << " #" << all_CPUs[cc];
        CERR << endl;
      }

#else   // ! HAVE_AFFINITY_NP

   if (count < 0)   count = 64;

   loop(c, CORE_COUNT_WANTED)   all_CPUs.push_back(CPU_Number(c));

   Log(LOG_Parallel || logit)
      {
        CERR <<
"The precise number of cores could not be detected because function\n"
"pthread_getaffinity_np() is not provided by your platform. Assuming a\n"
" maximum of " << all_CPUs.size() << " cores." << endl;
      }

#endif // HAVE_AFFINITY_NP
}
//-----------------------------------------------------------------------------
void
Parallel::lock_pool(bool logit)
{
   if (Thread_context::get_active_core_count() <= 1)   return;

   Thread_context::get_master().M_lock_pool();
   Thread_context::M_join();

   Log(LOG_Parallel || logit)
      {
        PRINT_LOCKED(
            CERR << "Parallel::lock_pool() : pool state is now :" << endl; )
        Thread_context::print_all(CERR);
      }
}
//-----------------------------------------------------------------------------
void
Parallel::unlock_pool(bool logit)
{
   if (Thread_context::get_active_core_count() <= 1)   return;

   Log(LOG_Parallel || logit)
     {
        for (int a = 1; a < Thread_context::get_active_core_count(); ++a)
            {
              PRINT_LOCKED(CERR << "Parallel::unlock_pool() : " << endl; )
              Thread_context * tc = Thread_context::get_context(CoreNumber(a));
              sem_post(tc->pool_sema);
              PRINT_LOCKED(
              CERR << "    pool_sema of thread #" << a << " incremented."
                   << endl;)
            }
     }
   else
     {
        for (int a = 1; a < Thread_context::get_active_core_count(); ++a)
            sem_post(Thread_context::get_context(CoreNumber(a))->pool_sema);
       }
}
//-----------------------------------------------------------------------------
void *
Parallel::worker_main(void * arg)
{
Thread_context & tctx = *reinterpret_cast<Thread_context *>(arg);

   Log(LOG_Parallel)
      {
        PRINT_LOCKED(CERR << "worker #" << tctx.get_N() << " started" << endl)
      }

   // tell the creator that we have started
   //
   sem_post(pthread_create_sema);

   // start in state locked
   //
   tctx.blocked = true;
   sem_wait(tctx.pool_sema);
   tctx.blocked = false;

   Log(LOG_Parallel)
      PRINT_LOCKED(CERR << "thread #" << tctx.get_N()
                        << " was unblocked (initially) from pool_sema" << endl)

   for (;;)
       {
         tctx.PF_fork();
         tctx.job_name = Thread_context::get_master().job_name;
         tctx.do_join = true;
         Thread_context::do_work(tctx);
         if (tctx.do_join)   tctx.PF_join();
       }

   /* not reached */
   return 0;
}
//=============================================================================
