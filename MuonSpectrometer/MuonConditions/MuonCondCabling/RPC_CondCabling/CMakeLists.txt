################################################################################
# Package: RPC_CondCabling
################################################################################

# Declare the package name:
atlas_subdir( RPC_CondCabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonCablings/MuonCablingTools
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonIdHelpers
                          Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( RPC_CondCablingLib
                   src/*.cxx
                   PUBLIC_HEADERS RPC_CondCabling
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel MuonCondInterface MuonCablingTools SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier MuonIdHelpersLib PathResolver )

atlas_add_component( RPC_CondCabling
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES}  RPC_CondCablingLib)

