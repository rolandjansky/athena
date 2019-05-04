# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaMVACalib.egammaMVACalibConf import egammaMVACalibTool, egammaMVASvc
from ROOT import xAOD
import cppyy
cppyy.loadDictionary('xAODEgammaDict')


def TrigEgammaMVACalibCfg(flags, name="TrigEgammaMVASvc"):

    acc = ComponentAccumulator()

    mlog = logging.getLogger('TrigEgammaMVACalibConfig')

    if flags.Trigger.egamma.calibMVAVersion is not None:
        folder = flags.Trigger.egamma.calibMVAVersion
        mlog.debug('MVA version: %s', folder)
    else:
        mlog.error("Trigger.egamma.calibMVAVersion not set")

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
