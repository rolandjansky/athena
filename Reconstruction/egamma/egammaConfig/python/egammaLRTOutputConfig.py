# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Prepare LRT EGamma output list"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaLRTOutputCfg(flags, name="LRTEGOutputList"):

    # This need some work form LRT community
    # ....
    acc = ComponentAccumulator()

    mlog = logging.getLogger(name)

    outFlags = flags.Egamma.Keys.Output

    toESD = []
    toAOD = []

    toESD += [
        f"xAOD::ElectronContainer#LRT{outFlags.Electrons}",
        f"xAOD::ElectronAuxContainer#LRT{outFlags.Electrons}"
        f"Aux.{outFlags.ElectronsSuppESD}"]
    toESD += [
        f"xAOD::CaloClusterContainer#LRT{outFlags.CaloClusters}",
        f"xAOD::CaloClusterAuxContainer#LRT{outFlags.CaloClusters}"
        f"Aux.{outFlags.CaloClustersSuppESD}"]
    toESD += [
        f"xAOD::CaloClusterContainer#LRT{outFlags.EgammaLargeClusters}",
        f"xAOD::CaloClusterAuxContainer#LRT{outFlags.EgammaLargeClusters}"
        f"Aux.{outFlags.EgammaLargeClustersSuppESD}"]
    toESD += [
        f"CaloClusterCellLinkContainer#LRT{outFlags.CaloClusters}"
        "_links"]
    toESD += [
        f"CaloClusterCellLinkContainer#LRT{outFlags.EgammaLargeClusters}"
        "_links"]
    toESD += [
        f"xAOD::TrackParticleContainer#LRT{outFlags.GSFTrackParticles}",
        f"xAOD::TrackParticleAuxContainer#LRT{outFlags.GSFTrackParticles}"
        f"Aux.{outFlags.GSFTrackParticlesSuppESD}"]

    toAOD += [
        f"xAOD::ElectronContainer#LRT{outFlags.Electrons}",
        f"xAOD::ElectronAuxContainer#LRT{outFlags.Electrons}"
        f"Aux.{outFlags.ElectronsSuppAOD}"]
    toAOD += [
        f"xAOD::CaloClusterContainer#LRT{outFlags.CaloClusters}",
        f"xAOD::CaloClusterAuxContainer#LRT{outFlags.CaloClusters}"
        f"Aux.{outFlags.CaloClustersSuppAOD}"]
    toAOD += [
        f"CaloClusterCellLinkContainer#LRT{outFlags.CaloClusters}"
        "_links"]
    toAOD += [
        f"xAOD::TrackParticleContainer#LRT{outFlags.GSFTrackParticles}",
        f"xAOD::TrackParticleAuxContainer#LRT{outFlags.GSFTrackParticles}"
        f"Aux.{outFlags.GSFTrackParticlesSuppAOD}"]

    if flags.Output.doWriteESD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToESD
        acc.merge(addToESD(flags, toESD))
        mlog.info('egammaESDList: %s ', toESD)

    if flags.Output.doWriteAOD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToAOD
        acc.merge(addToAOD(flags, toAOD))
        mlog.info('egammaAODList: %s ', toAOD)

    mlog.info("EGamma LRT Output configured")
    return acc
