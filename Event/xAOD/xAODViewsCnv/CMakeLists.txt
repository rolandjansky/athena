################################################################################
# Package: xAODViewsCnv
################################################################################

 

# Declare the package name:
atlas_subdir( xAODViewsCnv )

 

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Tracking/TrkEvent/TrkTrack
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/xAOD/xAODViews )

# Component(s) in the package:

atlas_add_poolcnv_library( xAODViewsCnvPoolCnv
                           FILES xAODViews/TrackParticleViewVector.h xAODViews/VertexViewVector.h
                           INCLUDE_DIRS xAODViews
                           TYPES_WITH_NAMESPACE xAOD::TrackParticleViewVector xAOD::VertexViewVector
                           CNV_PFX xAOD 
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib TrkTrack xAODTracking)

