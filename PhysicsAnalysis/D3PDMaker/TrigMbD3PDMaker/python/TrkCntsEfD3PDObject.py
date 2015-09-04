# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Trigger Track Counts

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrigMbD3PDMaker.TrigMbD3PDMakerConf import TrigMbD3PD__TrkCntsEfFillerTool as TrkCntsEfFiller

def makeTrkCntsEfD3PDObject (name, prefix, object_name, getter = None,
                             sgkey = 'HLT_trackcounts',
                             label = 'trk_cnts_ef'):
    if not getter:
        getter = D3PDMakerCoreComps.SGObjGetterTool (name + '_Getter',
                                                     SGKey = sgkey,
                                                     TypeName = 'TrigTrackCountsCollection')

    return  D3PDMakerCoreComps.ObjFillerTool (name, Prefix = prefix,
                                              Getter = getter,
                                              ObjectName = object_name)

TrkCntsEfD3PDObject = D3PDObject (makeTrkCntsEfD3PDObject, 'ef_trk_', 'TrkCntsEfD3PDObject')
TrkCntsEfD3PDObject.defineBlock (0, 'TrkCntsEf',  TrkCntsEfFiller)
