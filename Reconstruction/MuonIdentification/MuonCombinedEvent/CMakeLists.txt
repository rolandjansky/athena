################################################################################
# Package: MuonCombinedEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCombinedEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCombinedEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel AthLinks AthContainers GeoPrimitives Identifier xAODMuon xAODTracking MuidEvent muonEvent TrkEventPrimitives TrkMaterialOnTrack TrkParameters MuonLayerEvent
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonSegment TrkTrack )

