include.block ( "EventOverlayJobTransforms/OverlayOutputItemList_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.DetFlags import DetFlags

# The output - overlay
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream( "StreamRDO",OverlayCollection )
outStream.Store = ServiceMgr.StoreGateSvc
print "ACH123 new OverlayOutputItemList_jobOptions.py"

# overlay output stream
outStream.ItemList += [ "EventInfo#*", "PileUpEventInfo#*" ]
outStream.ItemList += [ "xAOD::EventInfo#*", "xAOD::EventAuxInfo#*" ]
outStream.ItemList += [ "LumiBlockCollection#*" ]

if DetFlags.overlay.Truth_on():
   outStream.ItemList += [
                             "McEventCollection#*",
                             "TrackRecordCollection#*"
                           ]
   if DetFlags.overlay.CSC_on():
      outStream.ItemList += [ "CscSimDataCollection#CSC_SDO" ]
   if DetFlags.overlay.MDT_on():
      outStream.ItemList += [ "MuonSimDataCollection#MDT_SDO" ]
   if DetFlags.overlay.RPC_on():
      outStream.ItemList += [ "MuonSimDataCollection#RPC_SDO" ]
   if DetFlags.overlay.TGC_on():
      outStream.ItemList += [ "MuonSimDataCollection#TGC_SDO" ]
   if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
      outStream.ItemList += [ "CaloCalibrationHitContainer#*" ]
   if DetFlags.overlay.pixel_on():
      outStream.ItemList += [ "InDetSimDataCollection#PixelSDO_Map" ]
   if DetFlags.overlay.SCT_on():
      outStream.ItemList += [ "InDetSimDataCollection#SCT_SDO_Map" ]
   if DetFlags.overlay.TRT_on():
      outStream.ItemList += [ "InDetSimDataCollection#TRT_SDO_Map" ]
   if DetFlags.overlay.BCM_on():
      outStream.ItemList += [ "InDetSimDataCollection#BCM_SDO_Map" ] 

if DetFlags.overlay.pixel_on():
   outStream.ItemList += ["PixelRDO_Container#*"]
   outStream.ItemList += ["InDetBSErrContainer#*"]
if DetFlags.overlay.SCT_on():
   outStream.ItemList += ["SCT_RDO_Container#*"]
   outStream.ItemList += ["SCT_RDO_Container#*"]
if DetFlags.overlay.TRT_on():
   outStream.ItemList += ["TRT_RDO_Container#*"]
   outStream.ItemList += ["TRT_BSIdErrContainer#*"]
   outStream.ItemList += ["TRT_BSErrContainer#*"]

if DetFlags.overlay.LAr_on():
   outStream.ItemList+=["LArRawChannelContainer#*"]
   outStream.ItemList+=["LArDigitContainer#LArDigitContainer_MC_Thinned"]
   outStream.ItemList+=["LArFebErrorSummary#*"]
if DetFlags.overlay.Tile_on():
   if isRealData:
      outStream.ItemList += [ "TileDigitsContainer#*" ]
   else:
      outStream.ItemList += [ "TileDigitsContainer#TileDigitsFlt" ]
   outStream.ItemList += [ "TileRawChannelContainer#*" ]

if DetFlags.overlay.CSC_on():
   outStream.ItemList += [ "CscRawDataContainer#*" ]
   outStream.ItemList += [ "CscDigitContainer#*" ]
if DetFlags.overlay.MDT_on():
   outStream.ItemList += [ "MdtCsmContainer#*" ]
#   outStream.ItemList += [ "MdtDigitContainer#*" ]
if DetFlags.overlay.RPC_on():
   outStream.ItemList += [ "RpcPadContainer#*" ]
#   outStream.ItemList += [ "RpcDigitContainer#*" ]
if DetFlags.overlay.TGC_on():
   outStream.ItemList += [ "TgcRdoContainer#*" ]
#   outStream.ItemList += [ "TgcDigitContainer#*" ]

if DetFlags.overlay.BCM_on():
   outStream.ItemList+=["BCM_RDO_Container#*"]

if DetFlags.overlay.LVL1_on():
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
