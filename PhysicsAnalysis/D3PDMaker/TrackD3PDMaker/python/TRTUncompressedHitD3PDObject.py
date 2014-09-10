# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf
from RecExConfig.RecFlags import rec

# would be needed for truth association:
#from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

def makeD3PDObject (name, prefix, object_name, getter = None, sgkey = None, label = None):
     if sgkey == None: sgkey = "TRTUncompressedHits"
     if label == None: label = prefix
     
     if not getter:
         getter = TrackD3PDMaker.TRTUncompressedHitContainerGetterTool \
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
 
TRTUncompressedHitD3PDObject.defineBlock (1, 'Hits', TrackD3PDMaker.TRTUncompressedHitFillerTool)

# Truth matching
#if rec.doTruth() :
#    TruthAssoc = SimpleAssociation\
#                 (PixelSiHitD3PDObject,
#                  TrackD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
#    TruthAssoc.defineBlock (1, 'TruthAssocIndex', D3PDMakerCoreComps.IndexFillerTool, Target = 'mc')
