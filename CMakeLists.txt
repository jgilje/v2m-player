CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(v2mplayer)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -Os -flto -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DRONAN -Os -flto -Wall -fno-asynchronous-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections")

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
SET(MAJOR_VERSION 0)
SET(DATE_VERSION 20171117)
SET(V2M_VERSION ${MAJOR_VERSION}.${DATE_VERSION})
FIND_PACKAGE(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})

ADD_LIBRARY(v2m
    SHARED
    ronan.cpp
    v2mplayer.cpp
    v2mconv.cpp
    synth_core.cpp
    sounddef.cpp
)
SET_TARGET_PROPERTIES(v2m PROPERTIES VERSION ${V2M_VERSION} SOVERSION ${MAJOR_VERSION})

ADD_EXECUTABLE(v2mplayer
    tinyplayer.cpp
)

TARGET_LINK_LIBRARIES(v2mplayer
    ${SDL2_LIBRARY}
    v2m
)
