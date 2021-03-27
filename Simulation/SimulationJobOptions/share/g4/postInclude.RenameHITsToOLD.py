from AthenaCommon import CfgMgr
if not hasattr(svcMgr,'AddressRemappingSvc'):
    svcMgr += CfgMgr.AddressRemappingSvc()
if not hasattr(svcMgr,'ProxyProviderSvc'):
    svcMgr += CfgMgr.ProxyProviderSvc()
if 'AddressRemappingSvc' not in svcMgr.ProxyProviderSvc.ProviderNames:
    svcMgr.ProxyProviderSvc.ProviderNames += ['AddressRemappingSvc']
svcMgr.AddressRemappingSvc.OutputLevel=VERBOSE

svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps=[ "McEventCollection#TruthEvent->McEventCollection#TruthEventOLD",
                                                  "RecoTimingObj#EVNTtoHITS_timings->RecoTimingObj#EVNTtoHITS_timingsOLD"]
StreamHITS.ItemList += ['EventInfo#*', # Just copied not renamed
                        'McEventCollection#TruthEventOLD',
                        'xAOD::JetContainer#*', # Just copied not renamed
                        'xAOD::JetAuxContainer#*', # Just copied not renamed
                        'RecoTimingObj#EVNTtoHITS_timingsOLD']

from AthenaCommon.DetFlags import DetFlags
## Inner Detector
if DetFlags.ID_on():
    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[
        "SiHitCollection#BCMHits->SiHitCollection#BCMHitsOLD",
        "SiHitCollection#BLMHits->SiHitCollection#BLMHitsOLD",
        "SiHitCollection#PixelHits->SiHitCollection#PixelHitsOLD",
        "SiHitCollection#SCT_Hits->SiHitCollection#SCT_HitsOLD",
        "TRTUncompressedHitCollection#TRTUncompressedHits->TRTUncompressedHitCollection#TRTUncompressedHitsOLD",
        "TrackRecordCollection#CaloEntryLayer->TrackRecordCollection#CaloEntryLayerOLD"]
    StreamHITS.ItemList += ['SiHitCollection#*OLD',
                            'TRTUncompressedHitCollection#TRTUncompressedHitsOLD',
                            'TrackRecordCollection#CaloEntryLayerOLD']
## Calo
if DetFlags.Calo_on():
    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[
        "LArHitContainer#LArHitEMB->LArHitContainer#LArHitEMBOLD",
        "LArHitContainer#LArHitEMEC->LArHitContainer#LArHitEMECOLD",
        "LArHitContainer#LArHitFCAL->LArHitContainer#LArHitFCALOLD",
        "LArHitContainer#LArHitHEC->LArHitContainer#LArHitHECOLD",
        "LArHitContainer#LArHitMiniFCAL->LArHitContainer#LArHitMiniFCALOLD",
        "TileHitVector#MBTSHits->TileHitVector#MBTSHitsOLD",
        "TileHitVector#TileHitVec->TileHitVector#TileHitVecOLD",
        "CaloCalibrationHitContainer#LArCalibrationHitActive->CaloCalibrationHitContainer#LArCalibrationHitActiveOLD" ,
        "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial->CaloCalibrationHitContainer#LArCalibrationHitDeadMaterialOLD",
        "CaloCalibrationHitContainer#LArCalibrationHitInactive->CaloCalibrationHitContainer#LArCalibrationHitInactiveOLD",
        "CaloCalibrationHitContainer#TileCalibHitActiveCell->CaloCalibrationHitContainer#TileCalibHitActiveCellOLD",
        "CaloCalibrationHitContainer#TileCalibHitInactiveCell->CaloCalibrationHitContainer#TileCalibHitInactiveCellOLD",
        "CaloCalibrationHitContainer#TileCalibHitDeadMaterial->CaloCalibrationHitContainer#TileCalibHitDeadMaterialOLD",
        "TrackRecordCollection#MuonEntryLayer->TrackRecordCollection#MuonEntryLayerOLD"]
    StreamHITS.ItemList += ['LArHitContainer#*OLD',
                            'TileHitVector#*OLD',
                            'CaloCalibrationHitContainer#*OLD',
                            'TrackRecordCollection#MuonEntryLayerOLD']

## Muon
if DetFlags.Muon_on():
     svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[
        "CSCSimHitCollection#CSC_Hits->CSCSimHitCollection#CSC_HitsOLD",
        "MDTSimHitCollection#MDT_Hits->MDTSimHitCollection#MDT_HitsOLD",
        "RPCSimHitCollection#RPC_Hits->RPCSimHitCollection#RPC_HitsOLD",
        "TGCSimHitCollection#TGC_Hits->TGCSimHitCollection#TGC_HitsOLD",
        "TrackRecordCollection#MuonExitLayer->TrackRecordCollection#MuonExitLayerOLD"]
     StreamHITS.ItemList += ['CSCSimHitCollection#CSC_HitsOLD',
                             'MDTSimHitCollection#MDT_HitsOLD',
                             'RPCSimHitCollection#RPC_HitsOLD',
                             'TGCSimHitCollection#TGC_HitsOLD',
                             'TrackRecordCollection#MuonExitLayerOLD']

#FIXME Add Renaming for Fwd Detector sim hits
## FwdRegion
if DetFlags.FwdRegion_on():
    #svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[]
    StreamHITS.ItemList += ["SimulationHitCollection#*"]

## Lucid
if DetFlags.Lucid_on():
    #svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[]
    StreamHITS.ItemList += ["LUCID_SimHitCollection#*"]

## ZDC
if DetFlags.ZDC_on():
    #svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[]
    StreamHITS.ItemList += ["ZDC_SimPixelHit_Collection#*",
                             "ZDC_SimStripHit_Collection#*"]
## ALFA
if DetFlags.ALFA_on():
    #svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[]
    StreamHITS.ItemList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*"]

## AFP
if DetFlags.AFP_on():
    #svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps+=[]
    StreamHITS.ItemList += ["AFP_TDSimHitCollection#*","AFP_SIDSimHitCollection#*"]

StreamHITS.TakeItemsFromInput=False;


print(StreamHITS)
