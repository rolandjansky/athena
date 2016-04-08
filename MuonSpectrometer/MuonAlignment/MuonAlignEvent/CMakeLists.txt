################################################################################
# Package: MuonAlignEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkAlignment/TrkAlignEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry )

# Component(s) in the package:
atlas_add_library( MuonAlignEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonAlignEvent
                   LINK_LIBRARIES TrkAlignEvent
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps MuonReadoutGeometry )

