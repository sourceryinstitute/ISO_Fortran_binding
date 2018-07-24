cmake_minimum_required(VERSION 3.2)

# Set the type/configuration of build to perform
set ( CMAKE_CONFIGURATION_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "CodeCoverage" )
set ( CMAKE_BUILD_TYPE "Release"
  CACHE STRING "Select which configuration to build." )
set_property ( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
message( STATUS
  "
==================================================================
   Building ISO_Fortran_binding configuration: ${CMAKE_BUILD_TYPE}
==================================================================
")

message( STATUS "Running with CMake from: ${CMAKE_COMMAND}" )
message( STATUS "Current source dir (for ISO_Fortran_binding): ${CMAKE_CURRENT_SOURCE_DIR}")

# Name project and specify source languages
# Parse version from .VERSION file so that more info can be added and easier to get from scripts
file(STRINGS ".VERSION" first_line
  LIMIT_COUNT 1
  )

string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+(-rc[0-9]+)?"
  ISO_Fortran_binding_Version "${first_line}")

if((NOT (ISO_Fortran_binding_Version MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+(-rc[0-9]+)?")) AND (EXISTS "${CMAKE_SOURCE_DIR}/.git"))
  message( STATUS "Build from git repository detected")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --abbrev=0
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      RESULT_VARIABLE git_status
      OUTPUT_VARIABLE git_output
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if((git_status STREQUAL "0") AND (git_output MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+(-rc[0-9]+)?"))
      set(ISO_Fortran_binding_version "${git_output}")
    endif()
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --always
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      RESULT_VARIABLE git_status
      OUTPUT_VARIABLE full_git_describe
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT (git_status STREQUAL "0"))
      set(full_git_describe NOTFOUND)
    endif()
    # Create a source distribution target using git archive
    # e.g., `make dist` will package a release using current git state
    add_custom_target(dist # OUTPUT "${CMAKE_BINARY_DIR}/${_ISO_Fortran_binding_stem_name}.tar.gz"
      COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/makeDist.cmake" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
      COMMENT "Creating source release asset, ${ISO_Fortran_binding_stem_name}.tar.gz, from ${git_full_describe} using the `git archive` command."
      VERBATIM)
  else()
    message( WARNING "Could not find git executable!")
  endif()
endif()

if(NOT (ISO_Fortran_binding_Version MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+(-rc[0-9]+)?"))
  message( WARNING "Could not extract version from git, falling back on .VERSION, line 3.")
  file(STRINGS ".VERSION" ISO_Fortran_binding_version
  REGEX "[0-9]+\\.[0-9]+\\.[0-9]+(-rc[0-9]+)?"
  )
endif()

if(NOT full_git_describe)
  set(full_git_describe ${ISO_Fortran_binding_version})
endif()

string(REGEX REPLACE "-rc[0-9]+$"
       ".0" ISO_Fortran_binding_CMAKE_PROJECT_VERSION
       "${ISO_Fortran_binding_version}")

project(ISO_Fortran_binding VERSION "${ISO_Fortran_binding_CMAKE_PROJECT_VERSION}" LANGUAGES C Fortran)

message( STATUS "Building ISO_Fortran_binding version: ${full_git_describe}" )
set(ISO_Fortran_binding_dist_string "ISO_Fortran_binding-${full_git_describe}")
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
    "Please create a directory outside the opencoarrays source tree and build under that outside directory "
    "in a manner such as\n"
    "  $ mkdir build\n"
    "  $ cd build\n"
    "  $ CC=gcc FC=gfortran cmake -DBUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/install/dir /path/to/ISO_Fortran_binding/src/dir \n"
    "\nsubstituting the appropriate syntax for your shell (the above line assumes the bash shell)."
    )
endif()

#Report untested Fortran compiler unless explicitly directed to build all examples.
if ("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU" )
  set(gfortran_compiler true)
  set ( CMAKE_C_FLAGS_CODECOVERAGE "-fprofile-arcs -ftest-coverage -O0"
    CACHE STRING "Code coverage C compiler flags")
  set ( CMAKE_Fortran_FLAGS_CODECOVERAGE "-fprofile-arcs -ftest-coverage -O0"
    CACHE STRING "Code coverage Fortran compiler flags")
else()
  message(WARNING
    "\n"
    "Attempting to build with untested Fortran compiler: ${CMAKE_Fortran_COMPILER_ID}. "
    "Please report any failures at https://github.com/sourceryinstitute/ISO_Fortran_binding\n\n"
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
    message( FATAL_ERROR "Exhausted all possible means of detecting the Fortran compiler version, cannot proceed!" )
  endif()
  set( CMAKE_Fortran_COMPILER_VERSION "${DETECTED_VER}" )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug|DEBUG|debug")
  add_definitions(-DEXTRA_DEBUG_OUTPUT)
endif()


#--------------------------------------------------------
# Make sure a simple "hello world" C program compiles
#--------------------------------------------------------
include (CheckCSourceCompiles)
CHECK_C_SOURCE_COMPILES("
int main(int argc, char** argv) {
  return 0;
}"
C_COMPILES)

if (NOT C_COMPILES)
  message(FATAL_ERROR "MPI_C is missing! "
    "Try setting CC to the appropriate C compiler and reconfigure. "
    "i.e., `cmake -DCMAKE_C_COMPILER=/path/to/cc ..` or set it by editing the cache using "
    "cmake-gui or ccmake."
    )
endif()

#--------------------------------------------------------------
# Make sure a simple "hello world" Fortran mpi program compiles
# Try using mpi.mod first then fall back on includ 'mpif.h'
#--------------------------------------------------------------
include (CheckFortranSourceCompiles)
CHECK_Fortran_SOURCE_COMPILES("
program main
implicit none
print *,'Hello, world!'
end program
"
Fortran_SOURCE_COMPILES)

#---------------------------------------------------
# Use standardized GNU install directory conventions
#---------------------------------------------------
include(GNUInstallDirs)
set(mod_dir_tail "${ISO_Fortran_binding_dist_string}_${CMAKE_Fortran_COMPILER_ID}-${CMAKE_Fortran_COMPILER_VERSION}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${ISO_Fortran_binding_dist_string}-tests")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Setup shellcheck if present for testing/linting scripts
#--------------------------------------------------------
find_program(SHELLCHECK_EXE shellcheck
  DOC "Path to shellcheck executable for linting scripts"
  )
  if(NOT SHELLCHECK_EXE)
    message( AUTHOR_WARNING "Install shellcheck to test/lint all shell scripts.
    See https://github.com/koalaman/shellcheck#installing for info on obtaining shellcheck.")
endif()

function(lint_script script_dir script_name)
  if (SHELLCHECK_EXE)
    add_test(NAME "shellcheck:${script_name}"
      COMMAND ${SHELLCHECK_EXE} -x "${script_dir}/${script_name}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  else()
    message( AUTHOR_WARNING "test: shellcheck:${script_name} not run because shellcheck not installed." )
  endif()
endfunction()

#-----------------------------------------------
# Setup script style testing & enforcement macro
#-----------------------------------------------

find_program(style_pl style.pl "${CMAKE_SOURCE_DIR}/developer-scripts/")
function(check_script_style script_full_path)
  if(style_pl)
    add_test(NAME "style:${script_full_path}"
      COMMAND "${style_pl}" "${script_full_path}")
  endif()
endfunction()

#-------------------------------
# Recurse into the src directory
#-------------------------------
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)

#-----------------------------------------------------
# Publicize installed location to other CMake projects
#-----------------------------------------------------
install(EXPORT ISO_Fortran_bindingTargets
  NAMESPACE
    ISO_Fortran_binding::
  DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/opencoarrays"
)
include(CMakePackageConfigHelpers) # standard CMake module
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ISO_Fortran_bindingConfigVersion.cmake"
  VERSION "${opencoarrays_VERSION}"
  COMPATIBILITY AnyNewerVersion
)
configure_file("${CMAKE_SOURCE_DIR}/cmake/pkg/ISO_Fortran_bindingConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ISO_Fortran_bindingConfig.cmake" @ONLY)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ISO_Fortran_bindingConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ISO_Fortran_bindingConfigVersion.cmake"
  DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/ISO_Fortran_binding"
)

add_library(ISO_Fortran_binding INTERFACE)
target_compile_options(ISO_Fortran_binding INTERFACE -fcoarray=lib)
target_link_libraries(ISO_Fortran_binding INTERFACE caf_mpi)

#------------------------------------------
# Add portable unistall command to makefile
#------------------------------------------
# Adapted from the CMake Wiki FAQ
configure_file ( "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake.in" "${CMAKE_BINARY_DIR}/uninstall.cmake"
    @ONLY)

add_custom_target ( uninstall
  COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/uninstall.cmake" )

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

#----------
# Add tests
#----------

add_test(co_reduce_extension test-co_reduce-extension.sh)
