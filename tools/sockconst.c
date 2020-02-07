/*
   kindly sent by Christian Robert to bug-apl@gnu.org on April 25, 2015
  
   This program generates APL code that helps translating the various
   socket-related macros defined in netinet/in.h and sys/socket.h (which
   are represented as strings such as 'AF_INET' in APL) and converts them
   to the corresponding numbers (as expected by the FILE_IO API).

   That makes it possible to write e.g.

   sock ← FIO∆socket FIO∆SockHelper 'AF_INET' 'SOCK_STREAM' 'IPPROTO_TCP' 

   instead of:

   sock ← FIO∆socket 2 1 6

   in APL programs using sockets in FILE_IO.

   The APL code generated by this program is then supposed to be inserted into
   function FIO∆SockHelper in woekspace FILE_IO.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

#define P(a) printf ("t←t⍪'%s' %d\n", #a, a)

int main (void)
{
  printf ("t←0 2⍴'' 0\n");

  #ifdef AF_UNSPEC
    P (AF_UNSPEC);
  #endif

  #ifdef AF_LOCAL
    P (AF_LOCAL);
  #endif

  #ifdef AF_UNIX
    P (AF_UNIX);
  #endif

  #ifdef AF_INET
    P (AF_INET);
  #endif

  #ifdef AF_SNA
    P (AF_SNA);
  #endif

  #ifdef AF_DECnet
    P (AF_DECnet);
  #endif

  #ifdef AF_APPLETALK
    P (AF_APPLETALK);
  #endif

  #ifdef AF_ROUTE
    P (AF_ROUTE);
  #endif

  #ifdef AF_IPX
    P (AF_IPX);
  #endif

  #ifdef AF_ISDN
    P (AF_ISDN);
  #endif

  #ifdef AF_INET6
    P (AF_INET6);
  #endif

  #ifdef AF_BLUETOOTH
    P (AF_BLUETOOTH);
  #endif

  #ifdef AF_MAX
    P (AF_MAX);
  #endif

  #ifdef PF_INET
    P (PF_INET);
  #endif

  #ifdef PF_LOCAL
    P (PF_LOCAL);
  #endif

  #ifdef PF_UNIX
    P (PF_UNIX);
  #endif

  #ifdef SOCK_STREAM
    P (SOCK_STREAM);
  #endif

  #ifdef SOCK_DGRAM
    P (SOCK_DGRAM);
  #endif

  #ifdef SOCK_RAW
    P (SOCK_RAW);
  #endif

  #ifdef SOCK_RDM
    P (SOCK_RDM);
  #endif

  #ifdef SOCK_SEQPACKET
    P (SOCK_SEQPACKET);
  #endif

  #ifdef SOCK_DCCP
    P (SOCK_DCCP);
  #endif

  #ifdef SOCK_PACKET
    P (SOCK_PACKET);
  #endif

  #ifdef SOCK_CLOEXEC
    P (SOCK_CLOEXEC);
  #endif

  #ifdef SOCK_NONBLOCK
    P (SOCK_NONBLOCK);
  #endif

  #ifdef IPPROTO_IP
    P (IPPROTO_IP);
  #endif

  #ifdef IPPROTO_ICMP
    P (IPPROTO_ICMP);
  #endif

  #ifdef IPPROTO_IPIP
    P (IPPROTO_IPIP);
  #endif

  #ifdef IPPROTO_TCP
    P (IPPROTO_TCP);
  #endif

  #ifdef IPPROTO_PUP
    P (IPPROTO_PUP);
  #endif

  #ifdef IPPROTO_UDP
    P (IPPROTO_UDP);
  #endif

  #ifdef IPPROTO_IDP
    P (IPPROTO_IDP);
  #endif

  #ifdef IPPROTO_TP
    P (IPPROTO_TP);
  #endif

  #ifdef IPPROTO_DCCP
    P (IPPROTO_DCCP);
  #endif

  #ifdef IPPROTO_IPV6
    P (IPPROTO_IPV6);
  #endif

  #ifdef IPPROTO_ROUTING
    P (IPPROTO_ROUTING);
  #endif

  #ifdef IPPROTO_FRAGMENT
    P (IPPROTO_FRAGMENT);
  #endif

  #ifdef IPPROTO_RSVP
    P (IPPROTO_RSVP);
  #endif

  #ifdef IPPROTO_GRE
    P (IPPROTO_GRE);
  #endif

  #ifdef IPPROTO_ESP
    P (IPPROTO_ESP);
  #endif

  #ifdef IPPROTO_AH
    P (IPPROTO_AH);
  #endif

  #ifdef IPPROTO_ICMPV6
    P (IPPROTO_ICMPV6);
  #endif

  #ifdef IPPROTO_NONE
    P (IPPROTO_NONE);
  #endif

  #ifdef IPPROTO_DSTOPTS
    P (IPPROTO_DSTOPTS);
  #endif

  #ifdef IPPROTO_MTP
    P (IPPROTO_MTP);
  #endif

  #ifdef IPPROTO_ENCAP
    P (IPPROTO_ENCAP);
  #endif

  #ifdef IPPROTO_PIM
    P (IPPROTO_PIM);
  #endif

  #ifdef IPPROTO_COMP
    P (IPPROTO_COMP);
  #endif

  #ifdef IPPROTO_SCTP
    P (IPPROTO_SCTP);
  #endif

  #ifdef IPPROTO_UDPLITE
    P (IPPROTO_UDPLITE);
  #endif

  #ifdef IPPROTO_RAW
    P (IPPROTO_RAW);
  #endif

  #ifdef SOL_SOCKET
    P (SOL_SOCKET);
  #endif

  #ifdef SO_BINDTODEVICE
    P (SO_BINDTODEVICE);
  #endif

  #ifdef SO_REUSEADDR
    P (SO_REUSEADDR);
  #endif

  #ifdef SO_BROADCAST
    P (SO_BROADCAST);
  #endif

  #ifdef INADDR_ANY
    P (INADDR_ANY);
  #endif

  return 0;
}

