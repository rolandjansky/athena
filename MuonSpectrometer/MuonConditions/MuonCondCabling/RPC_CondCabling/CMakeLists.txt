################################################################################
# Package: RPC_CondCabling
################################################################################

# Declare the package name:
atlas_subdir( RPC_CondCabling )

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

