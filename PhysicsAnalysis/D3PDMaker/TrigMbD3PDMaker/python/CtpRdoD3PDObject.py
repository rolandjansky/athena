# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## CTP RDO

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__CtpRdoFillerTool as CtpRdoFiller

def makeCtpRdoD3PDObject (name, prefix, object_name, getter = None,
                          sgkey = 'CTP_RDO',
                          label = 'ctpRdo'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'CTP_RDO')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

CtpRdoD3PDObject = D3PDObject (makeCtpRdoD3PDObject, 'ctpRDO_', 'CtpRdoD3PDObject')
CtpRdoD3PDObject.defineBlock (0, 'CTP_RDO',  CtpRdoFiller)
