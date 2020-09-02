# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Tauolapp_i )

# External dependencies:
find_package( CLHEP )
if (HEPMC3_USE)
find_package( Tauolapp COMPONENTS TauolaFortran TauolaHepMC3 )
else()
find_package( Tauolapp COMPONENTS TauolaFortran TauolaHepMC )
endif()

# Component(s) in the package:
atlas_add_component( Tauolapp_i
   Tauolapp_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${TAUOLAPP_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${TAUOLAPP_LIBRARIES} AtlasHepMCLib ${CLHEP_LIBRARIES}
   AthenaBaseComps AthenaKernel AthenaPoolUtilities EventInfo GaudiKernel
   GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py )
