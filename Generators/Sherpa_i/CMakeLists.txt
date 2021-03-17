# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Sherpa_i )

# External dependencies:
find_package( CLHEP )
find_package( OpenLoops )
find_package( Sherpa COMPONENTS SherpaTools SherpaInitialization )
find_package( hepmc3 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Sherpa_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${SHERPA_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${SHERPA_LIBRARIES} GaudiKernel GeneratorModulesLib TruthUtils AthenaBaseComps AthenaKernel )

# Install file(s) from the package.
atlas_install_headers( Sherpa_i )
atlas_install_joboptions( share/common/*.py )
atlas_install_python_modules( python/sherpaTarCreator)
atlas_install_scripts( python/*.py )

# Set (a) Sherpa specific environment variable(s).
set( SherpaEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of SherpaEnvironment.cmake" )
find_package( SherpaEnvironment )
