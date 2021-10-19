# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the two supercluster
          builders with default configuration
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
xAODEgammaBuilder = CompFactory.xAODEgammaBuilder
EGammaAmbiguityTool = CompFactory.EGammaAmbiguityTool


def topoEgammaBuilderCfg(flags, name='topoEgammaBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault(
        "InputElectronRecCollectionName",
        flags.Egamma.Keys.Internal.ElectronSuperRecs)
    kwargs.setdefault(
        "InputPhotonRecCollectionName",
        flags.Egamma.Keys.Internal.PhotonSuperRecs)
    kwargs.setdefault(
        "ElectronOutputName",
        flags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault(
        "PhotonOutputName",
        flags.Egamma.Keys.Output.Photons)
    kwargs.setdefault(
        "AmbiguityTool",
        EGammaAmbiguityTool())
    kwargs.setdefault(
        "isTruth",
        flags.Input.isMC
    )

    topoegAlg = xAODEgammaBuilder(name, **kwargs)

    acc.addEventAlgo(topoegAlg)
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
    acc.merge(topoEgammaBuilderCfg(flags))
    mlog = logging.getLogger("topoEgammaBuilderConfigTest")
    mlog.info("Configuring  topoEgammaBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("topoEgammaBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("topoegammabuilder.pkl", "wb") as f:
        acc.store(f)
