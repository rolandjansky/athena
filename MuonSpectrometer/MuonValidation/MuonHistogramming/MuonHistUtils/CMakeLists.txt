################################################################################
# Package: MuonHistUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonHistUtils )

set( extra_libs )
if( NOT XAOD_ANALYSIS )
    set( extra_libs TrkEventPrimitives )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonHistUtils
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS MuonHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMuon xAODTracking xAODTrigMuon xAODTrigger xAODTruth TrkValHistUtils MuonStationIndexLib FourMomUtils
                   PRIVATE_LINK_LIBRARIES GeoPrimitives ${extra_libs} )

