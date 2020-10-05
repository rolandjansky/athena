################################################################################
# Package: MuonCablingServers
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingServers )

# Component(s) in the package:
atlas_add_library( MuonCablingServersLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingServers
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel RPCcablingInterfaceLib TGCcablingInterfaceLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES EventInfo EventInfoMgtLib )

atlas_add_component( MuonCablingServers
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel RPCcablingInterfaceLib TGCcablingInterfaceLib StoreGateLib SGtests EventInfo MuonCablingServersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

