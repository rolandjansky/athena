# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PerfMonVTune )

####
# VTune hack for the time-being
find_program( VTUNE_EXECUTABLE vtune )
get_filename_component( VTUNE_DIR ${VTUNE_EXECUTABLE} PATH )
set( ITT_PREFIX ${VTUNE_DIR}/.. )

find_path( ITT_INCLUDE_DIR NAMES ittnotify.h HINTS ${ITT_PREFIX}/include )
find_library( ITT_LIBRARY NAMES ittnotify HINTS ${ITT_PREFIX}/lib64 )

include_directories(${ITT_INCLUDE_DIR})

if ( NOT ITT_LIBRARY OR NOT ITT_INCLUDE_DIR )
  message( FATAL_ERROR
    "\nYou must have VTune setup properly for compiling PerfMonVTune.\n"
    "From within CERN this can be accomplished by doing:\n"
    "source /cvmfs/projects.cern.ch/intelsw/psxe/linux/all-setup.sh\n"
    "*BEFORE* setting up Athena." )
endif()
####

# Component(s) in the package:
atlas_add_component( PerfMonVTune
                     src/*.cxx
                     src/components/*.cxx 
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel ${ITT_LIBRARY} ${CMAKE_DL_LIBS} )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py  POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
