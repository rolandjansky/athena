# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## BCM RDO

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__BcmRdoFillerTool as BcmRdoFiller

def makeBcmRdoD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'BCM_RDOs',
                          label = 'bcmRdo'):
    if not getter:
        from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__BcmRdoGetterTool
        getter = TrigMbD3PD__BcmRdoGetterTool ( name + '_Getter',
                                                 SGKey = sgkey,
                                                 TypeName = 'BCM_RDO_Container',
                                                 Label = label )
        
    return  D3PDMakerCoreComps.VectorFillerTool ( name, Prefix = prefix,
                                                  Getter = getter,
                                                  ObjectName = object_name )

BcmRdoD3PDObject = D3PDObject ( makeBcmRdoD3PDObject, 'bcmRDO_', 'BcmRdoD3PDObject' )
BcmRdoD3PDObject.defineBlock (0, 'BCM_RDO',  BcmRdoFiller)
