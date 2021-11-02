# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Prepare EGamma output list"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaWriteOutputCfg(flags, name="EGOutputList"):

    acc = ComponentAccumulator()

    mlog = logging.getLogger(name)

    # if not enabled add nothing
    if not flags.Egamma.enabled:
        return acc

    outFlags = flags.Egamma.Keys.Output

    toESD = []
    toAOD = []

    if flags.Egamma.doCaloSeeded:
        toESD += [
            f"xAOD::ElectronContainer#{outFlags.Electrons}",
            f"xAOD::ElectronAuxContainer#{outFlags.Electrons}"
            f"Aux.{outFlags.ElectronsSuppESD}"]
        toESD += [
            f"xAOD::PhotonContainer#{outFlags.Photons}",
            f"xAOD::PhotonAuxContainer#{outFlags.Photons}"
            f"Aux.{outFlags.PhotonsSuppESD}"]
        toESD += [
            f"xAOD::CaloClusterContainer#{outFlags.CaloClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.CaloClusters}"
            f"Aux.{outFlags.CaloClustersSuppESD}"]
        toESD += [
            f"xAOD::CaloClusterContainer#{outFlags.EgammaLargeClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.EgammaLargeClusters}"
            f"Aux.{outFlags.EgammaLargeClustersSuppESD}"]
        toESD += [
            f"CaloClusterCellLinkContainer#{outFlags.CaloClusters}"
            "_links"]
        toESD += [
            f"CaloClusterCellLinkContainer#{outFlags.EgammaLargeClusters}"
            "_links"]

        toAOD += [
            f"xAOD::ElectronContainer#{outFlags.Electrons}",
            f"xAOD::ElectronAuxContainer#{outFlags.Electrons}"
            f"Aux.{outFlags.ElectronsSuppAOD}"]
        toAOD += [
            f"xAOD::PhotonContainer#{outFlags.Photons}",
            f"xAOD::PhotonAuxContainer#{outFlags.Photons}"
            f"Aux.{outFlags.PhotonsSuppAOD}"]
        toAOD += [
            f"xAOD::CaloClusterContainer#{outFlags.CaloClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.CaloClusters}"
            f"Aux.{outFlags.CaloClustersSuppAOD}"]
        toAOD += [
            f"CaloClusterCellLinkContainer#{outFlags.CaloClusters}_links"]

    if flags.Egamma.doForwardSeeded:
        toESD += [
            f"xAOD::ElectronContainer#{outFlags.ForwardElectrons}",
            f"xAOD::ElectronAuxContainer#{outFlags.ForwardElectrons}"
            f"Aux.{outFlags.ForwardElectronsSuppESD}"]
        toESD += [
            f"xAOD::CaloClusterContainer#{outFlags.ForwardClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.ForwardClusters}"
            f"Aux.{outFlags.ForwardClustersSuppESD}"]
        toESD += [
            f"xAOD::CaloClusterContainer#{outFlags.EgammaLargeFWDClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.EgammaLargeFWDClusters}"
            f"Aux.{outFlags.EgammaLargeFWDClustersSuppESD}"]
        toESD += [
            f"CaloClusterCellLinkContainer#{outFlags.ForwardClusters}"
            "_links"]
        toESD += [
            f"CaloClusterCellLinkContainer#{outFlags.EgammaLargeFWDClusters}"
            "_links"]

        toAOD += [
            f"xAOD::ElectronContainer#{outFlags.ForwardElectrons}",
            f"xAOD::ElectronAuxContainer#{outFlags.ForwardElectrons}"
            f"Aux.{outFlags.ForwardElectronsSuppAOD}"]
        toAOD += [
            f"xAOD::CaloClusterContainer#{outFlags.ForwardClusters}",
            f"xAOD::CaloClusterAuxContainer#{outFlags.ForwardClusters}"
            f"Aux.{outFlags.ForwardClustersSuppAOD}"]
        toAOD += [
            f"CaloClusterCellLinkContainer#{outFlags.ForwardClusters}"
            "_links"]

    if flags.Egamma.doGSF:
        toESD += [
            f"xAOD::TrackParticleContainer#{outFlags.GSFTrackParticles}",
            f"xAOD::TrackParticleAuxContainer#{outFlags.GSFTrackParticles}"
            f"Aux.{outFlags.GSFTrackParticlesSuppESD}"]
        toESD += [
            f"TrackCollection#{outFlags.GSFTracks}"]

        toAOD += [
            f"xAOD::TrackParticleContainer#{outFlags.GSFTrackParticles}",
            f"xAOD::TrackParticleAuxContainer#{outFlags.GSFTrackParticles}"
            f"Aux.{outFlags.GSFTrackParticlesSuppAOD}"]

    if flags.Egamma.doConversionBuilding:
        toESD += [
            f"xAOD::VertexContainer#{outFlags.ConversionVertices}",
            f"xAOD::VertexAuxContainer#{outFlags.ConversionVertices}"
            f"Aux.{outFlags.ConversionVerticesSuppESD}"]

        toAOD += [
            f"xAOD::VertexContainer#{outFlags.ConversionVertices}",
            f"xAOD::VertexAuxContainer#{outFlags.ConversionVertices}"
            f"Aux.{outFlags.ConversionVerticesSuppAOD}"]

    if flags.Egamma.doTruthAssociation:
        toESD += [
            f"TruthParticleContainer#{outFlags.TruthParticles}",
            f"TruthParticleAuxContainer#{outFlags.TruthParticles}"
            f"Aux.{outFlags.TruthParticlesSuppESD}"]

        toAOD += [f"TruthParticleContainer#{outFlags.TruthParticles}",
                  f"TruthParticleAuxContainer#{outFlags.TruthParticles}"
                  f"Aux.{outFlags.TruthParticlesSuppAOD}"]

    if flags.InDet.doR3LargeD0:
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

    mlog.info('egammaESDList: %s \n', toESD)
    mlog.info('egammaAODList: %s \n', toAOD)

    from OutputStreamAthenaPool.OutputStreamConfig import (addToAOD, addToESD)
    acc.merge(addToESD(flags, toESD))
    acc.merge(addToAOD(flags, toAOD))

    return acc
