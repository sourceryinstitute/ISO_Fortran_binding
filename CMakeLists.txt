cmake_minimum_required(VERSION 3.2)
set(ISO_Fortran_binding_version 0.9.0)
project(ISO_Fortran_binding VERSION "${ISO_Fortran_binding_version}" LANGUAGES C Fortran)

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
if ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU" )
  set ( CMAKE_C_FLAGS_CODECOVERAGE "-fprofile-arcs -ftest-coverage -O0"
    CACHE STRING "Code coverage C compiler flags")
  set ( CMAKE_Fortran_FLAGS_CODECOVERAGE "-fprofile-arcs -ftest-coverage -O0"
    CACHE STRING "Code coverage Fortran compiler flags")
else()
  message(WARNING
    "\n"
    "Attempting to build with untested Fortran compiler: ${CMAKE_Fortran_COMPILER_ID}."
    "Please report any issues at https://github.com/sourceryinstitute/ISO_Fortran_binding/issue\n\n"
  )
endif()

#-----------------------------------------------------------------
# Set CMAKE_Fortran_COMPILER_VERSION if CMake doesn't do it for us
#-----------------------------------------------------------------
if (  NOT CMAKE_Fortran_COMPILER_VERSION )
  if ( NOT (CMAKE_VERSION VERSION_LESS 3.3.1) )
    message( AUTHOR_WARNING
     "CMake ${CMAKE_VERSION} should know about Fortran compiler versions but is missing CMAKE_Fortran_COMPILER_VERSION variable."
    )
  endif()
  # No CMAKE_Fortran_COMPILER_VERSION set, build our own
  # Try extracting it directly from ISO_FORTRAN_ENV's compiler_version
  # Write program for introspection
  file( WRITE "${CMAKE_BINARY_DIR}/get_compiler_ver.f90"
    "program main
     use iso_fortran_env, only: compiler_version, output_unit
     write(output_unit,'(a)') compiler_version()
end program"
  )
  try_run( PROG_RAN COMPILE_SUCCESS
    "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/get_compiler_ver.f90"
    RUN_OUTPUT_VARIABLE VER_STRING
  )
  if ( COMPILE_SUCCESS )
    string( REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?"
      DETECTED_VER "${VER_STRING}"
    )
    message( STATUS "Detected Fortran compiler as ${VER_STRING}" )
    message( STATUS "Extracted version number: ${DETECTED_VER}" )
  endif()
  if( ( NOT COMPILE_SUCCESS ) OR ( NOT DETECTED_VER ) )
    message( WARNING "Could not reliably detect Fortran compiler version. We'll infer it from
the C compiler if it matches the Fortran compiler ID." )
  endif()
  if( "${CMAKE_C_COMPILER_ID}" MATCHES "${CMAKE_Fortran_COMPILER_ID}" )
    set( DETECTED_VER "${CMAKE_C_COMPILER_VERSION}" )
  else()
	  message( FATAL_ERROR "Failed all attempts to detect the Fortran compiler version, cannot proceed!" )
  endif()
  set( CMAKE_Fortran_COMPILER_VERSION "${DETECTED_VER}" )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug|DEBUG|debug")
  add_definitions(-DEXTRA_DEBUG_OUTPUT)
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

#----------------------------------------------------------
# Make sure a simple "hello world" Fortran program compiles
#----------------------------------------------------------
set(OLD_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
if ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU" )
  set(CMAKE_REQUIRED_FLAGS "-ffree-form")
endif()
include (CheckFortranSourceCompiles)
CHECK_Fortran_SOURCE_COMPILES("
program main
implicit none
print *,'Hello, world!'
end program
"
Fortran_SOURCE_COMPILES)
if ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU" )
  set (CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
endif()
unset(OLD_REQUIRED_FLAGS)

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
