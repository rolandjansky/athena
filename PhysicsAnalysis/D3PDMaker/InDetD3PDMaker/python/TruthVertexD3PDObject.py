# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

## Truth Vertices

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from InDetD3PDMaker import TruthVertexFillerTool
from InDetRecExample.InDetKeys import InDetKeys

def makeTruthVertexD3PDObject (name, prefix, object_name, getter = None,
                               sgkey = "TruthEvent,GEN_AOD",
                               label = 'mcVx'):
    
    if not getter:
        from InDetD3PDMaker import TruthVertexGetterTool
        getter = TruthVertexGetterTool (name + '_Getter',
                                        SGKey = sgkey,
                                        TypeName = 'McEventCollection',
                                        Label = label)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())

TruthVertexD3PDObject = D3PDObject (makeTruthVertexD3PDObject, 'mcVx_',
                                    'TruthVertexD3PDObject')
TruthVertexD3PDObject.defineBlock (0, 'TruthVxPosition',  TruthVertexFillerTool)
