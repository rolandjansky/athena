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
                          Control/DataModel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          Event/EventPrimitives
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/FourMomUtils
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
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
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( egammaTrackTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib CaloGeoHelpers AthenaBaseComps DataModel AtlasDetDescr IdDictDetDescr EventPrimitives xAODCaloEvent xAODEgamma xAODTracking FourMomUtils GaudiKernel InDetIdentifier RecoToolInterfaces TrkSurfaces TrkCaloCluster_OnTrack TrkCaloExtension TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkNeutralParameters TrkParameters TrkParametersIdentificationHelpers TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexOnTrack VxVertex TrkExInterfaces TrkFitterInterfaces TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( egammaTrackTools )
atlas_install_python_modules( python/*.py )

