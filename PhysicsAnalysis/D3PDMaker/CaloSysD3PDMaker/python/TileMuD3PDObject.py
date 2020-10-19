# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTileMuD3PDObject (name, prefix, object_name='TileMuD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'TileMuObj'
    if label == None: label = prefix

    
    print(" makeTileMuD3PDObject: name = ", name)
    print(" makeTileMuD3PDObject: prefix = ", prefix)
    print(" makeTileMuD3PDObject: object_name = ", object_name)
    print(" makeTileMuD3PDObject: sgkey = ", sgkey)

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileMuContainer',
                  SGKey = sgkey,
                  Label = label)
        

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



TileMuD3PDObject=D3PDObject(makeTileMuD3PDObject,'TileMuId_','TileMuD3PDObject')
    
TileMuD3PDObject.defineBlock (0, 'TileMus',
                              CaloSysD3PDMaker.TileMuFillerTool)
