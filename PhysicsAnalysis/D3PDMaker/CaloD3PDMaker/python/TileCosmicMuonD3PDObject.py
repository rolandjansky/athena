# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from CaloD3PDMaker.makeTileCellFilterAlg import makeTileCellFilterAlg
from CaloIdentifier import SUBCALO
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation

def makeTileCosmicMuonD3PDObject (name, prefix, object_name='TileCosmicMuonD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'TileCosmicMuonHT'
    if label == None: label = prefix

    
    print " makeTileCosmicMuonD3PDObject: name = ", name
    print " makeTileCosmicMuonD3PDObject: prefix = ", prefix
    print " makeTileCosmicMuonD3PDObject: object_name = ", object_name
    print " makeTileCosmicMuonD3PDObject: sgkey = ", sgkey

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileCosmicMuonContainer',
                  SGKey = sgkey,
                  Label = label)
        

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



TileCosmicMuonD3PDObject=D3PDObject(makeTileCosmicMuonD3PDObject,'TileTracks_','TileCosmicMuonD3PDObject')
    
TileCosmicMuonD3PDObject.defineBlock (0, 'TileCosmicMuons',
                                      CaloD3PDMaker.TileCosmicMuonFillerTool)

TileCosmicMuonTileCells=ContainedVectorMultiAssociation(TileCosmicMuonD3PDObject,
                                                        CaloD3PDMaker.TileCosmicMuonTileCellAssociationTool,
                                                        "cell",1)

TileCosmicMuonTileCells.defineBlock (1, 'TileDetail0',
                                CaloD3PDMaker.TileCellDetailsFillerTool,
                                SaveCellDetails=True,
                                SavePositionInfo=False,
                                )
    
TileCosmicMuonTileCells.defineBlock (2, 'TileDetail1',
                                CaloD3PDMaker.TileCellDetailsFillerTool,
                                SaveCellDetails=False,
                                SavePositionInfo=True,
                                )





