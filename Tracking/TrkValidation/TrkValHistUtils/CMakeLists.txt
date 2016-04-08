################################################################################
# Package: TrkValHistUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkValHistUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          Event/EventPrimitives )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkValHistUtils
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrkValHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODTracking xAODTruth TrkEventPrimitives
                   PRIVATE_LINK_LIBRARIES EventPrimitives )

