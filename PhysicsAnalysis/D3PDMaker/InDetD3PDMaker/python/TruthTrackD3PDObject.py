# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from InDetD3PDMaker import TruthTrackFillerTool
from TruthD3PDMaker import GenParticlePerigeeFillerTool
from TruthD3PDMaker import GenParticleFillerTool
from InDetRecExample.InDetKeys import InDetKeys

## Track Particles Truth
def makeTruthTrackD3PDObject (name, prefix, object_name, getter = None,
                              sgkey = "TruthEvent,GEN_AOD",
                              typename = 'McEventCollection',
                              label = 'mc'):
    if not getter:
        from InDetD3PDMaker import TruthTrackGetterTool
        getter = TruthTrackGetterTool (name + '_Getter',
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

TruthTrackD3PDObject.defineBlock (0, 'TruthTrackGenParticleInfo', GenParticleFillerTool, prefix='gen_', WriteE = True, WriteM = False)
TruthTrackD3PDObject.defineBlock (0, 'TruthTrackPerigee',  GenParticlePerigeeFillerTool, prefix='perigee_')
TruthTrackD3PDObject.defineBlock (0, 'TruthTrackParameters',  TruthTrackFillerTool)
