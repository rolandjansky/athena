################################################################################
# Package: MuonByteStreamCnvTest
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStreamCnvTest )

# Component(s) in the package:
atlas_add_library( MuonByteStreamCnvTestLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonByteStreamCnvTest
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonReadoutGeometry MuonRDO StoreGateLib SGtests RPCcablingInterfaceLib TrigT1RPChardwareLib TrigT1RPClogicLib MuonMDT_CablingLib TGCcablingInterfaceLib MuonIdHelpersLib RPC_CondCablingLib MuonDigToolInterfacesLib CscCalibToolsLib MuonDigitContainer
                   PRIVATE_LINK_LIBRARIES EventInfo MuonPrepRawData MuonCablingData MuonMDT_CnvToolsLib MuonMM_CnvToolsLib MuonCSC_CnvToolsLib MuonRPC_CnvToolsLib MuonTGC_CnvToolsLib MuonSTGC_CnvToolsLib EventInfoMgtLib )

atlas_add_component( MuonByteStreamCnvTest
                     src/components/*.cxx
                     LINK_LIBRARIES  MuonByteStreamCnvTestLib MuonCSC_CnvToolsLib MuonMDT_CnvToolsLib MuonRPC_CnvToolsLib MuonTGC_CnvToolsLib MuonSTGC_CnvToolsLib MuonMM_CnvToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

