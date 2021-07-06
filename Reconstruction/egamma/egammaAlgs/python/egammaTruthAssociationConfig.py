# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCaloTruthMatchCfg
__doc__ = "Configure egammaTruthAssociation"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaTruthAssociationAlg = CompFactory.egammaTruthAssociationAlg


def egammaTruthAssociationCfg(flags, name='egammaTruthAssociation', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Start configuration')

    acc = ComponentAccumulator()

    if "MCTruthClassifier" not in kwargs:
        mctruth = MCTruthClassifierCaloTruthMatchCfg(flags)
        kwargs["MCTruthClassifier"] = mctruth.popPrivateTools()
        acc.merge(mctruth)

    kwargs.setdefault(
        "ClusterContainerName",
        flags.Egamma.Keys.Output.CaloClusters)
    kwargs.setdefault(
        "ElectronContainerName",
        flags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault(
        "PhotonContainerName",
        flags.Egamma.Keys.Output.Photons)
    kwargs.setdefault(
        "FwdElectronContainerName",
        flags.Egamma.Keys.Output.ForwardElectrons)
    kwargs.setdefault(
        "TruthEventContainerName",
        flags.Egamma.Keys.Input.TruthEvents)
    kwargs.setdefault(
        "TruthParticleContainerName",
        flags.Egamma.Keys.Input.TruthParticles)
    kwargs.setdefault(
        "EgammaTruthContainerName",
        flags.Egamma.Keys.Output.TruthParticles)
    kwargs.setdefault(
        "MatchForwardElectrons",
        flags.Egamma.doForwardSeeded)
    kwargs.setdefault("SimBarcodeOffset",
                      flags.Sim.SimBarcodeOffset)

    egtruthAlg = egammaTruthAssociationAlg(name, **kwargs)

    acc.addEventAlgo(egtruthAlg)
    return acc
