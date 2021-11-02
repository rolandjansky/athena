# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """ Instantiate the EMGSFCaloExtensionBuilder
with default configuration """

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def EMGSFCaloExtensionBuilderCfg(
        flags,
        name='EMGSFCaloExtensionBuilder',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    from egammaTrackTools.egammaTrackToolsConfig import (
        EMParticleCaloExtensionToolCfg, EMLastCaloExtensionToolCfg)
    if "PerigeeCaloExtensionTool" not in kwargs:
        kwargs["PerigeeCaloExtensionTool"] = acc.popToolsAndMerge(
            EMParticleCaloExtensionToolCfg(flags))

    if "LastCaloExtensionTool" not in kwargs:
        kwargs["LastCaloExtensionTool"] = acc.popToolsAndMerge(
            EMLastCaloExtensionToolCfg(flags))

    kwargs.setdefault(
        "GFFTrkPartContainerName",
        flags.Egamma.Keys.Output.GSFTrackParticles)
    kwargs.setdefault("GSFPerigeeCache", "GSFPerigeeCaloExtension")
    kwargs.setdefault("GSFLastCache", "GSFLastCaloExtension")

    emgscaloextfAlg = CompFactory.EMGSFCaloExtensionBuilder(name, **kwargs)

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
