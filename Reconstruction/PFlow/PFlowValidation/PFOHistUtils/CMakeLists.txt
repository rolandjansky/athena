################################################################################
# Package: PFOHistUtils
################################################################################

# Declare the package name:
atlas_subdir( PFOHistUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPFlow
			  Event/xAOD/xAODTracking
                          Tracking/TrkValidation/TrkValHistUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PFOHistUtils
                   src/*.cxx
                   PUBLIC_HEADERS PFOHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEvent xAODPFlow TrkValHistUtils )

