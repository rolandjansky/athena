# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def egammaTrackSlimmerCfg(
        flags,
        name='egammaTrackSlimmer',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault("StreamName", "StreamAOD")
    kwargs.setdefault("InputElectronContainerName",
                      flags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault("InputPhotonContainerName",
                      flags.Egamma.Keys.Output.Photons)
    kwargs.setdefault("TrackParticleContainerName",
                      flags.Egamma.Keys.Output.GSFTrackParticles)
    kwargs.setdefault("VertexContainerName",
                      flags.Egamma.Keys.Output.ConversionVertices)

    acc.addEventAlgo(CompFactory.egammaTrackSlimmer(name, **kwargs))

    # To use egamma CA within standard config
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.ESD

    acc = MainServicesCfg(flags)
    mlog = logging.getLogger("egammaTrackSlimmerConfigTest")
    mlog.info("Configuring  egammaTrackSlimmer: ")
    acc.merge(egammaTrackSlimmerCfg(flags))
    printProperties(mlog,
                    acc.getEventAlgo("egammaTrackSlimmer"),
                    nestLevel=1,
                    printDefaults=True)
    with open("egammatrackslimmer.pkl", "wb") as f:
        acc.store(f)
