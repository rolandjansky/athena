# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( JetRecTools )

# External dependencies.
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )

# Component(s) in the package.
atlas_add_library( JetRecToolsLib
   JetRecTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetRecTools
   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
   LINK_LIBRARIES ${FASTJET_LIBRARIES} AthContainers AsgTools xAODPFlow
   xAODTracking xAODCore xAODBase xAODCaloEvent xAODEgamma xAODJet
   TrackVertexAssociationToolLib JetEDM JetInterface JetRecLib
   InDetTrackSelectionToolLib PFlowUtilsLib
   PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} xAODTruth )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetRecTools
      src/components/*.cxx
      LINK_LIBRARIES JetRecToolsLib )
endif()

# Install files from the package.
atlas_install_python_modules( python/*.py )
