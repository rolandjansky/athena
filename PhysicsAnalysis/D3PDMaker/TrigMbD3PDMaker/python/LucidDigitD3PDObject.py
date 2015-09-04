# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__LucidDigitFillerTool as LucidDigitFiller

def makeLucidDigitD3PDObject(name, prefix, object_name, getter = None, sgkey = 'Lucid_Digits', label = 'lucidDigit'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool(name + '_Getter', SGKey = sgkey, TypeName = 'LUCID_DigitContainer')
        return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix, Getter = getter, ObjectName = object_name)

LucidDigitD3PDObject = D3PDObject(makeLucidDigitD3PDObject, 'lucidDigit_', 'LucidDigitD3PDObject')
LucidDigitD3PDObject.defineBlock(0, 'Lucid_Digit', LucidDigitFiller)
