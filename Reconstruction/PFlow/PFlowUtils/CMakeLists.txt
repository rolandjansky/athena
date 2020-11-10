# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PFlowUtils )

# Component(s) in the package:
atlas_add_library( PFlowUtilsLib
   PFlowUtils/*.h Root/*.cxx
   PUBLIC_HEADERS PFlowUtils
   LINK_LIBRARIES AsgTools xAODPFlow )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PFlowUtils
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthContainers AthenaBaseComps xAODJet xAODPFlow xAODTruth
      xAODCore PFlowUtilsLib CaloCalibHitRecLib CaloSimEvent StoreGateLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
