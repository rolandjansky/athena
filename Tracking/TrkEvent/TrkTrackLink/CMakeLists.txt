################################################################################
# Package: TrkTrackLink
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackLink )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrkTrackLinkDict
                      TrkTrackLink/TrkTrackLinkDict.h
                      TrkTrackLink/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrkNeutralParameters TrkParameters )

# Install files from the package:
atlas_install_headers( TrkTrackLink )

