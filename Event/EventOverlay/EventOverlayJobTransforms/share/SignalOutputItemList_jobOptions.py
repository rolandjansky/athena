include.block ( "EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.DetFlags import DetFlags

#copy stuff back into the MC before persistency
from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyObjects

if DetFlags.overlay.Truth_on():
   job += CopyObjects("CopyTruth")
   job.CopyTruth.TruthObjects = True

# copy InDet objects back into Signal event store
if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():
   job += CopyObjects("CopyInDet")
   job.CopyInDet.InDetObjects = True

if DetFlags.overlay.CSC_on() or DetFlags.overlay.MDT_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on():
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

if DetFlags.overlay.Truth_on():
   signalStream.ItemList += [
                             "McEventCollection#*",
                             "TrackRecordCollection#*"
                           ]
   if DetFlags.overlay.CSC_on():
      signalStream.ItemList += [ "CscSimDataCollection#CSC_SDO" ]
   if DetFlags.overlay.MDT_on():
      signalStream.ItemList += [ "MuonSimDataCollection#MDT_SDO" ]
   if DetFlags.overlay.RPC_on():
      signalStream.ItemList += [ "MuonSimDataCollection#RPC_SDO" ]
   if DetFlags.overlay.TGC_on():
      signalStream.ItemList += [ "MuonSimDataCollection#TGC_SDO" ]
   if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
      signalStream.ItemList += [ "CaloCalibrationHitContainer#*" ]
   if DetFlags.overlay.pixel_on():
      signalStream.ItemList += [ "InDetSimDataCollection#PixelSDO_Map" ]
   if DetFlags.overlay.SCT_on():
      signalStream.ItemList += [ "InDetSimDataCollection#SCT_SDO_Map" ]
   if DetFlags.overlay.TRT_on():
      signalStream.ItemList += [ "InDetSimDataCollection#TRT_SDO_Map" ]
   if DetFlags.overlay.BCM_on():
      signalStream.ItemList += [ "InDetSimDataCollection#BCM_SDO_Map" ] 

if DetFlags.overlay.pixel_on():
   signalStream.ItemList += ["PixelRDO_Container#*"]
if DetFlags.overlay.SCT_on():
   signalStream.ItemList += ["SCT_RDO_Container#*"]
if DetFlags.overlay.TRT_on():
   signalStream.ItemList += ["TRT_RDO_Container#*"]

if DetFlags.overlay.LAr_on():
    signalStream.ItemList+=["LArRawChannelContainer#*"]
    signalStream.ItemList+=["LArDigitContainer#LArDigitContainer_MC_Thinned"]
if DetFlags.overlay.Tile_on():
    signalStream.ItemList += [ "TileDigitsContainer#TileDigitsFlt" ]
    signalStream.ItemList += ["TileL2Container#TileL2Cnt"]
    signalStream.ItemList += [ "TileRawChannelContainer#*" ]

if DetFlags.overlay.CSC_on():
   signalStream.ItemList += [ "CscRawDataContainer#*" ]
if DetFlags.overlay.MDT_on():
   signalStream.ItemList += [ "MdtCsmContainer#*" ]
if DetFlags.overlay.RPC_on():
   signalStream.ItemList += [ "RpcPadContainer#*" ]
if DetFlags.overlay.TGC_on():
   signalStream.ItemList += [ "TgcRdoContainer#*" ]

if DetFlags.overlay.BCM_on():
   signalStream.ItemList+=["BCM_RDO_Container#*"]

if DetFlags.overlay.LVL1_on():
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

