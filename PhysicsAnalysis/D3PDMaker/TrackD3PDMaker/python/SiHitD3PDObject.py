# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf
from RecExConfig.RecFlags import rec

from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

#def SiHitD3PDObject(_label='sihit',
#                    _prefix='sihit_',
#                    _sgkey='SiHits',
#                    _object_name='SiHitD3PDObject',
#                    typeName='SiHitCollection'):
#
#    #object = make_SGDataVector_D3PDObject (typeName,
#    #                                       _sgkey,
#    #                                       _prefix,
#    #                                       _object_name,
#    #                                       default_allowMissing = True,
#    #                                       default_label = _label)
    




#    object.defineBlock (1, 'Hits', TrackD3PDMaker.SiHitFillerTool)
#    if rec.doTruth() :
#       TruthAssoc = SimpleAssociation\
#                 (object,
#                  TrackD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
#
#    return object

#PixelSiHitD3PDObject = SiHitD3PDObject(_label='pixhit',
#                    _prefix='pixhit_',
#                    _sgkey='PixelHits',
#                    _object_name='PixelSiHitD3PDObject')
#
#SctSiHitD3PDObject = SiHitD3PDObject(_label='scthit',
#                    _prefix='scthit_',
#                    _sgkey='SCT_Hits',
#                    _object_name='SctSiHitD3PDObject')
 
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
         getter = TrackD3PDMaker.SiHitContainerGetterTool \
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
                                TrackD3PDMaker.SiHitFillerTool)

SctSiHitD3PDObject = D3PDObject (makeD3PDObject, 'scthit_', 'SctSiHitD3PDObject')
 
SctSiHitD3PDObject.defineBlock (1, 'Hits',
                                TrackD3PDMaker.SiHitFillerTool)


# Truth matching
#if rec.doTruth() :
if True :
    TruthAssoc = SimpleAssociation\
                 (PixelSiHitD3PDObject,
                  TrackD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
    TruthAssoc.defineBlock (1,
                                    'TruthAssocIndex',
                                    D3PDMakerCoreComps.IndexFillerTool,
                                    Target = 'mc')


    TruthAssoc = SimpleAssociation\
                 (SctSiHitD3PDObject,
                  TrackD3PDMaker.SiHitTruthAssociationTool, prefix="mc_")
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

