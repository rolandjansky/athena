# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers GaudiKernel SGTools StoreGateLib TrigDecisionToolLib AthenaBaseComps AthAnalysisBaseCompsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} xAODBTagging xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODEventShape xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth )

	   # atlas_add_component( ExpressionEvaluation
	   #          src/components/*.cxx
	   #          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
	   #:          LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthContainers SGTools StoreGateLib SGtests GaudiKernel TrigDecisionToolLib xAODBTagging xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODEventShape xAODJet xAODMissingET xAODMuon xAODPFlow xAODParticleEvent xAODTau xAODTracking xAODTruth ExpressionEvaluationLib )

atlas_add_test( ExpressionParser_test
                SOURCES test/ExpressionParser_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools CxxUtils ${ROOT_LIBRARIES} ExpressionEvaluationLib xAODTracking xAODMuon
                LOG_IGNORE_PATTERN "^AtlasFieldSvc +INFO"
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
