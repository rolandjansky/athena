# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file CaloD3PDMaker/python/CaloHitD3PD.py
# @author ps, from code from sss
# @date Feb, 2013
# @brief Construct a calorimeter hits D3PD.
#


from EventCommonD3PDMaker.EventInfoD3PDObject     import EventInfoD3PDObject
from CaloD3PDMaker.LArHitD3PDObject               import LArHitEMBD3PDObject, LArHitEMECD3PDObject, LArHitHECD3PDObject, LArHitFCALD3PDObject
from CaloD3PDMaker.TileHitD3PDObject              import TileHitD3PDObject, makeTileHitD3PDObject
from CaloD3PDMaker.CaloCalibHitD3PDObject         import LArActiveCaloCalibHitD3PDObject, LArInactiveCaloCalibHitD3PDObject, LArDMCaloCalibHitD3PDObject, TileActiveCaloCalibHitD3PDObject, TileInactiveCaloCalibHitD3PDObject, TileDMCaloCalibHitD3PDObject, TileDMOCaloCalibHitD3PDObject
from CaloD3PDMaker                                import TileHitFillerTool
from D3PDMakerCoreComps.D3PDObject                import D3PDObject
from D3PDMakerCoreComps.resolveSGKey              import testSGKey

def CaloHitD3PD (alg = None,
              file = 'calo.root',
              tuplename = 'calohit'):
    # Core algorithm
    # By default the user of this function should have created an algorithm
    # already.  But for backwards compatibility let the function create its own
    # algorithm if needed...
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
    

        alg += EventInfoD3PDObject (10)

        calohit_sgkey_lar = 'LArHitEMB'
        if testSGKey ('LArHitContainer', calohit_sgkey_lar):
           alg +=  LArHitEMBD3PDObject(10)

        calohit_sgkey_lar = 'LArHitEMEC'
        if testSGKey ('LArHitContainer', calohit_sgkey_lar):
           alg +=  LArHitEMECD3PDObject(10)

        calohit_sgkey_lar = 'LArHitHEC'
        if testSGKey ('LArHitContainer', calohit_sgkey_lar):
           alg +=  LArHitHECD3PDObject(10)

        calohit_sgkey_lar = 'LArHitFCAL'
        if testSGKey ('LArHitContainer', calohit_sgkey_lar):
           alg +=  LArHitFCALD3PDObject(10)

        calohit_sgkey_tile = 'TileHitVec'
        if testSGKey ('TileHitVector', calohit_sgkey_tile):
           alg +=  TileHitD3PDObject(10)

        calohit_sgkey_tile = 'MBTSHits'
        if testSGKey ('TileHitVector', calohit_sgkey_tile):
           MBTSHitD3PDObject = D3PDObject (makeTileHitD3PDObject, 'MBTSHit_', 'MBTSHitD3PDObject')
           MBTSHitD3PDObject.defineBlock (0, 'MBTSHitDetails', TileHitFillerTool)
           alg +=  MBTSHitD3PDObject(10, sgkey=calohit_sgkey_tile)

        calohit_sgkey_lar = 'LArCalibrationHitActive'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_lar):
            alg +=  LArActiveCaloCalibHitD3PDObject(10)

        calohit_sgkey_lar = 'LArCalibrationHitInactive'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_lar):
            alg +=  LArInactiveCaloCalibHitD3PDObject(10)

        calohit_sgkey_tile = 'TileCalibrationCellHitCnt'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_tile):
            alg += TileCaloCalibHitD3PDObject(10)

        calohit_sgkey_tile = 'TileCalibHitActiveCell'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_tile):
            alg += TileActiveCaloCalibHitD3PDObject(10)

        calohit_sgkey_tile = 'TileCalibHitInactiveCell'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_tile):
            alg += TileInactiveCaloCalibHitD3PDObject(10)

        calohit_sgkey_dmlar = 'LArCalibrationHitDeadMaterial'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_dmlar):
            alg += LArDMCaloCalibHitD3PDObject(10)

        calohit_sgkey_dmtile = 'TileCalibrationDMHitCnt'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_dmtile):
            alg += TileDMOCaloCalibHitD3PDObject(10)

        calohit_sgkey_dmtile = 'TileCalibHitDeadMaterial'
        if testSGKey ('CaloCalibrationHitContainer', calohit_sgkey_dmtile):
            alg += TileDMCaloCalibHitD3PDObject(10)

        return alg
    
