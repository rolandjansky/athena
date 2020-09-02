################################################################################
# Package: MuonCalibITools
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibITools )

# Component(s) in the package:
atlas_add_library( MuonCalibITools
                   PUBLIC_HEADERS MuonCalibITools
                   LINK_LIBRARIES Identifier GaudiKernel MuonCalibEventBase MuonCalibIdentifier )

