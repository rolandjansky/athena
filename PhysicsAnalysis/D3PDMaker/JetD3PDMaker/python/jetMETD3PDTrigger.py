# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TriggerD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeJetMETTriggerBitsD3PDObject (name,
                                     prefix,
                                     object_name,
                                     getter = None,
                                     sgkey = ''):
    
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix,
                                                 ObjectName = object_name
                                                 )
# Create the object type.
jetMETTriggerBitsD3PDObject = D3PDObject (makeJetMETTriggerBitsD3PDObject,
                                          'jetMETTriggerBitsFiller',
                                          'jetMETTriggerBitsD3PDObject')


######
# Define blocks.


jetMETTriggerBitsD3PDObject.defineBlock (0, 'jetMETBits0',
                                         TriggerD3PDMaker.TriggerBitFillerTool ,
                                         Triggers=['EF_J30']
                                         )

jetMETTriggerBitsD3PDObject.defineBlock (1, 'jetMETBits1',
                                         TriggerD3PDMaker.TriggerBitFillerTool ,
                                         Triggers=['L1_J5',
                                                   'L1_J10',
                                                   'L1_J15',
                                                   ]
                                         )
