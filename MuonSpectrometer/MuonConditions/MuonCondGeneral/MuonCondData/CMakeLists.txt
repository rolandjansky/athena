################################################################################
# Package: MuonCondData
################################################################################

# Declare the package name:
atlas_subdir( MuonCondData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_library( MuonCondData
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondData
                   LINK_LIBRARIES DataModel SGTools AthenaPoolUtilities Identifier StoreGateLib SGtests )

