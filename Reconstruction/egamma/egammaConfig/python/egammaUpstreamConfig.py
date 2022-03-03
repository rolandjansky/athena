# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
        Instantiate things needed upstream the main egamma Reconstruction,
        could be all systems or just egamma related things
        (doAll default false).
        """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egUpstreamSystemCfg(flags,
                        name="egUpstreamSystem"):

    mlog = logging.getLogger(name)
    mlog.info('EGamma All Upstream Systems configs requested')
    acc = ComponentAccumulator()
    # AOD2xAOD Truth conversion
    if flags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
        acc.merge(GEN_AOD2xAODCfg(flags))

    # calorimeter
    if flags.Detector.EnableCalo:
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        acc.merge(CaloRecoCfg(flags))

    # ID / ITk
    if flags.Reco.EnableTracking:
        from InDetConfig.TrackRecoConfig import InDetTrackRecoCfg
        acc.merge(InDetTrackRecoCfg(flags))

    return acc


def egammaUpstreamCfg(flags,
                      doAll=False,
                      name="EGammaUpstream"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma Upstream configuration')

    acc = ComponentAccumulator()

    # In case we want to redo calo/tracking
    # from e/gamma config
    if doAll:
        acc.merge(egUpstreamSystemCfg(flags))

    # egamma selected/copied topo clusters
    if flags.Detector.EnableCalo:
        from egammaAlgs.egammaTopoClusterCopierConfig import (
            egammaTopoClusterCopierCfg)
        acc.merge(egammaTopoClusterCopierCfg(flags))

    # This is not a direct dependency
    if not flags.Input.isMC:
        from LumiBlockComps.LumiBlockMuWriterConfig import (
                LumiBlockMuWriterCfg)
        acc.merge(LumiBlockMuWriterCfg(flags))

    mlog.info("EGamma upstream configured")

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(egammaUpstreamCfg(flags, doAll=True))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammareconstructionconfig.pkl", "wb") as f:
        acc.store(f)
