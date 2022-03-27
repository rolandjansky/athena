# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

message( STATUS "Preparing IDE helpers" )

# singularity settings
set( ATLAS_SINGULARITY_IMAGE "" CACHE PATH
   "Use singularity wrapper" )
set( ATLAS_SINGULARITY_ARGS "" CACHE STRING
   "Singularity weapper args" )

# Common prefix
set( ATLAS_IDE_PREFIX "${CMAKE_BINARY_DIR}/ide_" )

# Dump python environment variables
set( ATLAS_DUMP_ENV_FILE "${CMAKE_BINARY_DIR}/env.txt" CACHE FILEPATH
   "File containing dump of environment variables" )

if( ATLAS_DUMP_ENV_FILE )
   execute_process(
      COMMAND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh" printenv
      COMMAND grep -w ^PYTHONPATH
      OUTPUT_FILE "${ATLAS_DUMP_ENV_FILE}" )
endif()

# compiler
execute_process(
   COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CXX_COMPILER}" "${ATLAS_IDE_PREFIX}compiler"
   OUTPUT_FILE "${ATLAS_IDE_PREFIX}compiler" )

# python
find_package( Python COMPONENTS Interpreter )
execute_process(
   COMMAND ${CMAKE_COMMAND} -E create_symlink "${Python_EXECUTABLE}" "${ATLAS_IDE_PREFIX}python"
   OUTPUT_FILE "${ATLAS_IDE_PREFIX}python" )

# flake8 wrapper
configure_file( "${CMAKE_CURRENT_LIST_DIR}/flake8_wrapper.sh.in" "${ATLAS_IDE_PREFIX}flake8" @ONLY )

# gdb wrappers
configure_file( "${CMAKE_CURRENT_LIST_DIR}/gdb_wrapper.sh.in" "${ATLAS_IDE_PREFIX}gdb_wrapper" @ONLY )
if( ATLAS_SINGULARITY_IMAGE )
   configure_file( "${CMAKE_CURRENT_LIST_DIR}/gdb_runner_singularity.sh.in" "${ATLAS_IDE_PREFIX}gdb_runner" @ONLY )
else()
   configure_file( "${CMAKE_CURRENT_LIST_DIR}/gdb_runner_plain.sh.in" "${ATLAS_IDE_PREFIX}gdb_runner" @ONLY )
endif()
