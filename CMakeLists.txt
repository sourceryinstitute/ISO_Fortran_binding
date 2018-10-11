cmake_minimum_required(VERSION 3.2)
set(ISO_Fortran_binding_version 1.0.0)
project(ISO_Fortran_binding VERSION "${ISO_Fortran_binding_version}" LANGUAGES C)

# Set the type/configuration of build to perform
set ( CMAKE_CONFIGURATION_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "CodeCoverage" )
set ( CMAKE_BUILD_TYPE "Release" CACHE STRING "Select which configuration to build." )
set_property ( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )

message( STATUS "Building ISO_Fortran_binding configuration: ${CMAKE_BUILD_TYPE}")
message( STATUS "Running with CMake from: ${CMAKE_COMMAND}" )
message( STATUS "Current source dir (for ISO_Fortran_binding): ${CMAKE_CURRENT_SOURCE_DIR}")
message( STATUS "Building ISO_Fortran_binding version: ${ISO_Fortran_binding_version}" )
message( STATUS "Building for target architecture: ${CMAKE_SYSTEM_PROCESSOR}" )

#Print an error message on an attempt to build inside the source directory tree:
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "ERROR! "
    "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    " == CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    "\nThis archive does not support in-source builds:\n"
    "You must now delete the CMakeCache.txt file and the CMakeFiles/ directory under "
    "the 'src' source directory or you will not be able to configure correctly!"
    "\nYou must now run something like:\n"
    "  $ rm -r CMakeCache.txt CMakeFiles/"
    "\n"
    "Please create a directory outside the ISO_Fortran_binding source tree and build under that outside directory "
    "in a manner such as\n"
    "  $ mkdir build\n"
    "  $ cd build\n"
    "  $ CC=gcc FC=gfortran cmake -DBUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/install/dir /path/to/ISO_Fortran_binding/source/dir \n"
    "\nsubstituting the appropriate syntax for your shell (the above line assumes the bash shell)."
    )
endif()

#Report untested Fortran compiler unless explicitly directed to build all examples.
if ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU" )
  set ( CMAKE_C_FLAGS_CODECOVERAGE "-fprofile-arcs -ftest-coverage -O0"
    CACHE STRING "Code coverage C compiler flags")
else()
  message(WARNING
    "\n"
    "Attempting to build with untested C compiler: ${CMAKE_C_COMPILER_ID}.  This project uses"
    "variable-length arrays, a GCC extension that might not be supported by ${CMAKE_C_COMPILER_ID}."
    "Please report any issues at https://github.com/sourceryinstitute/ISO_Fortran_binding/issue\n\n"
  )
endif()

#---------------------------------------
# Make sure a minimal C program compiles
#---------------------------------------
include (CheckCSourceCompiles)
CHECK_C_SOURCE_COMPILES("
int main(int argc, char** argv) {
  return 0;
}"
C_SOURCE_COMPILES)

if (NOT C_SOURCE_COMPILES)
    message(FATAL_ERROR "C compiler doesn't compile a minimal C program."
    "Try setting CC to the appropriate C compiler and reconfigure. "
    "i.e., `cmake -DCMAKE_C_COMPILER=/path/to/cc ..` or set it by editing the cache using "
    "cmake-gui or ccmake."
    )
endif()

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

enable_testing()

set( unit_tests
  CFI_address
  CFI_deallocate
  CFI_establish
  CFI_is_contiguous
  CFI_section
  CFI_select_part
  CFI_setpointer
)
foreach(unit_test ${unit_tests})
  add_test("${unit_test}-test" tests/test-${unit_test})
endforeach()
