 ################################################################################
 # Package: TrackCaloClusterRecAlgs
 ################################################################################
 
 # Declare the package name:
 atlas_subdir( TrackCaloClusterRecAlgs )
 
 
 find_package( FastJet )
 find_package( FastJetContrib ) 
                           
 # Component(s) in the package:
 atlas_add_component( TrackCaloClusterRecAlgs
                      src/*.cxx
                      src/components/*.cxx
                      INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
                      LINK_LIBRARIES ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} AthenaBaseComps GaudiKernel AtlasDetDescr xAODBase 
                      xAODTracking xAODAssociations xAODPFlow TrackCaloClusterRecToolsLib TrkParametersIdentificationHelpers )
                      
