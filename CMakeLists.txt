cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(tunedotsh VERSION 0.1.0 LANGUAGES CXX)

file(GLOB SRCS src/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)


add_executable(tunedotsh ${SRCS})

if(UNIX AND NOT APPLE)
	find_package(PkgConfig)
	pkg_check_modules(NCURSES REQUIRED ncurses)
	if(NCURSES_LIBRARIES)
		message("NCURSES - Found")
	endif()

	link_directories(${PROJECT_SOURCE_DIR}/lib/unix)
	include_directories(${PROJECT_SOURCE_DIR}/include/unix)

	target_link_libraries(tunedotsh ${NCURSES_LIBRARIES} ${PROJECT_SOURCE_DIR}/lib/unix/libfmod.so)


elseif(WIN32)

	include_directories(${PROJECT_SOURCE_DIR}/include/win32)
	link_directories(${PROJECT_SOURCE_DIR}/lib/win32)

	target_link_libraries(tunedotsh ${PROJECT_SOURCE_DIR}/lib/win32/pdcurses.lib ${PROJECT_SOURCE_DIR}/lib/win32/fmod64_vc.lib
	user32.lib gdi32.lib advapi32.lib Comdlg32.lib shell32.lib)

endif()
