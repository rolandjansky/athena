# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the InDetForwardTrackParticles Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def ThinInDetForwardTrackParticlesCfg(flags, name="ThinInDetForwardTrackParticlesAlg", **kwargs):

    mlog = logging.getLogger(name)
    mlog.info("Starting InDetForwardTrackParticles Thinning configuration")
    acc = ComponentAccumulator()

    if "xAOD::TrackParticleContainer#InDetForwardTrackParticles" not in flags.Input.TypedCollections and not (flags.Detector.GeometryPixel and flags.InDet.Tracking.doForwardTracks):
        mlog.info("Not attempting to thin InDetForwardParticles, because the container InDetForwardTrackParticles does not seem to be available")
        return acc

    kwargs.setdefault("StreamName", "StreamAOD")
    acc.addEventAlgo(CompFactory.ThinInDetForwardTrackParticlesAlg(name, **kwargs))
    mlog.info("InDetForwardTrackParticles Thinning configured")
    return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(ThinInDetForwardTrackParticlesCfg(flags))
    mlog = logging.getLogger("ThinInDetForwardTrackParticlesConfigTest")
    printProperties(
        mlog,
        acc.getEventAlgo("ThinInDetForwardTrackParticlesAlg"),
        nestLevel=1,
        printDefaults=True,
    )

    with open("thinindetforwardtrackparticlescfg.pkl", "wb") as f:
        acc.store(f)
