cmake_minimum_required(VERSION 3.25)
project(gquake)

file(GLOB SOURCES src/*.c)

if(PS2)
	set(GL ps2gl)

	set(CMAKE_EXECUTABLE_SUFFIX .elf)
elseif(VITA)
	set(GL vitaGL)
else()
	find_package(OpenGL REQUIRED)

	set(GL OpenGL::GL)
endif()

set(MILSKO "/usr/local")
option(TRY_SDL2 "Try SDL2" ON)

add_executable(gquake ${SOURCES})
target_include_directories(gquake PRIVATE include)
target_compile_definitions(gquake PRIVATE GL_EXT_SHARED GLQUAKE)
target_link_libraries(gquake PRIVATE ${GL})

if(PSP)
	target_link_libraries(gquake PRIVATE SDL2main pspsdk pspnet cglue)

	create_pbp_file(TARGET gquake
		TITLE "GQuake")
elseif(PS2)
	target_link_libraries(gquake PRIVATE SDL2main stdc++ ps2stuff dma -ldebug)

	add_custom_command(TARGET gquake POST_BUILD
		COMMAND rm -rf gquake
		COMMAND mkdir -p gquake
		COMMAND cp gquake.elf gquake/GQUAKE.ELF
		COMMAND cp -rf ${CMAKE_SOURCE_DIR}/id1 gquake/
		COMMAND echo "BOOT2 = cdrom0:\\\\GQUAKE.ELF" > gquake/SYSTEM.CNF
		COMMAND genisoimage -udf -o gquake.iso gquake
	)
elseif(VITA)
	include(${VITASDK}/share/vita.cmake)

	target_link_libraries(gquake PRIVATE SDL2main stdc++ vitashark SceShaccCg_stub SceKernelDmacMgr_stub mathneon)

	vita_create_self(gquake.self gquake)
	vita_create_vpk(gquake.vpk PYRT10000 gquake.self
		FILE ${CMAKE_SOURCE_DIR}/id1 sce_sys/id1
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(gquake PRIVATE wsock32)
else()
	target_link_libraries(gquake PRIVATE m)
endif()

set(FOUND FALSE)

if(NOT FOUND AND TRY_SDL2)
	find_package(SDL2)
	target_compile_definitions(gquake PRIVATE USE_SDL2)
	target_link_libraries(gquake PRIVATE SDL2::SDL2)

	set(FOUND ON)
endif()

if(NOT FOUND)
	target_compile_definitions(gquake PRIVATE USE_MILSKO USE_MINIAUDIO)
	target_include_directories(gquake PRIVATE ${MILSKO}/include)
	target_link_directories(gquake PRIVATE ${MILSKO}/src ${MILSKO}/lib)
	target_link_libraries(gquake PRIVATE Mw)
endif()
