# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def AddTrackD3PDComponents(alg, level=1, **kw):

    from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
    from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
    from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
    #from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
    #from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
    #from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
    from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
    from TrackD3PDMaker.V0D3PDObject import DefaultV0D3PDObject as V0D3PDObject
    from TrackD3PDMaker.PixelClusterD3PDObject import PixelClusterD3PDObject

    alg += V0D3PDObject(level)
    alg += PixelClusterD3PDObject(**_args(level, 'PixelCluster', kw))
    alg += TruthTrackD3PDObject(**_args(level, 'TruthTrack', kw))
    alg += TruthVertexD3PDObject(**_args(level, 'TruthVertex', kw))
    alg += TrackParticleD3PDObject(**_args(level, 'Track', kw))
    alg += PrimaryVertexD3PDObject(**_args(level, 'PrimaryVertex', kw))
##     alg += PixelTrackD3PDObject(level)
##     alg += SCTTrackD3PDObject(level)
##     alg += TRTTrackD3PDObject(level)

def TrackD3PD (file,
               level = 4,
               tuplename = 'track',
               seq = topSequence,
               D3PDSvc = 'D3PD::RootD3PDSvc',
               **kw):

    from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
    from TrigMbD3PDMaker.CtpRdoD3PDObject         import CtpRdoD3PDObject

    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq, file = file, D3PDSvc = D3PDSvc)
    alg += EventInfoD3PDObject (level)
    alg += CtpRdoD3PDObject(level)
    AddTrackD3PDComponents(alg, level, **kw)
    return alg
