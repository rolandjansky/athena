################################################################################
# Package: MuonCalibExtraUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraUtils )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraUtils
                   LINK_LIBRARIES GeoPrimitives MuonCalibIdentifier )

