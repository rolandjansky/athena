################################################################################
# Package: MuonIdHelpersAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonIdHelpersAlgs )

# Component(s) in the package:
atlas_add_library( MuonIdHelpersAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonIdHelpersAlgs
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES MuonReadoutGeometry MuonDigitContainer )

atlas_add_component( MuonIdHelpersAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonIdHelpersAlgsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

