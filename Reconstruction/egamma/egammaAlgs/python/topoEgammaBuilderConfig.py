# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the two supercluster
          builders with default configuration
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
topoEgammaBuilder = CompFactory.topoEgammaBuilder
EGammaAmbiguityTool = CompFactory.EGammaAmbiguityTool


def topoEgammaBuilderCfg(flags, name='topoEgammaBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault(
        "SuperElectronRecCollectionName",
        flags.Egamma.Keys.Internal.ElectronSuperRecs)
    kwargs.setdefault(
        "SuperPhotonRecCollectionName",
        flags.Egamma.Keys.Internal.PhotonSuperRecs)
    kwargs.setdefault(
        "ElectronOutputName",
        flags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault(
        "PhotonOutputName",
        flags.Egamma.Keys.Output.Photons)
    kwargs.setdefault(
        "AmbiguityTool",
        EGammaAmbiguityTool())

    topoegAlg = topoEgammaBuilder(flags, **kwargs)

    acc.addEventAlgo(topoegAlg)
    return acc
