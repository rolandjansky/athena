# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__TruthTrackFillerTool as TruthTrackFiller
from TruthD3PDMaker.TruthD3PDMakerConf import D3PD__GenParticlePerigeeFillerTool as GenParticlePerigeeFiller
from TruthD3PDMaker.TruthD3PDMakerConf import D3PD__GenParticleFillerTool as GenParticleFiller
from InDetRecExample.InDetKeys import InDetKeys

## Track Particles Truth
def makeTruthTrackD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = "TruthEvent,GEN_AOD",
                              typename = 'McEventCollection',
                              label = 'mc'):
    if not getter:
        from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__TruthTrackGetterTool
        getter = D3PD__TruthTrackGetterTool (name + '_Getter',
                                             SGKey = sgkey,
                                             TypeName = typename,
                                             Label = label,
                                             KeepOnlyPrimaryParticles = False,
                                             KeepOnlyChargedParticles = False,
                                             KeepV0 = True,
                                             TrueTrackPtCut = 0,
                                             TrueTrackEtaCut = 99999.,
                                             MaxRAll = 2000000,
                                             MaxZAll = 2000000,
                                             MaxRPrimary = 25,
                                             MaxZPrimary = 200)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix,
                                                 Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())

## Track Truth
TruthTrackD3PDObject = D3PDObject (makeTruthTrackD3PDObject, 'mc_', 'TruthTrackD3PDObject')

TruthTrackD3PDObject.defineBlock (0, 'TruthTrackGenParticleInfo', GenParticleFiller, prefix='gen_', WriteE = True, WriteM = False)
TruthTrackD3PDObject.defineBlock (0, 'TruthTrackPerigee',  GenParticlePerigeeFiller, prefix='perigee_')
TruthTrackD3PDObject.defineBlock (0, 'TruthTrackParameters',  TruthTrackFiller)
