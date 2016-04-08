################################################################################
# Package: MuonSegmentTrackMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTrackMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( MuonSegmentTrackMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces Particle TrkSegment TrkTrack )

# Install files from the package:
atlas_install_headers( MuonSegmentTrackMaker )

