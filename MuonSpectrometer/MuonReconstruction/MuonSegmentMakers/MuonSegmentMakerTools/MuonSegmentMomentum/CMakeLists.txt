################################################################################
# Package: MuonSegmentMomentum
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMomentum )

# Component(s) in the package:
atlas_add_library( MuonSegmentMomentumLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions
 MuonRecToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES GeoPrimitives MuonSegment TrkGeometry TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkExInterfaces TrkExUtils )

atlas_add_component( MuonSegmentMomentum
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldElements MagFieldConditions
 MuonRecToolInterfaces GeoPrimitives MuonIdHelpersLib MuonSegment TrkGeometry TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkExInterfaces TrkExUtils MuonSegmentMomentumLib )

