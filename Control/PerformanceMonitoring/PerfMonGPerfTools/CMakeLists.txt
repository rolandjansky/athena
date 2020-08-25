# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PerfMonGPerfTools )

# External dependencies:
find_package( gperftools COMPONENTS profiler )

# Don't build the package if GPerfTools is not available.
if( NOT GPERFTOOLS_FOUND )
  message( WARNING "GPerfTools not available. Not building PerfMonGPerfTools." )
  return()
endif()

# Component(s) in the package:
atlas_add_component( PerfMonGPerfTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GPERFTOOLS_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GPERFTOOLS_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py scripts/gathena )

