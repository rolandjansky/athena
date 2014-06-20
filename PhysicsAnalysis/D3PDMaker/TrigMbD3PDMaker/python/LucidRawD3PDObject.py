# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__LucidRawDataFillerTool as LucidRawDataFiller

def makeLucidRawD3PDObject(name, prefix, object_name, getter = None, sgkey = 'Lucid_RawData', label = 'lucidRaw'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool(name + '_Getter', SGKey = sgkey, TypeName = 'LUCID_RawDataContainer')
        return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix, Getter = getter, ObjectName = object_name)

LucidRawD3PDObject = D3PDObject(makeLucidRawD3PDObject, 'lucidRaw_', 'LucidRawD3PDObject')
LucidRawD3PDObject.defineBlock(0, 'Lucid_Raw', LucidRawDataFiller)
