# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import MuonD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


def makeD3PDObject (name, prefix, object_name, getter = None,
                    sgkey = 'RPC_Measurements',
                    typename = 'Muon::RpcPrepDataContainer',
                    label = 'RpcPrepData'):
    if not getter:
        from MuonD3PDMaker.MuonD3PDMakerConf import D3PD__RpcPrepDataCollectionGetterTool
        getter = D3PD__RpcPrepDataCollectionGetterTool \
                 (name + '_Getter',
                  SGKey = sgkey,
                  TypeName = typename,
                  Label = label)
    getter.OutputLevel=1    
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name, OutputLevel=1)


RpcPrepDataD3PDObject =  D3PDObject (makeD3PDObject, 'rpc_', 'RpcPrepDataD3PDObject')

RpcPrepDataD3PDObject.defineBlock (0, 'PrepData', MuonD3PDMaker.RpcPrepDataFillerTool)
