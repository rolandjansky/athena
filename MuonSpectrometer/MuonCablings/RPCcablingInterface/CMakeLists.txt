################################################################################
# Package: RPCcablingInterface
################################################################################

# Declare the package name:
atlas_subdir( RPCcablingInterface )

# Component(s) in the package:
atlas_add_library( RPCcablingInterfaceLib
                   src/*.cxx
                   PUBLIC_HEADERS RPCcablingInterface
                   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel MuonCablingTools MuonIdHelpersLib StoreGateLib SGtests RPC_CondCablingLib)

