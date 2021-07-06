# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Instantiate the two supercluster
builders with default configuration"""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from egammaTools.EMTrackMatchBuilderConfig import EMTrackMatchBuilderCfg
from egammaTools.EMConversionBuilderConfig import EMConversionBuilderCfg
from egammaTools.egammaMVACalibConfig import egammaMVASvcCfg
from egammaTools.egammaSwToolConfig import egammaSwToolCfg
from egammaCaloTools.egammaCaloToolsConf import egammaCheckEnergyDepositTool


def electronSuperClusterBuilderCfg(flags, name='electronSuperClusterBuilder', **kwargs):

    from egammaAlgs.egammaAlgsConf import electronSuperClusterBuilder

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "MVACalibSvc" not in kwargs:
        mvacal = egammaMVASvcCfg(flags)
        kwargs["MVACalibSvc"] = mvacal.popPrivateTools()
        acc.merge(mvacal)

    if "TrackMatchBuilderTool" not in kwargs:
        emtrkmatch = EMTrackMatchBuilderCfg(flags)
        kwargs["TrackMatchBuilderTool"] = emtrkmatch.popPrivateTools()
        acc.merge(emtrkmatch)

    if "ClusterCorrectionTool" not in kwargs:
        egswtool = egammaSwToolCfg(flags)
        kwargs["ClusterCorrectionTool"] = egswtool.popPrivateTools()
        acc.merge(egswtool)

    kwargs.setdefault(
        "InputEgammaRecContainerName",
        flags.Egamma.Keys.Internal.EgammaRecs)
    kwargs.setdefault(
        "SuperElectronRecCollectionName",
        flags.Egamma.Keys.Internal.ElectronSuperRecs)
    kwargs.setdefault(
        "egammaCheckEnergyDepositTool",
        egammaCheckEnergyDepositTool())
    kwargs.setdefault("EtThresholdCut", 1000)

    elscAlg = electronSuperClusterBuilder(name, **kwargs)

    acc.addEventAlgo(elscAlg)
    return acc


def photonSuperClusterBuilderCfg(
        flags,
        name='photonSuperClusterBuilder',
        **kwargs):

    from egammaAlgs.egammaAlgsConf import photonSuperClusterBuilder

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "MVACalibSvc" not in kwargs:
        mvacal = egammaMVASvcCfg(flags)
        kwargs["MVACalibSvc"] = mvacal.popPrivateTools()
        acc.merge(mvacal)

    if "ConversionBuilderTool" not in kwargs:
        emcnv = EMConversionBuilderCfg(flags)
        kwargs["ConversionBuilderTool"] = emcnv.popPrivateTools()
        acc.merge(emcnv)

    if "ClusterCorrectionTool" not in kwargs:
        egswtool = egammaSwToolCfg(flags)
        kwargs["ClusterCorrectionTool"] = egswtool.popPrivateTools()
        acc.merge(egswtool)

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
