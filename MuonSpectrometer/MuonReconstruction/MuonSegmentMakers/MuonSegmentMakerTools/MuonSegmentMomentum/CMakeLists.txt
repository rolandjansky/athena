################################################################################
# Package: MuonSegmentMomentum
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMomentum )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonSegmentMomentumLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions
 MuonRecToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives MuonSegment TrkGeometry TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkExInterfaces TrkExUtils )

atlas_add_component( MuonSegmentMomentum
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions
 MuonRecToolInterfaces GeoPrimitives MuonIdHelpersLib MuonSegment TrkGeometry TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkExInterfaces TrkExUtils MuonSegmentMomentumLib )

