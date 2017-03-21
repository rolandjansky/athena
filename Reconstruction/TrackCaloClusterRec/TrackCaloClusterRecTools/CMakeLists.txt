################################################################################
# Package: TrackCaloClusterRecTools
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODTrackCaloCluster
                          Event/xAOD/xAODAssociations
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecInterfaces                         
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrackCaloClusterRecToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackCaloClusterRecTools
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrkCaloExtension TrkParametersIdentificationHelpers 
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AtlasDetDescr xAODMuon xAODTracking xAODTruth xAODTrackCaloCluster xAODAssociations TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrackVertexAssociationTool)

atlas_add_component( TrackCaloClusterRecTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} GaudiKernel TrkCaloExtension TrkParametersIdentificationHelpers 
                     AthenaBaseComps AtlasDetDescr xAODMuon xAODTracking xAODTruth xAODTrackCaloCluster xAODAssociations TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrackVertexAssociationTool TrackCaloClusterRecToolsLib )
