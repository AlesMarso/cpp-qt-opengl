cmake_minimum_required(VERSION 3.10)

project(my-app-hello LANGUAGES CXX)

if(WIN32)
message("Hello Windows")
elseif(LINUX)
message("Hello Linux")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets)

message(STATUS "${CMAKE_PREFIX_PATH}")
message(STATUS "${Qt6_VERSION}")

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${SOURCES}) 

# Include current source directories
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

# Link your executable to the detected components
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets)