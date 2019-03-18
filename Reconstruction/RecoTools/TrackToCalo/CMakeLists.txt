################################################################################
# Package: TrackToCalo
################################################################################

# Declare the package name:
atlas_subdir( TrackToCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecTools
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Event/FourMomUtils
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODMuon
			  Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          Reconstruction/RecoTools/ParticlesInConeTools
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrackToCaloLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackToCalo
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers GeoPrimitives xAODCaloEvent GaudiKernel ParticleCaloExtension RecoToolInterfaces 
		   TrackCaloClusterRecTools TrkCaloExtension TrkParametersIdentificationHelpers CaloDetDescrLib CaloUtilsLib
		   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaBaseComps AtlasDetDescr FourMomUtils xAODTracking xAODMuon xAODEgamma xAODTruth TrkSurfaces 
		   TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces )

atlas_add_component( TrackToCalo
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers GeoPrimitives xAODCaloEvent GaudiKernel ParticleCaloExtension RecoToolInterfaces TrackCaloClusterRecTools TrkCaloExtension TrkParametersIdentificationHelpers CaloDetDescrLib CaloIdentifier CaloUtilsLib AthenaBaseComps AtlasDetDescr FourMomUtils xAODMuon xAODTracking xAODTruth TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces TrackToCaloLib )

