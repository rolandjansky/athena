# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTileL2D3PDObject (name, prefix, object_name='TileL2D3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'TileL2Cnt'
    if label == None: label = prefix

    
    print(" makeTileL2D3PDObject: name = ", name)
    print(" makeTileL2D3PDObject: prefix = ", prefix)
    print(" makeTileL2D3PDObject: object_name = ", object_name)
    print(" makeTileL2D3PDObject: sgkey = ", sgkey)

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileL2Container',
                  SGKey = sgkey,
                  Label = label)
        

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



TileL2D3PDObject=D3PDObject(makeTileL2D3PDObject,'TileL2Met_','TileL2D3PDObject')
    
TileL2D3PDObject.defineBlock (0, 
                              'TileL2',
                              CaloSysD3PDMaker.TileL2FillerTool,
                              SaveL2Details=True,
                              SaveMuRODDetails=False)
    
TileL2D3PDObject.defineBlock (2, 
                              'TileMuID_ROD',
                              CaloSysD3PDMaker.TileL2FillerTool,
                              SaveL2Details=False,
                              SaveMuRODDetails=True)






