# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from CaloIdentifier import SUBCALO
from CaloD3PDMaker.makeCaloCellFilterAlg import makeCaloCellFilterAlg 

prefix_to_det = {}
prefix_to_det["cc_sel_"] = [SUBCALO.LAREM,SUBCALO.LARHEC,SUBCALO.LARFCAL,SUBCALO.TILE] 
prefix_to_det["cc_em_"] = [SUBCALO.LAREM]
prefix_to_det["cc_hec_"] = [SUBCALO.LARHEC]
prefix_to_det["cc_fcal_"] = [SUBCALO.LARFCAL] 
prefix_to_det["cc_tile_"] = [SUBCALO.TILE]

def makeCellD3PDObject (name, prefix, object_name, getter = None,
                           sgkey = None,
                           label = None):
    if sgkey is None: sgkey = 'AllCalo'
    if label is None: label = prefix

    if prefix in prefix_to_det.keys():
        sgkey = prefix 
    #print " makeCellD3PDObject: sgkey = ", sgkey 

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'CaloCellContainer',
                  SGKey = sgkey,
                  Label = label)
        
    # create the selected cells
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())


# function to create the CaloCellContainer for selected 
def hookForCaloCellFilterAlg(c, prefix, *args, **kw) :


    cellSigmaCut=-1. 

    if prefix in prefix_to_det.keys():
        sgkey = prefix
        subCalo = prefix_to_det[prefix]

        if sgkey == "cc_sel_":
            cellSigmaCut=4 

        #print " in makeCellD3PDObject, subCalo, sgkey, cellSigmaCut = ", subCalo, sgkey, cellSigmaCut

        filter = makeCaloCellFilterAlg( CaloNums=subCalo, OutputCellsName=sgkey, CellSigmaCut=cellSigmaCut  )

        from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
        from AthenaCommon.AlgSequence import AlgSequence
        preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
        preseq +=filter 
        
    return 

def makeCaloCellD3PDObject (maker, prefix, object_name) :

    cellD3PDObject = D3PDObject (makeCellD3PDObject, prefix, object_name)

    if prefix in prefix_to_det.keys() :
        #print " in makeCaloCellD3PDMaker, prefix ="  , prefix 
        cellD3PDObject.defineHook  ( hookForCaloCellFilterAlg )

    cellD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,  WriteM = False)


    cellD3PDObject.defineBlock (1, 'Detail1',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=True,
                                SaveTimeInfo=True,
                                SaveDetInfo=True,
                                SaveCellGain=True,
                                SaveBadCellStatus=False,
                                SaveId =False,
                                SavePositionInfo=False,
                                )


    cellD3PDObject.defineBlock (2, 'Detail2',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=False,
                                SaveTimeInfo=False,
                                SaveDetInfo=False,
                                SaveCellGain=False,
                                SaveBadCellStatus=True,
                                SaveId =True,
                                SavePositionInfo=True,
                                )

    noiseType = "totalNoise"
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg(noisetype=noiseType)

    cellD3PDObject.defineBlock (3, 'Detail3',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=False,
                                SaveTimeInfo=False,
                                SaveDetInfo=False,
                                SaveCellGain=False,
                                SaveBadCellStatus=False,
                                SaveId =False,
                                SavePositionInfo=False,
                                SaveSigma = True,
                                CaloNoise = noiseType
                                )

    # Raw eta/phi --- off by default.
    cellD3PDObject.defineBlock (99, 'RawPosition',
                                CaloD3PDMaker.CaloCellRawFillerTool)

    return cellD3PDObject 

"""
level of details: 0: only kinematics
                  1: time, quality, gain, cell_id
                  2: xyz position, bad channel status, offline id. 
                  3: cell noise 
""" 

# all CaloCells
AllCaloCellD3PDObject = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_',      'AllCaloCellD3PDObject')
# Selected Cells with 4 sigma noise cut 
SelCaloCellD3PDObject = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_sel_',  'SelCaloCellD3PDObject')
# All EM cells
EMCellD3PDObject      = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_em_',   'EMCellD3PDObject')
# All HEC cells
HECCellD3PDObject     = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_hec_',  'HECCellD3PDObject')
# All FCAL cells
FCALCellD3PDObject    = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_fcal_', 'FCALCellD3PDObject')
# All Tile cels
TileCellD3PDObject    = makeCaloCellD3PDObject(makeCellD3PDObject, 'cc_tile_', 'TileCellD3PDObject')


#   slimmed version of the CaloCellObject
def makeCaloCellSlimmedD3PDObject (maker, prefix, object_name) :

    cellD3PDObject = D3PDObject (makeCellD3PDObject, prefix, object_name)

    if prefix in prefix_to_det.keys() :
        #print " in makeCaloCellD3PDMaker, prefix ="  , prefix 
        cellD3PDObject.defineHook  ( hookForCaloCellFilterAlg )

    cellD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,  WriteM = False, WritePt = False)


    noiseType = "totalNoise"
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg(noisetype=noiseType)

    cellD3PDObject.defineBlock (1, 'Detail1',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=True,
                                SaveBadCellStatus=True,
                                SaveSigma = True,
                                SaveId = True,
                                SaveDetInfo=True,
                                CaloNoise = noiseType
                               )

    return cellD3PDObject

SelCaloCellSlimmedD3PDObject = makeCaloCellSlimmedD3PDObject(makeCellD3PDObject, 'cc_sel_', 'SelCaloCellSlimmedD3PDObject')
