# Bytestream to TrigT1Calo objects conversions for data with Run 2 formats
include.block("L1TopoByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

from L1TopoByteStream.L1TopoByteStreamConf import L1TopoByteStreamTool

ToolSvc = Service("ToolSvc")
ToolSvc += L1TopoByteStreamTool("L1TopoByteStreamTool")

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )

# =============================================================================
# TOPO
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::L1TopoRawDataContainer/L1TopoRawData"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::L1TopoRawDataAuxContainer/L1TopoRawDataAux."]

ByteStreamAddressProviderSvc.TypeNames += [ "L1TopoRDOCollection/L1TopoRDOCollection"]

