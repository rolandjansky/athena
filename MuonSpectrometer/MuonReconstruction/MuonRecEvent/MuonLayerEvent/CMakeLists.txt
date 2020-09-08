################################################################################
# Package: MuonLayerEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonLayerEvent )

# Component(s) in the package:
atlas_add_library( MuonLayerEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonLayerEvent
                   LINK_LIBRARIES AthContainers xAODCore MuonPrepRawData TrkParameters MuonStationIndexLib
                   PRIVATE_LINK_LIBRARIES MuonRIO_OnTrack )

