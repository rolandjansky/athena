################################################################################
# Package: MuonEDM_AssociationObjects
################################################################################

# Declare the package name:
atlas_subdir( MuonEDM_AssociationObjects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment )

# Component(s) in the package:
atlas_add_library( MuonEDM_AssociationObjects
                   src/*.cxx
                   PUBLIC_HEADERS MuonEDM_AssociationObjects
                   LINK_LIBRARIES DataModel SGTools MuonPattern MuonSegment )

