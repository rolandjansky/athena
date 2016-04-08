################################################################################
# Package: MuonSpShowerBuilderAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonSpShowerBuilderAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( MuonSpShowerBuilderAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonSpShowerBuilderAlgs
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES xAODJet xAODMuon MuonReadoutGeometry MuonPrepRawData JetEvent muonEvent TrkRIO_OnTrack )

atlas_add_component( MuonSpShowerBuilderAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODJet xAODMuon MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData JetEvent muonEvent TrkRIO_OnTrack MuonSpShowerBuilderAlgsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

