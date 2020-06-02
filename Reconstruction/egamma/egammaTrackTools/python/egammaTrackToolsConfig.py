# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool configuration to instantiate all
 egammaCaloTools with default configuration"""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
EMExtrapolationTools = CompFactory.EMExtrapolationTools

# The extrapolator is not quite correct
#  we need to able to set the particular
#  egamma ones.


def EMExtrapolationToolsCfg(flags, **kwargs):

    mlog = logging.getLogger('EMExtrapolationTools')
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        extrapAcc = AtlasExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = extrapAcc.popPrivateTools()
        acc.merge(extrapAcc)

    if "PerigeeCaloExtensionTool" not in kwargs:
        perigeeCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="PerigeeCaloExtensionTool",
            Extrapolator=kwargs["Extrapolator"],
            ParticleType="electron",
            StartFromPerigee=True)
        kwargs["PerigeeCaloExtensionTool"] = (
            perigeeCaloExtrapAcc.popPrivateTools())
        acc.merge(perigeeCaloExtrapAcc)

    if "LastCaloExtensionTool" not in kwargs:
        lastCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="LastCaloExtensionTool",
            ParticleType="electron",
            Extrapolator=kwargs["Extrapolator"])

        kwargs["LastCaloExtensionTool"] = lastCaloExtrapAcc.popPrivateTools()
        acc.merge(lastCaloExtrapAcc)

    emExtrapolationTools = EMExtrapolationTools(**kwargs)
    acc.setPrivateTools(emExtrapolationTools)
    return acc


def EMExtrapolationToolsCacheCfg(flags, **kwargs):
    kwargs.setdefault("name", "EMExtrapolationToolsCache")
    kwargs.setdefault("useCaching", True)
    kwargs.setdefault("useLastCaching", True)
    return EMExtrapolationToolsCfg(flags, **kwargs)


# egammaTrkRefitterTool also needs a config, but depends on some
# tracking that is not ready
# CaloCluster_OnTrackBuilder is currently not used at all


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = [
        "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/myRDO.pool.root"]
    ConfigFlags.lock()

    cfg = ComponentAccumulator()
    cfg.printConfig()
    acc = EMExtrapolationToolsCfg(ConfigFlags)
    acc.popPrivateTools()
    cfg.merge(acc)
    acc = EMExtrapolationToolsCacheCfg(ConfigFlags)
    acc.popPrivateTools()
    cfg.merge(acc)

    f = open("egtracktools.pkl", "wb")
    cfg.store(f)
    f.close()
