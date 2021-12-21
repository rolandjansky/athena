# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the Egamma related xAOD Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaxAODThinningCfg(flags, name="EGammaxAODThinning"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma xAOD Thinning configuration')

    acc = ComponentAccumulator()
    # Add e/gamma track thinning
    # (although we call the Alg slimming)
    if flags.Egamma.doTrackThinning:
        from egammaAlgs.egammaTrackSlimmerConfig import (
            egammaTrackSlimmerCfg)
        acc.merge(egammaTrackSlimmerCfg(flags))

    # Add the Cell Thinning for egamma and egammaLargeClusters
    outFlags = flags.Egamma.Keys.Output
    allClusters = [outFlags.CaloClusters, outFlags.EgammaLargeClusters,
                   outFlags.ForwardClusters, outFlags.EgammaLargeFWDClusters]
    samplings = ['TileGap1', 'TileGap2', "TileGap3",
                 'TileBar0', 'TileExt0', 'HEC0']
    from CaloRec.CaloThinCellsByClusterAlgConfig import (
        CaloThinCellsByClusterAlgCfg)
    for clus in allClusters:
        acc.merge(CaloThinCellsByClusterAlgCfg(
            flags,
            streamName='StreamAOD',
            clusters=clus,
            samplings=samplings))

    mlog.info("EGamma xAOD Thinning configured")
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.ESD
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(egammaxAODThinningCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammaxaodthinningconfig.pkl", "wb") as f:
        acc.store(f)
