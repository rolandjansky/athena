# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Rivet_i )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib )
find_package( ROOT COMPONENTS Core Hist )
find_package( Rivet )
find_package( YODA )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Rivet_i
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
    ${RIVET_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS}
   LINK_LIBRARIES ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} AtlasHepMCLib
   ${RIVET_LIBRARIES} ${YODA_LIBRARIES} AthenaBaseComps GaudiKernel
   AthenaKernel GeneratorObjects GenInterfaces PathResolver EventInfo )

# Install file(s) from the package.
atlas_install_scripts( share/setupRivet )

# Set up the runtime environment of Rivet.
set( RivetEnvironment_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
   CACHE PATH "Location of RivetEnvironmentConfig.cmake" )
find_package( RivetEnvironment )
