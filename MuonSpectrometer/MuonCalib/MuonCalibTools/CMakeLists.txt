################################################################################
# Package: MuonCalibTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibTools )

# Component(s) in the package:
atlas_add_library( MuonCalibToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibTools
                   LINK_LIBRARIES AthenaBaseComps Identifier MuonCalibITools MuonReadoutGeometry StoreGateLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonCalibEventBase MuonCalibIdentifier  )

atlas_add_component( MuonCalibTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier MuonCalibITools MuonReadoutGeometry StoreGateLib GaudiKernel MuonCalibEventBase MuonCalibIdentifier  MuonIdHelpersLib MuonCalibToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

