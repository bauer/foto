cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(foto VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# configure header file to pass version number to the source code
configure_file(foto.h.in foto.h)

# find all src files (no sub cmake files available)
file(GLOB_RECURSE SRC RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
file(GLOB_RECURSE SRCLIB RELATIVE ${CMAKE_SOURCE_DIR} "libexif/*.c")

# add the executable
add_executable(foto ${SRCLIB} ${SRC})

# configured header file will be written into the binary tree, we must add that directory to the list of paths to search for include files
target_include_directories(foto PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${CMAKE_SOURCE_DIR}/inc"
                           "${CMAKE_SOURCE_DIR}"
                           #"${CMAKE_SOURCE_DIR}/libexif" # #include <libexif/?.h> is used inside libexif
                           )

# link math to get pow support
target_link_libraries(foto m)
