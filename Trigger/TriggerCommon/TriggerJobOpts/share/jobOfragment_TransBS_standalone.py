# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#--------------------------------------------------------------
# Write to Transient BS
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp, ServiceMgr
from AthenaCommon.Include import include
from AthenaServices.AthenaServicesConf import AthenaOutputStream
from TriggerJobOpts.TriggerFlags import TriggerFlags

include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )

# Configure Transient BS Output Stream. ExtraInputs make sure all data (e.g. conditions)
# needed for BS encoding are scheduled to be produced before StreamBS
StreamBS = AthenaOutputStream("StreamBS",
                              EvtConversionSvc = "ByteStreamCnvSvc",
                              ExtraInputs = [('xAOD::EventInfo','StoreGateSvc+EventInfo')])

#Ensure StreamBS runs before L1Decoder
transTypeKey = ("TransientBSOutType","StoreGateSvc+TransientBSOutKey")
StreamBS.ExtraOutputs += [transTypeKey]

if not TriggerFlags.fakeLVL1():
   from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamEncodersRecExSetup
   L1ByteStreamEncodersRecExSetup()
   StreamBS.ItemList += ["ROIB::RoIBResult#*"]
   StreamBS.ExtraInputs += [('ROIB::RoIBResult', 'StoreGateSvc+RoIBResult')]

if TriggerFlags.doID():
   # TRT
   StreamBS.ItemList += ["TRT_RDO_Container#*"]
   # SCT
   StreamBS.ItemList += ["SCT_RDO_Container#*"]
   StreamBS.ExtraInputs += [('SCT_CablingData','ConditionStore+SCT_CablingData')]
   # Pixel
   StreamBS.ItemList += ["PixelRDO_Container#*"]
   StreamBS.ExtraInputs += [('PixelHitDiscCnfgData','ConditionStore+PixelHitDiscCnfgData')]
   StreamBS.ExtraInputs += [('PixelCablingCondData','ConditionStore+PixelCablingCondData')]

if TriggerFlags.doCalo():
   # LAr
   theApp.Dlls += ["LArCalibUtils"]
   theApp.Dlls += ["LArByteStream"]
   StreamBS.ItemList += ["LArRawChannelContainer#*"]
   from AthenaCommon.AppMgr import ToolSvc
   from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
   ToolSvc += LArRawDataContByteStreamToolConfig(InitializeForWriting=True)
   # Tile
   theApp.Dlls += ["TileByteStream"]
   StreamBS.ItemList += ["TileRawChannelContainer#*"]
   StreamBS.ExtraInputs += [('TileBadChannels','ConditionStore+TileBadChannels')]

if TriggerFlags.doMuon():
   theApp.Dlls += ["MuonByteStream" ]
   # MDT
   StreamBS.ItemList += ["MdtCsmContainer#*"]
   StreamBS.ExtraInputs += [('MuonMDT_CablingMap','ConditionStore+MuonMDT_CablingMap')]
   # RPC
   StreamBS.ItemList += ["RpcPadContainer#*"]
   # TGC
   StreamBS.ItemList += ["TgcRdoContainer#*"]
   # CSC
   StreamBS.ItemList += ["CscRawDataContainer#*"]

#--------------------------------------------------------------
# List types to be read from transient BS in the BS address provider
#--------------------------------------------------------------
if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
   from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
   ServiceMgr += ByteStreamAddressProviderSvc()

if TriggerFlags.doCalo():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
      "TileCellIDC/TileCellIDC"]

if TriggerFlags.doID():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
      "InDet::PixelClusterContainer/PixelOnlineClusters",
      "InDet::SCT_ClusterContainer/SCT_OnlineClusters",
      "InDet::TRT_DriftCircleContainer/TRT_DriftCircle"]

# Muon digits not under doMuon() because needed for LVL1 simulation
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
      "MdtDigitContainer/MDT_DIGITS",
      "RpcDigitContainer/RPC_DIGITS",
      "TgcDigitContainer/TGC_DIGITS",
      "CscDigitContainer/CSC_DIGITS"]

if TriggerFlags.doMuon():
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
      "RpcPadContainer/RPCPAD",
      "MdtCsmContainer/MDTCSM",
      "TgcRdoContainer/TGCRDO",
      "CscRawDataContainer/CSCRDO",
      "MuCTPI_RDO/MUCTPI_RDO"]

# Tell the central proxy provider that some proxies may come from BS address provider
if not hasattr(ServiceMgr,"ProxyProviderSvc"):
   from SGComps.SGCompsConf import ProxyProviderSvc
   ServiceMgr += ProxyProviderSvc()
ServiceMgr.ProxyProviderSvc.ProviderNames += ["ByteStreamAddressProviderSvc"]
