################################################################################
# Package: TrkValHistUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkValHistUtils )

set( extra_deps )
set( extra_libs )
if( NOT XAOD_ANALYSIS )
    set( extra_dep Tracking/TrkEvent/TrkEventPrimitives )
    set( extra_libs TrkEventPrimitives )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PRIVATE
                          Event/EventPrimitives
                          ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkValHistUtils
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrkValHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODTracking xAODTruth
                   PRIVATE_LINK_LIBRARIES EventPrimitives ${extra_libs} )

