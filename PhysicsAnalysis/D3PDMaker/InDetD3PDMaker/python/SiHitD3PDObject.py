# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import InDetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from RecExConfig.RecFlags import rec

from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

def makeD3PDObject (name, prefix, object_name, getter = None,
                            sgkey = None,
                            label = None):
     if sgkey == None: sgkey = "SiHits"
     if label == None: label = prefix
     if prefix=="pixhit_" :
        sgkey = "PixelHits"
     if prefix=="scthit_" :
        sgkey = "SCT_Hits"


     
     if not getter:
         getter = InDetD3PDMaker.SiHitContainerGetterTool \
                  (name + '_Getter',
                   TypeName = 'SiHitCollection',
                   SGKey = sgkey,
                   Label = label)
     getter.OutputLevel=1    
     return D3PDMakerCoreComps.VectorFillerTool (name,
                                                 Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name, OutputLevel=1)

PixelSiHitD3PDObject = D3PDObject (makeD3PDObject, 'pixhit_', 'PixelSiHitD3PDObject')
 
PixelSiHitD3PDObject.defineBlock (1, 'Hits',
                                InDetD3PDMaker.SiHitFillerTool)

SctSiHitD3PDObject = D3PDObject (makeD3PDObject, 'scthit_', 'SctSiHitD3PDObject')
 
SctSiHitD3PDObject.defineBlock (1, 'Hits',
                                InDetD3PDMaker.SiHitFillerTool)


# Truth matching
#if rec.doTruth() :
if True :
    TruthAssoc = SimpleAssociation\
                 (PixelSiHitD3PDObject,
                  InDetD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
    TruthAssoc.defineBlock (1,
                                    'TruthAssocIndex',
                                    D3PDMakerCoreComps.IndexFillerTool,
                                    Target = 'mc')


    TruthAssoc = SimpleAssociation\
                 (SctSiHitD3PDObject,
                  InDetD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
    TruthAssoc.defineBlock (1,
                                    'TruthAssocIndex',
                                    D3PDMakerCoreComps.IndexFillerTool,
                                    Target = 'mc')





#            def _levelAssocToGP (reqlev, args, hookargs):
#                if reqlev < 1: return False
#                if hookargs.get ('TruthParticleTarget'): return False
#                if hookargs.get ('GenParticleTarget'):
#                    args['Target'] = hookargs.get ('GenParticleTarget')
#                return True
#            def _levelAssocToTP (reqlev, args, hookargs):
#                if reqlev < 1: return False
#                tpt = hookargs.get ('TruthParticleTarget')
#                if not tpt: return False
#                args['Target'] = tpt
#                return True
#
#            TruthAssoc = SimpleAssociation\
#                         (object,
#                          TruthAssociationTool,
#                          prefix = "mc_",
#                          SGKey = sgkey,
#                          MapKey = truthMapKey)
#            TruthAssoc.defineBlock (_levelAssocToGP,
#                                    'TruthAssocIndex',
#                                    D3PDMakerCoreComps.IndexFillerTool,
#                                    Target = truthTarget)
#77                                      truthTarget='mc',
#378                                      truthPrefix='mc_',
#379                                      detailedTruthPrefix='detailed_mc_',
#380                                      truthMapKey='TrackTruthCollection',
#381                                      SGKeyForTruth='Tracks',
#382                                      detailedTruthMapKey='DetailedTrackTruth',
#383                                      SGKeyForDetailedTruth='Tracks',
#384                                      flags=TrackD3PDFlags)

