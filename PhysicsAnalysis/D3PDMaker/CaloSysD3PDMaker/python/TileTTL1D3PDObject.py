# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTileTTL1D3PDObject (name, prefix, object_name='TileTTL1D3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'TileTTL1Cnt'
    if label == None: label = prefix

    
    print(" makeTileTTL1D3PDObject: name = ", name)
    print(" makeTileTTL1D3PDObject: prefix = ", prefix)
    print(" makeTileTTL1D3PDObject: object_name = ", object_name)
    print(" makeTileTTL1D3PDObject: sgkey = ", sgkey)

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileTTL1Container',
                  SGKey = sgkey,
                  Label = label)
        

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



TileTTL1D3PDObject=D3PDObject(makeTileTTL1D3PDObject,'TileTTL1_','TileTTL1D3PDObject')
    
TileTTL1D3PDObject.defineBlock (0, 
                              'TileTTL1',
                              CaloSysD3PDMaker.TileTTL1FillerTool)
