# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from MCTruthClassifier.MCTruthClassifierConfig import (
    MCTruthClassifierCaloTruthMatchCfg)
__doc__ = "Configure egammaTruthAssociation"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaTruthAssociationCfg(flags, name='egammaTruthAssociation',
                              sequenceName = None,
                              **kwargs):

    seqkw = {'sequence': sequenceName} if sequenceName else {}
    acc = ComponentAccumulator (**seqkw)

    if "MCTruthClassifier" not in kwargs:
        kwargs["MCTruthClassifier"] = acc.popToolsAndMerge(
            MCTruthClassifierCaloTruthMatchCfg(flags))

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
        flags.Egamma.doForward)
    kwargs.setdefault("SimBarcodeOffset",
                      flags.Sim.SimBarcodeOffset)

    egtruthAlg = CompFactory.egammaTruthAssociationAlg(name, **kwargs)

    acc.addEventAlgo(egtruthAlg)
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
    mlog = logging.getLogger("egammaTruthAssociationConfigTest")
    mlog.info("Configuring  egammaTruthAssociation: ")
    acc.merge(egammaTruthAssociationCfg(flags))
    printProperties(mlog,
                    acc.getEventAlgo("egammaTruthAssociation"),
                    nestLevel=1,
                    printDefaults=True)
    with open("egammatruthassociation.pkl", "wb") as f:
        acc.store(f)
