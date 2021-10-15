# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaTools )

# Component(s) in the package:
atlas_add_component( egammaTools
   src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloInterfaceLib CaloRecLib CaloUtilsLib EgammaAnalysisInterfacesLib EventPrimitives FourMom FourMomUtils GaudiKernel GeoPrimitives Identifier LArCablingLib LArRecConditions LumiBlockCompsLib PATCoreAcceptLib SGTools StoreGateLib TrkEventPrimitives egammaInterfacesLib egammaRecEvent egammaUtils xAODCaloEvent xAODEgamma xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test(EMTrackMatchBuilderConfigTest
               SCRIPT python -m egammaTools.EMTrackMatchBuilderConfig
               POST_EXEC_SCRIPT nopost.sh)

atlas_add_test(EMShowerBuilderConfigTest
               SCRIPT python -m egammaTools.EMShowerBuilderConfig
               POST_EXEC_SCRIPT nopost.sh)

atlas_add_test(EMConversionBuilderConfigTest
               SCRIPT python -m egammaTools.EMConversionBuilderConfig
               POST_EXEC_SCRIPT nopost.sh)

atlas_add_test(EMPIDBuilderConfigTest
               SCRIPT python -m egammaTools.EMPIDBuilderConfig
               POST_EXEC_SCRIPT nopost.sh)

 atlas_add_test(egammaSwToolConfigTest
               SCRIPT python -m egammaTools.egammaSwToolConfig
               POST_EXEC_SCRIPT nopost.sh)              

atlas_add_test(egammaOQFlagsBuilderTest
               SCRIPT python -m egammaTools.egammaOQFlagsBuilderConfig
               POST_EXEC_SCRIPT nopost.sh) 

atlas_add_test(egammaLargeClusterMakerConfigTest
               SCRIPT python -m egammaTools.egammaLargeClusterMakerConfig
               POST_EXEC_SCRIPT nopost.sh) 
