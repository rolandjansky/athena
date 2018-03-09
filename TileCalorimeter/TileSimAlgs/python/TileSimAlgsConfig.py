# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the TileHitVecToCntTool.
def Tile_FirstXing():
    return -200

# The latest bunch crossing time for which interactions will be sent
# to the TileHitVecToCntTool.
def Tile_LastXing():
    return 150

def getTileRange(name="TileRange", **kwargs):
    #this is the time of the xing in ns 
    kwargs.setdefault('FirstXing', Tile_FirstXing() )
    kwargs.setdefault('LastXing',  Tile_LastXing() )
    kwargs.setdefault('ItemList', ["TileHitVector#TileHitVec",
                                   "TileHitVector#MBTSHits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def TileHitVecToCntTool(name="TileHitVecToCntTool", **kwargs):
    ## FIXME includes to be replaced by confGetter configuration.
    from AthenaCommon.Resilience import protectedInclude
    protectedInclude( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
    protectedInclude( "TileConditions/TileConditions_jobOptions.py" )
    kwargs.setdefault("TileHitVectors", ["TileHitVec" , "MBTSHits" ])
    kwargs.setdefault("TileInfoName", "TileInfo")
    kwargs.setdefault("TileHitContainer", "TileHitCnt")
    kwargs.setdefault("TileHitContainer_DigiHSTruth", "TileHitCnt_DigiHSTruth")
    kwargs.setdefault("DoHSTruthReconstruction", digitizationFlags.doDigiTruth())
    from AthenaCommon.DetFlags import DetFlags
    kwargs.setdefault("PileUp", DetFlags.pileup.Tile_on())

    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == 'cosmics':
        kwargs.setdefault("TriggerTimeTool", "CosmicTriggerTimeTool")
        kwargs.setdefault("HitTimeFlag", 2)
        kwargs.setdefault("UseTriggerTime", True)

    # Random number engine
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc())
    digitizationFlags.rndmSeedList.addSeed("Tile_HitVecToCnt", 4789899, 989240512)

    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", Tile_FirstXing() ) 
        kwargs.setdefault("LastXing",  Tile_LastXing() ) 

    return CfgMgr.TileHitVecToCntTool(name, **kwargs)
