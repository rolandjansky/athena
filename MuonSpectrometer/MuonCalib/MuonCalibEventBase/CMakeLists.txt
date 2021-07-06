################################################################################
# Package: MuonCalibEventBase
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibEventBase )

# Component(s) in the package:
atlas_add_library( MuonCalibEventBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibEventBase
                   LINK_LIBRARIES GeoPrimitives MuonCalibIdentifier MuonRDO MuonCalibStl )

