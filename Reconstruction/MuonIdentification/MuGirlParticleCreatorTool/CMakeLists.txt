################################################################################
# Package: MuGirlParticleCreatorTool
################################################################################

# Declare the package name:
atlas_subdir( MuGirlParticleCreatorTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Reconstruction/MuonIdentification/MuidInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/TrkPrepRawData
                          Trigger/TrigEvent/TrigMuonEvent )

# Component(s) in the package:
atlas_add_component( MuGirlParticleCreatorTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuGirlInterfaces MuidInterfaces xAODEventInfo xAODTracking MuonSegment muonEvent TrkPrepRawData TrigMuonEvent )

# Install files from the package:
atlas_install_headers( MuGirlParticleCreatorTool )

