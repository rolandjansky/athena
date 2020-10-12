# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Declare the package name:
atlas_subdir( Starlight_i )

# External dependencies:
find_package( CLHEP )
find_package( Starlight )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Starlight_iLib
   Starlight_i/*.h src/*.cxx
   PUBLIC_HEADERS Starlight_i
   INCLUDE_DIRS ${STARLIGHT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${STARLIGHT_LIBRARIES} GeneratorModulesLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel
   GaudiKernel GeneratorUtilsLib TruthUtils StoreGateLib )

atlas_add_component( Starlight_i
   src/components/*.cxx
   LINK_LIBRARIES Starlight_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
