################################################################################
# Package: MuonCombinedToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_library( MuonCombinedToolInterfaces
                   PUBLIC_HEADERS MuonCombinedToolInterfaces
                   LINK_LIBRARIES CaloEvent xAODCaloEvent xAODMuon xAODTracking GaudiKernel MuonLayerEvent MuidEvent MuonCombinedEvent TrkParameters TrkTrack )

