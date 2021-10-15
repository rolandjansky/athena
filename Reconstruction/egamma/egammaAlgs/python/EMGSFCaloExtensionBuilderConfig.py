# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """ Instantiate the EMGSFCaloExtensionBuilder
with default configuration """

from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


EMGSFCaloExtensionBuilder = CompFactory.EMGSFCaloExtensionBuilder


def EMGSFCaloExtensionBuilderCfg(
        flags,
        name='EMGSFCaloExtensionBuilder',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "PerigeeCaloExtensionTool" not in kwargs:
        perigeeCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="PerigeeCaloExtensionTool",
            ParticleType="electron",
            StartFromPerigee=True)
        kwargs["PerigeeCaloExtensionTool"] = perigeeCaloExtrapAcc.popPrivateTools()
        acc.merge(perigeeCaloExtrapAcc)

    if "LastCaloExtensionTool" not in kwargs:
        lastCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="LastCaloExtensionTool",
            ParticleType="electron")
        kwargs["LastCaloExtensionTool"] = lastCaloExtrapAcc.popPrivateTools()
        acc.merge(lastCaloExtrapAcc)

    kwargs.setdefault(
        "GFFTrkPartContainerName",
        flags.Egamma.Keys.Output.GSFTrackParticles)

    emgscaloextfAlg = EMGSFCaloExtensionBuilder(name, **kwargs)

    acc.addEventAlgo(emgscaloextfAlg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO

    acc = MainServicesCfg(flags)
    acc.merge(EMGSFCaloExtensionBuilderCfg(flags))
    mlog = logging.getLogger("EMGSFCaloExtensionBuilderConfigTest")
    mlog.info("Configuring  EMGSFCaloExtensionBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("EMGSFCaloExtensionBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("emgsfcaloextensionbuilder.pkl", "wb") as f:
        acc.store(f)
