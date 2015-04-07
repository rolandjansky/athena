include.block ( "EventOverlayJobTransforms/OverlayOutputItemList_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from OverlayCommonAlgs.OverlayFlags import OverlayFlags

# The output - overlay
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream( "StreamRDO",OverlayCollection )
outStream.Store = ServiceMgr.StoreGateSvc
print "ACH123 new OverlayOutputItemList_jobOptions.py"

# overlay output stream
outStream.ItemList += [ "EventInfo#*", "PileUpEventInfo#*" ]
outStream.ItemList += [ "LumiBlockCollection#*" ]

if OverlayFlags.doTruth():
   outStream.ItemList += [
                             "McEventCollection#*",
                             "TrackRecordCollection#*"
                           ]
   if OverlayFlags.doCSC():
      outStream.ItemList += [ "CscSimDataCollection#CSC_SDO" ]
   if OverlayFlags.doMDT():
      outStream.ItemList += [ "MuonSimDataCollection#MDT_SDO" ]
   if OverlayFlags.doRPC():
      outStream.ItemList += [ "MuonSimDataCollection#RPC_SDO" ]
   if OverlayFlags.doTGC():
      outStream.ItemList += [ "MuonSimDataCollection#TGC_SDO" ]
   if OverlayFlags.doLAr() or OverlayFlags.doTile():
      outStream.ItemList += [ "CaloCalibrationHitContainer#*" ]
   if OverlayFlags.doPixel():
      outStream.ItemList += [ "InDetSimDataCollection#PixelSDO_Map" ]
   if OverlayFlags.doSCT():
      outStream.ItemList += [ "InDetSimDataCollection#SCT_SDO_Map" ]
   if OverlayFlags.doTRT():
      outStream.ItemList += [ "InDetSimDataCollection#TRT_SDO_Map" ]
   if OverlayFlags.doBCM():
      outStream.ItemList += [ "InDetSimDataCollection#BCM_SDO_Map" ] 

if OverlayFlags.doPixel():
   outStream.ItemList += ["PixelRDO_Container#*"]
if OverlayFlags.doSCT():
   outStream.ItemList += ["SCT_RDO_Container#*"]
if OverlayFlags.doTRT():
   outStream.ItemList += ["TRT_RDO_Container#*"]

if OverlayFlags.doLAr():
   outStream.ItemList+=["LArRawChannelContainer#*"]
   outStream.ItemList+=["LArDigitContainer#LArDigitContainer_MC_Thinned"]
if OverlayFlags.doTile():
   if isRealData:
      outStream.ItemList += [ "TileDigitsContainer#*" ]
   else:
      outStream.ItemList += [ "TileDigitsContainer#TileDigitsFlt" ]
   outStream.ItemList += [ "TileRawChannelContainer#*" ]

if OverlayFlags.doCSC():
   outStream.ItemList += [ "CscRawDataContainer#*" ]
   outStream.ItemList += [ "CscDigitContainer#*" ]
if OverlayFlags.doMDT():
   outStream.ItemList += [ "MdtCsmContainer#*" ]
#   outStream.ItemList += [ "MdtDigitContainer#*" ]
if OverlayFlags.doRPC():
   outStream.ItemList += [ "RpcPadContainer#*" ]
#   outStream.ItemList += [ "RpcDigitContainer#*" ]
if OverlayFlags.doTGC():
   outStream.ItemList += [ "TgcRdoContainer#*" ]
#   outStream.ItemList += [ "TgcDigitContainer#*" ]

if OverlayFlags.doBCM():
   outStream.ItemList+=["BCM_RDO_Container#*"]

if OverlayFlags.doLVL1():
   outStream.ItemList+=["LArTTL1Container#*"]
   outStream.ItemList+=["TileTTL1Container#*"]
   outStream.ItemList+=[
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


