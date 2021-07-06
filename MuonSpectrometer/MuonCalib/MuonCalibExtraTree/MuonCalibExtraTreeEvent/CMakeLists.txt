################################################################################
# Package: MuonCalibExtraTreeEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraTreeEvent )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraTreeEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraTreeEvent
                   LINK_LIBRARIES GeoPrimitives MuonCalibEventBase MuonCalibExtraUtils MuonCalibIdentifier )

