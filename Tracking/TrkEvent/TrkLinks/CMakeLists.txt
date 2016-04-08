################################################################################
# Package: TrkLinks
################################################################################

# Declare the package name:
atlas_subdir( TrkLinks )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackLink )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkLinks
                   src/*.cxx
                   PUBLIC_HEADERS TrkLinks
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel xAODTracking TrkNeutralParameters TrkParameters )

