# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaMVACalibTool, egammaMVASvc=CompFactory.getComps("egammaMVACalibTool","egammaMVASvc",)
from xAODEgamma.xAODEgammaParameters import xAOD


def TrigEgammaMVACalibCfg(flags, name="TrigEgammaMVASvc"):

    mlog = logging.getLogger('TrigEgammaMVACalibConfig')
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if flags.Trigger.egamma.calibMVAVersion is not None:
        folder = flags.Trigger.egamma.calibMVAVersion
        mlog.debug('MVA version: %s', folder)
    else:
        raise KeyError("Trigger.egamma.calibMVAVersion not set")

    mlog.debug('Cluster Correction version: %s', flags.Trigger.egamma.clusterCorrectionVersion)

    TrigElectronMVATool = egammaMVACalibTool(name="TrigElectronMVATool",
                                             ParticleType=xAOD.EgammaParameters.electron,
                                             folder=folder,
                                             use_layer_corrected=False)

    TrigPhotonMVATool = egammaMVACalibTool(name="TrigPhotonMVATool",
                                           ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
                                           folder=folder,
                                           use_layer_corrected=False)

    TrigEgammaMVASvc = egammaMVASvc(name=name,
                                    ElectronTool=TrigElectronMVATool,
                                    UnconvertedPhotonTool=TrigPhotonMVATool)

    acc.addService(TrigEgammaMVASvc)
    return acc
