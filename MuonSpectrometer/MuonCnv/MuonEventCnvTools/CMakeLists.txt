################################################################################
# Package: MuonEventCnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonEventCnvTools )

# Component(s) in the package:
atlas_add_component( MuonEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack TrkDetElementBase TrkPrepRawData TrkRIO_OnTrack TrkEventCnvToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

