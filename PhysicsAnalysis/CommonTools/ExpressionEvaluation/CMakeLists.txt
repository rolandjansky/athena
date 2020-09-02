# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ExpressionEvaluation )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( ExpressionEvaluationLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS ExpressionEvaluation
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers GaudiKernel SGTools StoreGateLib TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} xAODBTagging xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODEventShape xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth )
