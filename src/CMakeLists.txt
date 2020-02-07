INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(..)

file(GLOB APL_SRC *.cc)
file(GLOB APL_HDS *.hh)

file(GLOB APL_SQL_SRC sql/*.cc)
file(GLOB APL_SQL_HDS sql/*.hh)

find_package(Python3 COMPONENTS Interpreter Development)
find_package(PostgreSQL)

MESSAGE(STATUS ${Python3_INCLUDE_DIRS})
MESSAGE(STATUS ${PostgreSQL_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PostgreSQL_INCLUDE_DIRS})

# main library (prevent recompilation when building tests)
ADD_EXECUTABLE(apl
    ${APL_HDS}
    ${APL_SRC}
    ${APL_SQL_SRC}
    ${APL_SQL_HDS})
target_link_libraries(apl pcre2-32 ${Python3_LIBRARIES} ${PostgreSQL_LIBRARIES})
target_link_libraries(apl pthread dl sqlite3 xcb)
SET_PROPERTY(TARGET apl PROPERTY CXX_STANDARD 11)

