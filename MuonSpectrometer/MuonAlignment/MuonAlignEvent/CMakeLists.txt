################################################################################
# Package: MuonAlignEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignEvent )

# Component(s) in the package:
atlas_add_library( MuonAlignEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonAlignEvent
                   LINK_LIBRARIES TrkAlignEvent
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps MuonReadoutGeometry )

