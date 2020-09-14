################################################################################
# Package: MdtVsRpcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MdtVsRpcRawDataMonitoring )

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( MdtVsRpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringLib StoreGateLib SGtests GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonIdHelpersLib MuonDQAUtilsLib Identifier MuonRDO MuonPrepRawData )

# Install files from the package:
atlas_install_headers( MdtVsRpcRawDataMonitoring )
atlas_install_joboptions( share/*.py )

