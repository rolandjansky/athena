include.block ( "EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import OverlayFlags

#copy stuff back into the MC before persistency
from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyObjects

if OverlayFlags.doTruth():
   job += CopyObjects("CopyTruth")
   job.CopyTruth.TruthObjects = True

# copy InDet objects back into Signal event store
if OverlayFlags.doPixel() or OverlayFlags.doSCT() or OverlayFlags.doTRT():
   job += CopyObjects("CopyInDet")
   job.CopyInDet.InDetObjects = True

if OverlayFlags.doCSC() or OverlayFlags.doMDT() or OverlayFlags.doRPC() or OverlayFlags.doTGC():
   job += CopyObjects("CopyMuons")
   job.CopyMuons.MuonObjects = True

# convert Digit to RDO for both signal store and temporary background store

include ( "EventOverlayJobTransforms/MuonMcSignal_jobOptions.py" )

include ( "EventOverlayJobTransforms/LArMcSignal_jobOptions.py" )

include ( "EventOverlayJobTransforms/TileMcSignal_jobOptions.py" )

include ( "EventOverlayJobTransforms/L1Signal_jobOptions.py" ) 

from OverlayCommonAlgs.OverlayCommonAlgsConf import UpdateEventInfo
job += UpdateEventInfo()
job.UpdateEventInfo.InfoType="McEventInfo"

# The output - signal
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
signalStream = AthenaPoolOutputStream( "StreamRDO_MC" )
signalStream.OutputFile  = SignalCollection
signalStream.Store = "TemporaryStore"

signalStream.ItemList += [ "EventInfo#*", "EventStreamInfo#*", "PileUpEventInfo#*" ]

if OverlayFlags.doTruth():
   signalStream.ItemList += [
                             "McEventCollection#*",
                             "TrackRecordCollection#*"
                           ]
   if OverlayFlags.doCSC():
      signalStream.ItemList += [ "CscSimDataCollection#CSC_SDO" ]
   if OverlayFlags.doMDT():
      signalStream.ItemList += [ "MuonSimDataCollection#MDT_SDO" ]
   if OverlayFlags.doRPC():
      signalStream.ItemList += [ "MuonSimDataCollection#RPC_SDO" ]
   if OverlayFlags.doTGC():
      signalStream.ItemList += [ "MuonSimDataCollection#TGC_SDO" ]
   if OverlayFlags.doLAr() or OverlayFlags.doTile():
      signalStream.ItemList += [ "CaloCalibrationHitContainer#*" ]
   if OverlayFlags.doPixel():
      signalStream.ItemList += [ "InDetSimDataCollection#PixelSDO_Map" ]
   if OverlayFlags.doSCT():
      signalStream.ItemList += [ "InDetSimDataCollection#SCT_SDO_Map" ]
   if OverlayFlags.doTRT():
      signalStream.ItemList += [ "InDetSimDataCollection#TRT_SDO_Map" ]
   if OverlayFlags.doBCM():
      signalStream.ItemList += [ "InDetSimDataCollection#BCM_SDO_Map" ] 

if OverlayFlags.doPixel():
   signalStream.ItemList += ["PixelRDO_Container#*"]
if OverlayFlags.doSCT():
   signalStream.ItemList += ["SCT_RDO_Container#*"]
if OverlayFlags.doTRT():
   signalStream.ItemList += ["TRT_RDO_Container#*"]

if OverlayFlags.doLAr():
    signalStream.ItemList+=["LArRawChannelContainer#*"]
    signalStream.ItemList+=["LArDigitContainer#LArDigitContainer_MC_Thinned"]
if OverlayFlags.doTile():
    signalStream.ItemList += [ "TileDigitsContainer#TileDigitsFlt" ]
    signalStream.ItemList += ["TileL2Container#TileL2Cnt"]
    signalStream.ItemList += [ "TileRawChannelContainer#*" ]

if OverlayFlags.doCSC():
   signalStream.ItemList += [ "CscRawDataContainer#*" ]
if OverlayFlags.doMDT():
   signalStream.ItemList += [ "MdtCsmContainer#*" ]
if OverlayFlags.doRPC():
   signalStream.ItemList += [ "RpcPadContainer#*" ]
if OverlayFlags.doTGC():
   signalStream.ItemList += [ "TgcRdoContainer#*" ]

if OverlayFlags.doBCM():
   signalStream.ItemList+=["BCM_RDO_Container#*"]

if OverlayFlags.doLVL1():
   signalStream.ItemList+=["LArTTL1Container#*"]
   signalStream.ItemList+=["TileTTL1Container#*"]
   signalStream.ItemList+=[
                           "ROIB::RoIBResult#*",
                           "MuCTPI_RDO#*",
                           "CTP_RDO#*",
                           "DataVector<LVL1::TriggerTower>#*",
                           "DataVector<LVL1::CPMTower>#*",
                           "DataVector<LVL1::JetElement>#*",
                           "DataVector<LVL1::CPMHits>#*",
                           "DataVector<LVL1::CPMRoI>#*",
                           "DataVector<LVL1::CMMCPHits>#*",
                           "DataVector<LVL1::CMMJetHits>#*",
                           "DataVector<LVL1::CMMEtSums>#*",
                           "DataVector<LVL1::JEMHits>#*",
                           "DataVector<LVL1::JEMRoI>#*",
                           "DataVector<LVL1::JEMEtSums>#*",
                           "LVL1::CMMRoI#*"]

#options for writing the MetaData into the output file
MetaDataStore=ServiceMgr.MetaDataStore
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamRDO_MC_FH =  AthenaPoolOutputStream( "StreamRDO_MC_FH" )
from AthenaCommon.AthenaCommonFlags import jobproperties
StreamRDO_MC_FH.OutputFile = "ROOTTREE:"+SignalCollection
StreamRDO_MC_FH.Store = MetaDataStore
StreamRDO_MC_FH.WriteOnExecute = False
StreamRDO_MC_FH.WriteOnFinalize = True
# Write all IOV meta data containers
StreamRDO_MC_FH.ItemList += [ "IOVMetaDataContainer#*" ]
StreamRDO_MC_FH.ItemList += [ "LumiBlockCollection#*" ]

from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
StreamRDO_MC_FHTool = AthenaPoolOutputStreamTool("StreamRDO_MC_FHTool")
StreamRDO_MC_FHTool.OutputCollection = "MetaDataHdr";
StreamRDO_MC_FHTool.PoolContainerPrefix = "MetaData"
StreamRDO_MC_FH.WritingTool = StreamRDO_MC_FHTool

