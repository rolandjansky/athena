# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Prepare EGamma output list"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaOutputCfg(flags, name="EGOutputList"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma Output configuration')

    acc = ComponentAccumulator()
    outFlags = flags.Egamma.Keys.Output

    toESD = []
    toAOD = []

    if flags.Egamma.doCentral:
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

    if flags.Egamma.doForward:
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

    if flags.Egamma.doTracking:
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

    if flags.Output.doWriteESD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToESD
        acc.merge(addToESD(flags, toESD))
        mlog.info('egammaESDList: %s ', toESD)

    if flags.Output.doWriteAOD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToAOD
        acc.merge(addToAOD(flags, toAOD))
        mlog.info('egammaAODList: %s ', toAOD)

    mlog.info("EGamma Output configured")
    return acc
