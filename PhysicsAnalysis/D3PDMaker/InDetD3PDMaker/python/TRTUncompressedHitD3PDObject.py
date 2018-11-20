# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import InDetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from RecExConfig.RecFlags import rec

# would be needed for truth association:
#from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

def makeD3PDObject (name, prefix, object_name, getter = None, sgkey = None, label = None):
     if sgkey == None: sgkey = "TRTUncompressedHits"
     if label == None: label = prefix
     
     if not getter:
         getter = InDetD3PDMaker.TRTUncompressedHitContainerGetterTool \
                  (name + '_Getter',
                   TypeName = 'TRTUncompressedHitCollection',
                   SGKey = sgkey,
                   Label = label)
     getter.OutputLevel = 1
     return D3PDMakerCoreComps.VectorFillerTool (name,
                                                 Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name, OutputLevel=1)

TRTUncompressedHitD3PDObject = D3PDObject (makeD3PDObject, 'trthit_', 'TRTUncompressedHitD3PDObject')
 
TRTUncompressedHitD3PDObject.defineBlock (1, 'Hits', InDetD3PDMaker.TRTUncompressedHitFillerTool)

