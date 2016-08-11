################################################################################
# Package: TrkViewsCnv
################################################################################

# Declare the package name:

atlas_subdir( TrkViewsCnv )

# Declare the package's dependencies:

atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Tracking/TrkEvent/TrkViews )

# Component(s) in the package:

atlas_add_poolcnv_library( TrkViewsCnvPoolCnv
                           FILES TrkViews/TrackViewVector.h
                           INCLUDE_DIRS TrkViews
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib TrkTrack )
