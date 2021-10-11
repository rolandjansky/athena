################################################################################
# Package: MuonCablingServers
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingServers )

# Component(s) in the package:
atlas_add_library( MuonCablingServersLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingServers
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TGCcablingInterfaceLib StoreGateLib
                   PRIVATE_LINK_LIBRARIES EventInfoMgtLib )

atlas_add_component( MuonCablingServers
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TGCcablingInterfaceLib StoreGateLib EventInfoMgtLib MuonCablingServersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

