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
