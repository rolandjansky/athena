################################################################################
# Package: TrackCaloClusterAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel 
                          PUBLIC
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODTracking
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecInterfaces                         
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# Component(s) in the package:
atlas_add_component( TrackCaloClusterAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrackCaloClusterRecInterfaces AtlasDetDescr xAODTracking TrkParametersIdentificationHelpers )

