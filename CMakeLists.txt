cmake_minimum_required(VERSION 3.5)

set(MAJOR_VERSION 0)
set(DATE_VERSION 20200408)
set(V2M_MAN_DATE "08 Apr 2020")
set(V2M_VERSION ${MAJOR_VERSION}.${DATE_VERSION})
project(v2mplayer VERSION ${V2M_VERSION})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

configure_file(src/version.h.in ${PROJECT_BINARY_DIR}/include/version.h @ONLY)
configure_file(man/man1/v2mplayer.1.in ${PROJECT_BINARY_DIR}/man/man1/v2mplayer.1 @ONLY)
add_library(v2m_obj OBJECT
    src/ronan.cpp
    src/v2mplayer.cpp
    src/v2mconv.cpp
    src/synth_core.cpp
    src/sounddef.cpp
)
set_property(TARGET v2m_obj PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(v2m_obj PRIVATE RONAN)

add_library(v2m
    SHARED
    $<TARGET_OBJECTS:v2m_obj>
)
set_target_properties(v2m PROPERTIES VERSION ${V2M_VERSION} SOVERSION ${MAJOR_VERSION})

add_library(v2m_static
    STATIC
    $<TARGET_OBJECTS:v2m_obj>
)

add_executable(v2mplayer
    src/tinyplayer.cpp
)
target_include_directories(v2mplayer PRIVATE ${PROJECT_BINARY_DIR}/include)
target_link_libraries(v2mplayer
    ${SDL2_LIBRARY}
    v2m
)

install(TARGETS v2m DESTINATION lib COMPONENT lib)
install(TARGETS v2m_static DESTINATION lib COMPONENT devel)
install(TARGETS v2mplayer DESTINATION bin COMPONENT player)
install(FILES ${PROJECT_BINARY_DIR}/man/man1/v2mplayer.1 DESTINATION share/man/man1 COMPONENT player)
install(FILES ${PROJECT_BINARY_DIR}/include/version.h src/libv2.h src/phonemtab.h src/sounddef.h src/synth.h src/types.h src/v2mconv.h src/v2mplayer.h DESTINATION include/v2m COMPONENT devel)
install(FILES LICENSE README.md doc/AUTHORS doc/CHANGELOG doc/VERSION DESTINATION share/doc/libv2m COMPONENT lib)
install(FILES LICENSE README.md doc/AUTHORS doc/CHANGELOG doc/VERSION DESTINATION share/doc/v2mplayer COMPONENT player)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_COMPONENT_PLAYER_DEPENDS lib)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_CONTACT "jgilje@jgilje.net")

set(CPACK_DEBIAN_PLAYER_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PLAYER_PACKAGE_NAME "v2mplayer")
set(CPACK_DEBIAN_PLAYER_PACKAGE_SECTION "sound")
set(CPACK_COMPONENT_PLAYER_DESCRIPTION "simple v2m media player")

set(CPACK_DEBIAN_LIB_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libv2m")
set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "sound")
set(CPACK_COMPONENT_LIB_DESCRIPTION "library for rendering v2m audio files")

set(CPACK_DEBIAN_DEVEL_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_DEVEL_PACKAGE_NAME "libv2m-devel")
set(CPACK_COMPONENT_DEVEL_DESCRIPTION "development headers for libv2m")
include(CPack)
