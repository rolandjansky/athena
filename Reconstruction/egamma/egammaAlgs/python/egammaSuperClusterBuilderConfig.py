# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Instantiate the two supercluster
builders with default configuration"""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from egammaTools.EMTrackMatchBuilderConfig import EMTrackMatchBuilderCfg
from egammaTools.EMConversionBuilderConfig import EMConversionBuilderCfg
from egammaTools.egammaSwToolConfig import egammaSwToolCfg
from egammaMVACalib.egammaMVACalibConfig import egammaMVASvcCfg


def electronSuperClusterBuilderCfg(flags,
                                   name='electronSuperClusterBuilder',
                                   sequenceName = None,
                                   **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    seqkw = {'sequence': sequenceName} if sequenceName else {}
    acc = ComponentAccumulator (**seqkw)

    if "TrackMatchBuilderTool" not in kwargs:
        emtrkmatch = EMTrackMatchBuilderCfg(flags)
        kwargs["TrackMatchBuilderTool"] = acc.popToolsAndMerge(emtrkmatch)

    if "ClusterCorrectionTool" not in kwargs:
        egswtool = egammaSwToolCfg(flags)
        kwargs["ClusterCorrectionTool"] = acc.popToolsAndMerge(egswtool)

    if "MVACalibSvc" not in kwargs:
        mvacal = egammaMVASvcCfg(flags)
        kwargs["MVACalibSvc"] = acc.getPrimaryAndMerge(mvacal)

    kwargs.setdefault(
        "InputEgammaRecContainerName",
        flags.Egamma.Keys.Internal.EgammaRecs)
    kwargs.setdefault(
        "SuperElectronRecCollectionName",
        flags.Egamma.Keys.Internal.ElectronSuperRecs)
    kwargs.setdefault(
        "egammaCheckEnergyDepositTool",
        CompFactory.egammaCheckEnergyDepositTool())
    kwargs.setdefault("EtThresholdCut", 1000)
    elscAlg = CompFactory.electronSuperClusterBuilder(name, **kwargs)

    acc.addEventAlgo(elscAlg)
    return acc


def photonSuperClusterBuilderCfg(
        flags,
        name='photonSuperClusterBuilder',
        sequenceName = None,
        **kwargs):

    seqkw = {'sequence': sequenceName} if sequenceName else {}
    acc = ComponentAccumulator (**seqkw)

    photonSuperClusterBuilder = CompFactory.photonSuperClusterBuilder
    egammaCheckEnergyDepositTool = CompFactory.egammaCheckEnergyDepositTool

    if "ConversionBuilderTool" not in kwargs:
        emcnv = EMConversionBuilderCfg(flags)
        kwargs["ConversionBuilderTool"] = acc.popToolsAndMerge(emcnv)

    if "ClusterCorrectionTool" not in kwargs:
        egswtool = egammaSwToolCfg(flags)
        kwargs["ClusterCorrectionTool"] = acc.popToolsAndMerge(egswtool)

    if "MVACalibSvc" not in kwargs:
        mvacal = egammaMVASvcCfg(flags)
        kwargs["MVACalibSvc"] = acc.getPrimaryAndMerge(mvacal)

    kwargs.setdefault(
        "InputEgammaRecContainerName",
        flags.Egamma.Keys.Internal.EgammaRecs)
    kwargs.setdefault(
        "SuperPhotonRecCollectionName",
        flags.Egamma.Keys.Internal.PhotonSuperRecs)
    kwargs.setdefault(
        "egammaCheckEnergyDepositTool",
        egammaCheckEnergyDepositTool())

    phscAlg = photonSuperClusterBuilder(name, **kwargs)

    acc.addEventAlgo(phscAlg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(electronSuperClusterBuilderCfg(flags))
    mlog = logging.getLogger("egammaSuperClusterBuilderConfigTest")
    mlog.info("Configuring  electronSuperClusterBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("electronSuperClusterBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    acc.merge(photonSuperClusterBuilderCfg(flags))
    mlog.info("Configuring  photonSuperClusterBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("photonSuperClusterBuilder"),
                    nestLevel=1,
                    printDefaults=True)

    with open("egammasuperclusterbuilder.pkl", "wb") as f:
        acc.store(f)
