################################################################################
# Package: InDetCompetingRIOsOnTrack
################################################################################

# Declare the package name:
atlas_subdir( InDetCompetingRIOsOnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CxxUtils
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetCompetingRIOsOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS InDetCompetingRIOsOnTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils InDetRIO_OnTrack TrkCompetingRIOsOnTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrkSurfaces )

