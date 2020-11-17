# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

TileHitSGKey='TileHitVec'

def makeTileHitD3PDObject (name, prefix, object_name='TileHitD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey is None: sgkey = TileHitSGKey
    if label is None: label = prefix

    if not getter:
        getter = CaloSysD3PDMaker.SGTileHitGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileHitVector',
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




TileHitD3PDObject = D3PDObject (makeTileHitD3PDObject, 'TileHit_', 'TileHitD3PDObject')

TileHitD3PDObject.defineBlock (0, 'TileHitDetails',
                                CaloSysD3PDMaker.TileHitFillerTool)
MBTSHitD3PDObject = D3PDObject (makeTileHitD3PDObject, 'MBTSHit_', 'MBTSHitD3PDObject', None, 'MBTSHits')
MBTSHitD3PDObject.defineBlock (0, 'MBTSHitDetails',CaloSysD3PDMaker.TileHitFillerTool)

