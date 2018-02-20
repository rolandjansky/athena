################################################################################
# Package: TrackCaloClusterRecAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecAlgs )

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
                          Event/xAOD/xAODPFlow     
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecTools)

find_package( FastJet )
find_package( FastJetContrib ) 
                          
# Component(s) in the package:
atlas_add_component( TrackCaloClusterRecAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} AthenaBaseComps GaudiKernel AtlasDetDescr xAODBase 
                     xAODTracking xAODAssociations xAODPFlow TrackCaloClusterRecTools TrkParametersIdentificationHelpers )
                     