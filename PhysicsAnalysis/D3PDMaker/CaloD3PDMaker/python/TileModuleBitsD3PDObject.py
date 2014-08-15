# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTileModuleBitsD3PDObject (name, prefix, object_name='TileModuleBitsD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = "TileRawChannelFlt"
    if label == None: label = prefix

    if not getter:
        getter = CaloD3PDMaker.SGTileModuleBitsGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileRawChannelContainer',
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



TileModuleBitsD3PDObject = D3PDObject (makeTileModuleBitsD3PDObject, 'tilemodule_', 'TileModuleBitsD3PDObject')

TileModuleBitsD3PDObject.defineBlock (0, 'Bits',
                                CaloD3PDMaker.TileModuleBitsFillerTool
                                )

