################################################################################
# Package: MuonCombinedToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedToolInterfaces )

# Component(s) in the package:
atlas_add_library( MuonCombinedToolInterfaces
                   PUBLIC_HEADERS MuonCombinedToolInterfaces
                   LINK_LIBRARIES CaloEvent xAODCaloEvent xAODMuon xAODTracking GaudiKernel MuonLayerEvent MuidEvent MuonCombinedEvent TrkParameters TrkTrack )

