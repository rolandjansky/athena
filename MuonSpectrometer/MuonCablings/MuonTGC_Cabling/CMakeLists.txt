################################################################################
# Package: MuonTGC_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonTGC_Cabling )

# Component(s) in the package:
atlas_add_library( MuonTGC_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonTGC_Cabling
                   LINK_LIBRARIES AthenaKernel GaudiKernel MuonCondInterface TGCcablingInterfaceLib StoreGateLib MuonIdHelpersLib CxxUtils
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities PathResolver )

atlas_add_component( MuonTGC_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel TGCcablingInterfaceLib MuonCondInterface StoreGateLib AthenaPoolUtilities MuonIdHelpersLib PathResolver MuonTGC_CablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/*.db )

