################################################################################
# Package: TrackCaloClusterAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          DetectorDescription/AtlasDetDescr
                          PUBLIC
                          GaudiKernel 
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODAssociations
                          Event/xAOD/xAODTrackCaloCluster
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecInterfaces                    )

# Component(s) in the package:
atlas_add_component( TrackCaloClusterAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel AtlasDetDescr xAODBase xAODTracking xAODAssociations xAODTrackCaloCluster TrkParametersIdentificationHelpers )

