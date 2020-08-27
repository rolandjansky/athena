################################################################################
# Package: egammaTools
################################################################################

# Declare the package name:
atlas_subdir( egammaTools )


atlas_add_component( egammaTools
	src/*.cxx
	src/components/*.cxx
	INCLUDE_DIRS
	LINK_LIBRARIES CaloConditions CaloEvent  CaloIdentifier CaloRecLib CaloUtilsLib AthenaBaseComps AthenaKernel
	AthContainers StoreGateLib  Identifier EventKernel EventPrimitives FourMom FourMomUtils  xAODCaloEvent
	xAODEgamma xAODTracking xAODTruth GaudiKernel LArRecConditions LArCablingLib LumiBlockCompsLib PATCoreLib
	EgammaAnalysisInterfacesLib egammaRecEvent egammaUtils TrkEventPrimitives egammaInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
