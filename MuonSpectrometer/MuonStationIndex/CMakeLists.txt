################################################################################
# Package: MuonStationIndex
################################################################################

# Declare the package name:
atlas_subdir( MuonStationIndex )

# This package doesn't depend on anything.

# The main library of the package:
atlas_add_library( MuonStationIndexLib
   MuonStationIndex/*.h Root/*.cxx
   PUBLIC_HEADERS MuonStationIndex )
