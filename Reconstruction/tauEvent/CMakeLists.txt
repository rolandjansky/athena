# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( tauEvent
                   src/*.cxx
                   PUBLIC_HEADERS tauEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers CaloEvent CaloIdentifier AthLinks Navigation AthenaKernel EventKernel FourMom NavFourMom GaudiKernel ParticleEvent JetEvent Particle VxVertex )

atlas_add_dictionary( tauEventDict
                      tauEvent/tauEventDict.h
                      tauEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} tauEvent
                      ELEMENT_LINKS Analysis::TauJetContainer Analysis::TauDetailsContainer )

atlas_add_dictionary( tauEnumsDict
                      tauEvent/tauEnumsDict.h
                      tauEvent/selectionEnums.xml
                      LINK_LIBRARIES tauEvent )
