# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## MBTS Container

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__MbtsContainerFillerTool as MbtsContainerFiller

def makeMbtsContainerD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'MBTSContainer',
                          label = 'mbtsContainer'):
    if not getter:
        from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__MbtsCollectionGetterTool
        getter = TrigMbD3PD__MbtsCollectionGetterTool(name + '_Getter',
                                                       SGKey = sgkey,
                                                       Label = label,
                                                       TypeName = 'TileCellContainer')

    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name)

MbtsContainerD3PDObject = D3PDObject (makeMbtsContainerD3PDObject, 'mbts_',
                                      'MbtsContainerD3PDObject')
MbtsContainerD3PDObject.defineBlock (0, 'MBTS Tile Cells',  MbtsContainerFiller)
