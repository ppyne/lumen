cmake_minimum_required(VERSION 3.16)
project(Lumen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lumen
    main.cpp
    core/application.cpp
)

target_include_directories(lumen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
    target_sources(lumen PRIVATE platform/window_win32.cpp)
    target_link_libraries(lumen PRIVATE opengl32)
elseif (APPLE)
    target_sources(lumen PRIVATE platform/window_cocoa.mm)
    set_source_files_properties(platform/window_cocoa.mm PROPERTIES
        COMPILE_FLAGS "-fobjc-arc"
    )
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
    target_link_libraries(lumen PRIVATE ${COCOA_FRAMEWORK} ${OPENGL_FRAMEWORK})
else()
    target_sources(lumen PRIVATE platform/window_x11.cpp)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(lumen PRIVATE OpenGL::GL X11::X11)
endif()
