# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## CTP Decision

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__CtpDecisionFillerTool as CtpDecisionFiller

def makeCtpDecisionD3PDObject (name, prefix, object_name, getter = None,
                               sgkey = 'CTP_Decision'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'CTP_Decision')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

CtpDecisionD3PDObject = D3PDObject (makeCtpDecisionD3PDObject, 'ctp_',
                                    'CtpDecisionD3PDObject')
CtpDecisionD3PDObject.defineBlock (0, 'CTP Decision',  CtpDecisionFiller)
