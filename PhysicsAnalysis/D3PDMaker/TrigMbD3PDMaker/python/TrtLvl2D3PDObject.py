# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## TRT LVL2

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__TrtLvl2FillerTool as TrtLvl2Filler

def makeTrtLvl2D3PDObject (name, prefix, object_name, getter = None,
                            sgkey = 'HLT_TrtHitCount',
                            label = 'trt_lvl2'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'TrigTrtHitCountsCollection')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

TrtLvl2D3PDObject = D3PDObject (makeTrtLvl2D3PDObject, 'lvl2_mbTrt_', 'TrtLvl2D3PDObject')
TrtLvl2D3PDObject.defineBlock (0, 'TRT LVL2',  TrtLvl2Filler)
