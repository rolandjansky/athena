################################################################################
# Package: MuonCombinedEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEvent )

# Component(s) in the package:
atlas_add_library( MuonCombinedEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCombinedEvent
                   LINK_LIBRARIES AthenaKernel AthLinks AthContainers GeoPrimitives Identifier xAODMuon xAODTracking MuidEvent muonEvent TrkEventPrimitives TrkMaterialOnTrack TrkParameters MuonLayerEvent TrkCaloExtension
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonSegment TrkTrack )

