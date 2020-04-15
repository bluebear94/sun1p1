CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
CMAKE_POLICY(SET CMP0048 NEW)
CMAKE_POLICY(SET CMP0069 NEW)

PROJECT(sun1p1)

SET(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

IF ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
  INCLUDE(CheckIPOSupported)
  CHECK_IPO_SUPPORTED(RESULT SUPPORTED OUTPUT ERROR)
  IF (SUPPORTED)
    MESSAGE(STATUS "IPO / LTO enabled")
    SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  ELSEIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    MESSAGE(STATUS "IPO / LTO enabled (Clang)")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
  ELSE ()
    MESSAGE(STATUS "IPO / LTO disabled: ${ERROR}")
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "Debug build -- no IPO / LTO")
ENDIF ()

## ===============================================

# FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)
# INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

# FIND_PACKAGE(Lua REQUIRED)
# INCLUDE_DIRECTORIES(SYSTEM ${LUA_INCLUDE_DIR})

# FIND_PACKAGE(Threads REQUIRED)

FIND_PACKAGE(Curses REQUIRED)

## ===============================================

INCLUDE_DIRECTORIES(include/)

SET(SOURCES
  src/combat.cpp
  src/defs.cpp
  src/gen.cpp
  src/items.cpp
  src/main.cpp
  src/menu.cpp
  src/renderer.cpp
  src/slp.cpp
  src/stackops.cpp
)

# TODO: add -Werror when we get rid of all the warnings

ADD_EXECUTABLE(sun1p1 ${SOURCES})
SET(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} --std=c17 -Wall -Wextra -pedantic"
  #" -fno-exceptions -fno-rtti"
  )
SET(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} --std=c++2a -Wall -Wextra -pedantic"
  #" -fno-exceptions -fno-rtti"
  )
TARGET_LINK_LIBRARIES(sun1p1
  ${CURSES_LIBRARIES} -lm
)

# This works only with in-source builds. Sorry.
# SET(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
# ADD_CUSTOM_TARGET(
#   atest
#   COMMAND python3 ${TEST_DIR}/auto/test.py ${CMAKE_BINARY_DIR}/sca_e_kozet ${TEST_DIR}
#   SOURCES ${TEST_DIR}/auto/test.py
# )
# ADD_DEPENDENCIES(atest sca_e_kozet)

# ADD_CUSTOM_TARGET(
#   manual
#   COMMAND latexmk -xelatex -pdfxe
#     ${CMAKE_SOURCE_DIR}/doc/manual.tex --output-directory=${CMAKE_BINARY_DIR}
#   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

INSTALL(TARGETS sun1p1 DESTINATION bin)
