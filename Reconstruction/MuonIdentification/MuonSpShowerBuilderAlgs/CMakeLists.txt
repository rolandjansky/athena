################################################################################
# Package: MuonSpShowerBuilderAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonSpShowerBuilderAlgs )

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

