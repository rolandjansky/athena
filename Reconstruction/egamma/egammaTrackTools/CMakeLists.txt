################################################################################
# Package: egammaTrackTools
################################################################################

# Declare the package name:
atlas_subdir( egammaTrackTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTrackingGeometry
                          Calorimeter/CaloUtils
			  Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          Event/EventPrimitives
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/FourMomUtils
                          GaudiKernel
			  InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCaloCluster_OnTrack
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkVertexOnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )


# Component(s) in the package:
atlas_add_component( egammaTrackTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib CaloGeoHelpers AthenaBaseComps AthContainers 
		     AtlasDetDescr IdDictDetDescr EventPrimitives xAODCaloEvent xAODEgamma xAODTracking FourMomUtils GaudiKernel BeamSpotConditionsData InDetIdentifier 
		     RecoToolInterfaces TrkSurfaces TrkCaloCluster_OnTrack TrkCaloExtension TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase 
		     TrkNeutralParameters TrkParameters TrkParametersIdentificationHelpers TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack 
		     TrkVertexOnTrack TrkExInterfaces TrkFitterInterfaces TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check python syntax on Config files
atlas_add_test( flake8
                SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*Config.py
                POST_EXEC_SCRIPT nopost.sh )
