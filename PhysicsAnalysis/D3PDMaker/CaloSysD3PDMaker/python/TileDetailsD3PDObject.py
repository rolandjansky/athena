# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: TileDetailsD3PDObject.py 522639 2012-10-22 09:41:42Z ssnyder $

import CaloSysD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from CaloD3PDMaker.makeTileCellFilterAlg import makeTileCellFilterAlg
from CaloIdentifier import SUBCALO
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation

BaseSGKey='holder'

def makeTileD3PDObject (name, prefix, object_name='TileDetailsD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    global BaseSGKey
    BaseSGKey=sgkey
    sgkey = prefix
    if label == None: label = prefix

    
    print(" makeTileD3PDObject: name = ", name)
    print(" makeTileD3PDObject: prefix = ", prefix)
    print(" makeTileD3PDObject: object_name = ", object_name)
    print(" makeTileD3PDObject: sgkey = ", sgkey)

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
def hookForTileCellFilterAlg(c, prefix, *args, **kw) :

    cellSigmaCut=-1.
    global BaseSGKey
    if BaseSGKey == None or BaseSGKey=='holder' : BaseSGKey='AllCalo'
    sgkey = prefix

    print(" in makeTileD3PDObject, sgkey, cellSigmaCut = ",  sgkey, cellSigmaCut)

    filter = makeTileCellFilterAlg(OutputCellsName=sgkey, 
                                   CellSigmaCut=cellSigmaCut, 
                                   CellsName = BaseSGKey)

    from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
    from AthenaCommon.AlgSequence import AlgSequence
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    preseq +=filter

    return

def TileCellRawAssoc(parent,prefix='',target='',level=0,blockname=None,*args,**kw):

    if blockname==None:
        blockname=prefix+'TileCellRawAssociation'

    return IndexMultiAssociation(parent,
                                 CaloSysD3PDMaker.TileCellRawAssociationTool,
                                 target,
                                 prefix,
                                 level,
                                 blockname,
                                 nrowName='')

def TileCellDigitAssoc(parent,prefix='',target='',level=0,blockname=None,*args,**kw):
    if blockname==None:
        blockname=prefix+'TileCellDigitAssociation'

    return IndexMultiAssociation(parent,
                                 CaloSysD3PDMaker.TileCellDigitAssociationTool,
                                 target,
                                 prefix,
                                 level,
                                 blockname,
                                 nrowName='')


def makeTileCellD3PDObject (maker, prefix,object_name) :
    
    cellD3PDObject = D3PDObject (maker, prefix, object_name)
    cellD3PDObject.defineHook(hookForTileCellFilterAlg)

    cellD3PDObject.defineBlock (0, 'Kinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE=True,
                                WritePt=True,
                                WriteEtaPhi=True,
                                WriteM=False,
                                )
    
    cellD3PDObject.defineBlock (1, 'TileDetails',
                                CaloSysD3PDMaker.TileCellDetailsFillerTool,
                                SaveCellDetails=True,
                                SavePositionInfo=False,
                                )

    TileRawInCell=TileCellRawAssoc(parent=cellD3PDObject,
                                   prefix='rawCh_',
                                   target='tileraw_',
                                   level=2,
                                   blockname='RawChAssoc')

    TileDigitInCell=TileCellDigitAssoc(parent=cellD3PDObject,
                                   prefix='digit_',
                                   target='tiledigit_',
                                   level=3,
                                   blockname='DigitAssoc')


    return cellD3PDObject 



# All Tile cells
TileDetailsD3PDObject    = makeTileCellD3PDObject(makeTileD3PDObject,'tilecell_',
                                                  'TileDetailsD3PDObject')
