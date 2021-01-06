################################################################################
# Package: MuonMDT_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonMDT_Cabling )

# Component(s) in the package:
atlas_add_library( MuonMDT_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonMDT_Cabling
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests MuonIdHelpersLib MuonCondInterface MuonCablingData AthenaPoolUtilities EventInfoMgtLib
                   PRIVATE_LINK_LIBRARIES  Identifier MuonCondSvcLib PathResolver )

atlas_add_component( MuonMDT_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel EventInfoMgtLib MuonMDT_CablingLib )

