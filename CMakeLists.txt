cmake_minimum_required(VERSION 3.25)
project(gquake)

file(GLOB SOURCES src/*.c)

find_package(OpenGL REQUIRED)

set(MILSKO "/usr/local")
option(TRY_SDL2 "Try SDL2" ON)

add_executable(gquake ${SOURCES})
target_include_directories(gquake PRIVATE include)
target_compile_definitions(gquake PRIVATE GL_EXT_SHARED GLQUAKE)
target_link_libraries(gquake PRIVATE OpenGL::GL)

find_package(SDL2 OPTIONAL)
if(SDL2_FOUND AND TRY_SDL2)
	target_compile_definitions(gquake PRIVATE USE_SDL2)
	target_link_libraries(gquake PRIVATE SDL2::SDL2)
else()
	target_compile_definitions(gquake PRIVATE USE_MILSKO USE_MINIAUDIO)
	target_include_directories(gquake PRIVATE ${MILSKO}/include)
	target_link_directories(gquake PRIVATE ${MILSKO}/src ${MILSKO}/lib)
	target_link_libraries(gquake PRIVATE Mw)
endif()
