# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## SP LVL2

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__SpLvl2FillerTool as SpLvl2Filler

def makeSpLvl2D3PDObject (name, prefix, object_name, getter = None,
                            sgkey = 'HLT_spacepoints',
                            label = 'sp_lvl2'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'TrigSpacePointCountsCollection')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

SpLvl2D3PDObject = D3PDObject (makeSpLvl2D3PDObject, 'lvl2_sp_', 'SpLvl2D3PDObject')
SpLvl2D3PDObject.defineBlock (0, 'SP LVL2',  SpLvl2Filler)
