# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventUtils )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( EventUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools AthAnalysisBaseCompsLib AthContainers AthContainersInterfaces AthLinks AthenaBaseComps AthenaKernel EventInfo xAODBase xAODCaloEvent xAODCore xAODCutFlow xAODEgamma xAODEventInfo xAODJet xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth GaudiKernel PATCoreLib ExpressionEvaluationLib TrigDecisionToolLib DerivationFrameworkInterfaces )
