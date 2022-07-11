# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( uuidgen )

# External dependencies.
find_package( UUID )

# Declare the uuidgen executable.
atlas_add_executable( uuidgen src/*.h src/*.c
   INCLUDE_DIRS ${UUID_INCLUDE_DIRS}
   LINK_LIBRARIES ${UUID_LIBRARIES} )
target_compile_definitions( uuidgen
   PRIVATE PACKAGE_STRING="athena/External/uuidgen" )

# CMake include(s).
include( CheckIncludeFile )
include( CheckSymbolExists )

# Check for the existence of headers that the source code wants to use.
foreach( _header "err.h" "langinfo.h" "locale.h" "stdio_ext.h"
                 "sys/sysmacros.h" )

   # Generate a variable name to use for the existence check of the header
   # file in question.
   string( REPLACE "." "_" _cache_name "${_header}" )
   string( REPLACE "/" "_" _cache_name "${_cache_name}" )
   string( TOUPPER "${_cache_name}" _cache_name )

   # Check if the header is available.
   check_include_file( "${_header}" ATLAS_HAVE_${_cache_name} )

   # If it does, set the appropriate definition on the build.
   if( ATLAS_HAVE_${_cache_name} )
      target_compile_definitions( uuidgen
         PRIVATE HAVE_${_cache_name} )
   endif()
endforeach()

# Check for the existence of symbols/functions that the code wants to use.
check_symbol_exists( "nanosleep" "time.h" ATLAS_HAVE_NANOSLEEP )
if( ATLAS_HAVE_NANOSLEEP )
   target_compile_definitions( uuidgen
      PRIVATE HAVE_NANOSLEEP )
endif()

check_symbol_exists( "usleep" "unistd.h" ATLAS_HAVE_USLEEP )
if( ATLAS_HAVE_NANOSLEEP )
   target_compile_definitions( uuidgen
      PRIVATE HAVE_USLEEP )
endif()

check_symbol_exists( "__fpending" "stdio.h;stdio_ext.h" ATLAS_HAVE_FPENDING )
if( ATLAS_HAVE_FPENDING )
   target_compile_definitions( uuidgen
      PRIVATE HAVE___FPENDING )
endif()

check_symbol_exists( "fsync" "unistd.h" ATLAS_HAVE_FSYNC )
if( ATLAS_HAVE_FSYNC )
   target_compile_definitions( uuidgen
      PRIVATE HAVE_FSYNC )
endif()
