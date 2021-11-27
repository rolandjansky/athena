################################################################################
# Package: egammaAlgs
################################################################################

# Declare the package name:
atlas_subdir( egammaAlgs )

atlas_add_component( egammaAlgs
		     src/*.cxx
             src/components/*.cxx
             INCLUDE_DIRS
		     LINK_LIBRARIES AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel
		     MCTruthClassifierLib CaloGeoHelpers CaloUtilsLib CaloDetDescrLib AthenaKernel  StoreGateLib
		     xAODTracking InDetReadoutGeometry EgammaAnalysisInterfacesLib egammaRecEvent egammaUtils
		     TrkToolInterfaces InDetRecToolInterfaces FourMomUtils RecoToolInterfaces TrkTrack
		     TrkPseudoMeasurementOnTrack InDetConversionFinderToolsLib TrkCaloExtension egammaInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( EMVertexBuilderConfig_test
    SCRIPT python -m egammaAlgs.EMVertexBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( EMBremCollectionBuilderConfig_test
    SCRIPT python -m egammaAlgs.EMBremCollectionBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaRecBuilderConfig_test
    SCRIPT python -m egammaAlgs.egammaRecBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaSuperClusterBuilderConfig_test
    SCRIPT python -m egammaAlgs.egammaSuperClusterBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaSelectedTrackCopyConfig_test
    SCRIPT python -m egammaAlgs.egammaSelectedTrackCopyConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaTopoClusterCopierConfig_test
    SCRIPT python -m egammaAlgs.egammaTopoClusterCopierConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( topoEgammaBuilderConfig_test
    SCRIPT python -m egammaAlgs.topoEgammaBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaForwardBuilderConfig_test
    SCRIPT python -m egammaAlgs.egammaForwardBuilderConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaTruthAssociationConfig_test
    SCRIPT python -m egammaAlgs.egammaTruthAssociationConfig
    POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( egammaTrackSlimmerConfig_test
    SCRIPT python -m egammaAlgs.egammaTrackSlimmerConfig
    POST_EXEC_SCRIPT nopost.sh)

