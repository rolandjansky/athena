################################################################################
# Package: MuonPrdSelector
################################################################################

# Declare the package name:
atlas_subdir( MuonPrdSelector )

atlas_add_library( MuonPrdSelectorLib
                   MuonPrdSelector/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonPrdSelector
                   LINK_LIBRARIES GaudiKernel MuonCalibToolsLib MuonCalibIdentifier Identifier AthenaBaseComps MuonCalibToolsLib MuonIdHelpersLib MuonPrepRawData )

# Component(s) in the package:
atlas_add_component( MuonPrdSelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES MuonPrdSelectorLib StoreGateLib SGtests MuonReadoutGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )

