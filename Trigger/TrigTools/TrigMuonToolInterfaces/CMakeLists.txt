################################################################################
# Package: TrigMuonToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
			  Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TrigMuonToolInterfaces )

