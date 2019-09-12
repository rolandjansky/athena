################################################################################
# Package: egammaTools
################################################################################

# Declare the package name:
atlas_subdir( egammaTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
	Calorimeter/CaloConditions
	Calorimeter/CaloEvent
	Calorimeter/CaloIdentifier
	Calorimeter/CaloInterface
	Calorimeter/CaloRec
	Calorimeter/CaloUtils
	Control/AthenaBaseComps
	Control/AthenaKernel
	Control/StoreGate
	Control/AthContainers
	DetectorDescription/Identifier
	DetectorDescription/GeoPrimitives
	Event/EventKernel
	Event/EventPrimitives
	Event/FourMom
	Event/FourMomUtils
	Event/xAOD/xAODCaloEvent
	Event/xAOD/xAODEgamma
	Event/xAOD/xAODTracking
	Event/xAOD/xAODTruth
	GaudiKernel
	LArCalorimeter/LArRecConditions
	LArCalorimeter/LArCabling
	LumiBlock/LumiBlockComps
	PhysicsAnalysis/AnalysisCommon/PATCore
	PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
	Reconstruction/egamma/egammaInterfaces
	Reconstruction/egamma/egammaRecEvent
	Reconstruction/egamma/egammaUtils
	Tracking/TrkEvent/TrkEventPrimitives
	)


atlas_add_component( egammaTools
	src/*.cxx
	src/components/*.cxx
	INCLUDE_DIRS
	LINK_LIBRARIES CaloConditions CaloEvent  CaloIdentifier CaloRecLib CaloUtilsLib AthenaBaseComps AthenaKernel 
	AthContainers StoreGateLib  Identifier EventKernel EventPrimitives FourMom FourMomUtils  xAODCaloEvent  
	xAODEgamma xAODTracking xAODTruth GaudiKernel LArRecConditions LArCablingLib LumiBlockCompsLib PATCoreLib 
	EgammaAnalysisInterfacesLib egammaRecEvent egammaUtils TrkEventPrimitives )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check python syntax on Config files
atlas_add_test( flake8
                SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*Config.py
                POST_EXEC_SCRIPT nopost.sh )
