# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaTrackSlimmer = CompFactory.egammaTrackSlimmer


def egammaTrackSlimmerCfg(
        flags,
        name='egammaTrackSlimmer',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault("StreamName", 'StreamAOD')
    kwargs.setdefault("InputElectronContainerName",
                      flags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault("InputPhotonContainerName",
                      flags.Egamma.Keys.Output.Photons)
    kwargs.setdefault("TrackParticleContainerName",
                      flags.Egamma.Keys.Output.GSFTrackParticles)
    kwargs.setdefault("VertexContainerName",
                      flags.Egamma.Keys.Output.ConversionVertices)
    kwargs.setdefault("InDetTrackParticleContainerName",
                      flags.Egamma.Keys.Input.TrackParticles)

    egtrkslimmerAlg = egammaTrackSlimmer(name, **kwargs)

    acc.addEventAlgo(egtrkslimmerAlg)
    return acc
