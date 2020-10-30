# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Hydjet_i )

# External dependencies:
find_package( CLHEP )
find_package( Hijing )
find_package( Hydjet )
find_package( Pyquen )
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Hydjet_iLib
   Hydjet_i/*.h src/*.cxx src/*.F
   PUBLIC_HEADERS Hydjet_i
   INCLUDE_DIRS ${PYQUEN_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS}
   ${HYDJET_INCLUDE_DIRS} ${HIJING_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${PYQUEN_LIBRARIES} ${PYTHIA6_LIBRARIES}
   ${HYDJET_LIBRARIES} ${HIJING_LIBRARIES} CxxUtils GeneratorFortranCommonLib
   GeneratorModulesLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib
   AthenaKernel GaudiKernel GeneratorObjects GeneratorUtilsLib StoreGateLib TruthUtils )

atlas_add_component( Hydjet_i
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel Hydjet_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
