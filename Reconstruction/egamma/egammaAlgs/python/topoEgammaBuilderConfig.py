# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the two supercluster
          builders with default configuration
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from egammaTools.EMClusterToolConfig import EMClusterToolCfg
from egammaTools.EMShowerBuilderConfig import EMShowerBuilderCfg
from egammaTools.egammaOQFlagsBuilderConfig import egammaOQFlagsBuilderCfg
from egammaTools.EMPIDBuilderConfig import (
    EMPIDBuilderElectronCfg, EMPIDBuilderPhotonCfg)


def topoEgammaBuilderCfg(flags, name='topoEgammaBuilder',
                         sequenceName = None,
                         **kwargs):

    seqkw = {'sequence': sequenceName} if sequenceName else {}
    acc = ComponentAccumulator (**seqkw)

    if "EMClusterTool" not in kwargs:
        emclustool = EMClusterToolCfg(flags)
        kwargs["EMClusterTool"] = acc.popToolsAndMerge(emclustool)

    if "EMShowerTool" not in kwargs:
        emshowerbuilder = EMShowerBuilderCfg(flags)
        kwargs["EMShowerTool"] = acc.popToolsAndMerge(emshowerbuilder)

    if "ObjectQualityTool" not in kwargs:
        oqtool = egammaOQFlagsBuilderCfg(flags)
        kwargs["ObjectQualityTool"] = acc.popToolsAndMerge(oqtool)

    egammaTools = [CompFactory.EMFourMomBuilder()]
    eleTools = [acc.popToolsAndMerge(EMPIDBuilderElectronCfg(flags))]
    phoTools = [acc.popToolsAndMerge(EMPIDBuilderPhotonCfg(flags))]

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
        CompFactory.EGammaAmbiguityTool())
    kwargs.setdefault(
        "egammaTools",
        egammaTools)
    kwargs.setdefault(
        "ElectronTools",
        eleTools)
    kwargs.setdefault(
        "PhotonTools",
        phoTools)
    kwargs.setdefault(
        "isTruth",
        flags.Input.isMC
    )

    topoegAlg = CompFactory.xAODEgammaBuilder(name, **kwargs)

    acc.addEventAlgo(topoegAlg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.lock()

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
