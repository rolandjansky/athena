################################################################################
# Package: egammaAlgs
################################################################################

# Declare the package name:
atlas_subdir( egammaAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/EventKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
			  Calorimeter/CaloDetDescr
		    	  Control/AthenaKernel
                          Control/StoreGate
                          Event/FourMomUtils
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Reconstruction/egamma/egammaRecEvent
                          Reconstruction/egamma/egammaMVACalib
                          Reconstruction/egamma/egammaUtils
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
			  Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
			  Tracking/TrkTools/TrkToolInterfaces 
			  Tracking/TrkEvent/TrkCaloExtension)
 


atlas_add_component( egammaAlgs
		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
		     LINK_LIBRARIES AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel 
		     MCTruthClassifierLib CaloGeoHelpers CaloUtilsLib CaloDetDescrLib AthenaKernel  StoreGateLib 
		     xAODTracking InDetReadoutGeometry EgammaAnalysisInterfacesLib egammaRecEvent egammaUtils
		     TrkToolInterfaces InDetRecToolInterfaces FourMomUtils RecoToolInterfaces TrkTrack 
		     TrkPseudoMeasurementOnTrack InDetConversionFinderToolsLib TrkCaloExtension)
	     
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
