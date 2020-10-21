# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import CaloSysD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTileTriggerD3PDObject (name, prefix, object_name='TileTriggerD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'TileTriggerCnt'
    if label == None: label = prefix

    
    print(" makeTileTriggerD3PDObject: name = ", name)
    print(" makeTileTriggerD3PDObject: prefix = ", prefix)
    print(" makeTileTriggerD3PDObject: object_name = ", object_name)
    print(" makeTileTriggerD3PDObject: sgkey = ", sgkey)

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = 'TileTriggerContainer',
                  SGKey = sgkey,
                  Label = label)
        

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())



TileTriggerD3PDObject=D3PDObject(makeTileTriggerD3PDObject,'TileTrigger_','TileTriggerD3PDObject')
    
TileTriggerD3PDObject.defineBlock (0, 
                              'TileTrigger',
                              CaloSysD3PDMaker.TileTriggerFillerTool)
