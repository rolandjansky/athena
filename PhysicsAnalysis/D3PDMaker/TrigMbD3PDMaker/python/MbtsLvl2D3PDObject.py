# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## MBTS LVL2

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__MbtsLvl2FillerTool as MbtsLvl2Filler

def makeMbtsLvl2D3PDObject (name, prefix, object_name, getter = None,
                            sgkey = 'HLT_T2Mbts',
                            label = 'mbts_lvl2'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'TrigT2MbtsBitsContainer')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

MbtsLvl2D3PDObject = D3PDObject (makeMbtsLvl2D3PDObject, 'mbtsLvl2_',
                                 'MbtsLvl2D3PDObject')
MbtsLvl2D3PDObject.defineBlock (0, 'MBTS LVL2',  MbtsLvl2Filler)
