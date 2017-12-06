################################################################################
# Package: MuonCombinedAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
			  Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonCombinedAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODCaloEvent xAODMuon xAODTruth xAODTracking GaudiKernel MuonSegment MuonCombinedEvent MuonCombinedToolInterfaces TrkSegment TrkTrack TrkToolInterfaces)

