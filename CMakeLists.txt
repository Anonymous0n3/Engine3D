cmake_minimum_required(VERSION 3.20)

# --- NUCLEAR OPTION: FORCE VCPKG TOOLCHAIN ---
# Make sure to use forward slashes (/) in the path!
set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/vcpkg/scripts/buildsystems/vcpkg.cmake")
# ---------------------------------------------

project(Engine3D VERSION 1.0.0)

# Enforce modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find our libraries (vcpkg handles this invisibly)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Define your source files here
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Mesh.cpp
    src/Camera.cpp
    src/stb_image.cpp
    src/Texture.cpp
    src/Model.cpp
    src/Performance.cpp
    src/miniaudio.cpp
    src/imgui.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
)

# Create the executable
add_executable(Engine3D ${SOURCES})

set_target_properties(Engine3D PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_compile_definitions(Engine3D PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

# --- TELL CMAKE WHERE YOUR HEADERS ARE ---
target_include_directories(Engine3D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link the libraries to our executable
target_link_libraries(Engine3D PRIVATE 
    glfw
    glad::glad
    glm::glm
    assimp::assimp
    nlohmann_json::nlohmann_json
)