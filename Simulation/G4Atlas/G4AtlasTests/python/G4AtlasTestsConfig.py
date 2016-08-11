# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,Logging
from AthenaCommon.AppMgr import ToolSvc

def PixelHitsTestTool(name="PixelHitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "PixelHits")
    return CfgMgr.SiHitsTestTool(name, **kwargs)              
def SCT_HitsTestTool(name="SCT_HitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "SCT_Hits")
    return CfgMgr.SiHitsTestTool(name, **kwargs)              
def TrtHitsTestTool(name="TrtHitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "TRTUncompressedHits")
    return CfgMgr.TrtHitsTestTool(name, **kwargs)             
def PileupPixelHitsTestTool(name="PileupPixelHitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "PileupPixelHits")
    return CfgMgr.SiHitsTestTool(name, **kwargs)              
def PileupSCT_HitsTestTool(name="PileupSCT_HitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "PileupSCT_Hits")
    return CfgMgr.SiHitsTestTool(name, **kwargs)              
def PileupTrtHitsTestTool(name="PileupTrtHitsTestTool", **kwargs):
    kwargs.setdefault("CollectionName", "PileupTRTUncompressedHits")
    return CfgMgr.TrtHitsTestTool(name, **kwargs)             
## def LArHitsTestTool(name="LArHitsTestTool", **kwargs):
##     return CfgMgr.LArHitsTestTool(name, **kwargs)             
def EMBHitsTestTool(name="EMB", **kwargs):
    kwargs.setdefault("DetectorName", "EMB")
    #return CfgMgr.EMBHitsTestTool(name, **kwargs)
    return CfgMgr.LArHitsTestTool(name, **kwargs)             
def EMECHitsTestTool(name="EMEC", **kwargs):
    kwargs.setdefault("DetectorName", "EMEC")
    #return CfgMgr.EMECHitsTestTool(name, **kwargs)            
    return CfgMgr.LArHitsTestTool(name, **kwargs)             
def HECHitsTestTool(name="HEC", **kwargs):
    kwargs.setdefault("DetectorName", "HEC")
    #return CfgMgr.HECHitsTestTool(name, **kwargs)
    return CfgMgr.LArHitsTestTool(name, **kwargs)             
def FCALHitsTestTool(name="FCAL", **kwargs):
    kwargs.setdefault("DetectorName", "FCAL")
    #return CfgMgr.FCALHitsTestTool(name, **kwargs)            
    return CfgMgr.LArHitsTestTool(name, **kwargs)             
def TileHitsTestTool(name="TileHitsTestTool", **kwargs):
    return CfgMgr.TileHitsTestTool(name, **kwargs)            
def CSCHitsTestTool(name="CSCHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "CSC")
    return CfgMgr.CSCHitsTestTool(name, **kwargs)
def MDTHitsTestTool(name="MDTHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "MDT")
    return CfgMgr.MDTHitsTestTool(name, **kwargs)
def RPCHitsTestTool(name="RPCHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "RPC")
    return CfgMgr.RPCHitsTestTool(name, **kwargs)
def TGCHitsTestTool(name="TGCHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "TGC")
    return CfgMgr.TGCHitsTestTool(name, **kwargs)
def MMHitsTestTool(name="MMHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "MM")
    return CfgMgr.MMHitsTestTool(name, **kwargs)
def sTGCHitsTestTool(name="sTGCHitsTestTool", **kwargs):
    kwargs.setdefault("DetectorName", "sTGC")
    return CfgMgr.sTGCHitsTestTool(name, **kwargs)    
def ALFA_SimHitsTestTool(name="ALFA_SimHitsTestTool", **kwargs):
    return CfgMgr.ALFA_SimHitsTestTool(name, **kwargs)
def ZDCHitsTestTool(name="ZDCHitsTestTool", **kwargs):
    return CfgMgr.ZDCHitsTestTool(name, **kwargs)
def LArActiveCaloCalibHitsTestTool(name="LArActiveCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","LArActive")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def LArInactiveCaloCalibHitsTestTool(name="LArInactiveCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","LArInactive")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def LArDeadMaterialCaloCalibHitsTestTool(name="LArDeadMaterialCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","LArDeadMaterial")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TileActiveCellCaloCalibHitsTestTool(name="TileActiveCellCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","TileActiveCell")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TileInactiveCellCaloCalibHitsTestTool(name="TileInactiveCellCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","TileInactiveCell")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TileDeadMaterialCaloCalibHitsTestTool(name="TileDeadMaterialCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","TileDeadMaterial")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TileDMCaloCalibHitsTestTool(name="TileDMCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","TileDM")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TileCellCaloCalibHitsTestTool(name="TileCellCaloCalibHitsTestTool", **kwargs):
    kwargs.setdefault("CalibHitType","TileCell")
    return CfgMgr.CaloCalibrationHitsTestTool(name, **kwargs)
def TruthTestTool(name="TruthTestTool", **kwargs):
    kwargs.setdefault("McEventKey", "TruthEvent")
    return CfgMgr.TruthTestTool(name, **kwargs)
def PileupTruthTestTool(name="PileupTruthTestTool", **kwargs):
    kwargs.setdefault("McEventKey", "TruthEvent_PU")
    return CfgMgr.TruthTestTool(name, **kwargs)
def EvgenTruthTestTool(name="EvgenTruthTestTool", **kwargs):
    kwargs.setdefault("McEventKey", "GEN_EVENT")
    return CfgMgr.TruthTestTool(name, **kwargs)
def PileupEvgenTruthTestTool(name="PileupEvgenTruthTestTool", **kwargs):
    kwargs.setdefault("McEventKey", "GEN_EVENT_PU")
    return CfgMgr.TruthTestTool(name, **kwargs)
def CaloEntryLayerTestTool(name="CaloEntry", **kwargs):
    kwargs.setdefault("CollectionName", "CaloEntry")
    return CfgMgr.LayerTestTool(name, **kwargs)
def MuonEntryLayerTestTool(name="MuonEntry", **kwargs):
    kwargs.setdefault("CollectionName", "MuonEntry")
    return CfgMgr.LayerTestTool(name, **kwargs)
def MuonExitLayerTestTool(name="MuonExit", **kwargs):
    kwargs.setdefault("CollectionName", "MuonExit")
    return CfgMgr.LayerTestTool(name, **kwargs)
def LucidHitsTestTool(name="LucidHitsTestTool",**kwargs):
    return CfgMgr.LucidHitsTestTool(name, **kwargs)


def getSteppingValidationTool(name="G4UA::SteppingValidationTool",**kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    from G4AtlasTests.G4AtlasTestsConf import G4UA__SteppingValidationTool
    return G4UA__SteppingValidationTool(name, **kwargs)
