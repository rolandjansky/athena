# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigHLTJetHypoLib
                   src/*.cxx
                   src/TrigHLTJetHypoUtils/*.cxx
                   PUBLIC_HEADERS TrigHLTJetHypo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps DecisionHandlingLib GaudiKernel TrigCompositeUtilsLib TrigInterfacesLib TrigParticle TrigTimeAlgsLib xAODJet
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringKernelLib CaloGeoHelpers TrigMissingEtEvent xAODEventInfo xAODTrigger )

atlas_add_component( TrigHLTJetHypo
                     src/components/*.cxx
                     LINK_LIBRARIES TrigHLTJetHypoLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} ) 

# Tests:
atlas_add_test( flake8_share
                SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigHLTJetHypoTool SCRIPT python -m TrigHLTJetHypo.TrigJetHypoToolConfig
				POST_EXEC_SCRIPT nopost.sh )
