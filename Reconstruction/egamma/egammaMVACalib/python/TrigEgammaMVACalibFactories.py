# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool and service factories to instantiate MVA calibration for trigger"
__author__ = "Jovan Mitrevski"


from egammaRec.Factories import ToolFactory, ServiceFactory

import logging
mlog = logging.getLogger ('TrigEgammaMVACalibFactories')

from egammaMVACalib import egammaMVACalibConf
from TriggerJobOpts.TriggerFlags import TriggerFlags
mlog.info("MVA version version %s", TriggerFlags.EgammaSlice.calibMVAVersion() )
mlog.info("Cluster Correction version %s", TriggerFlags.EgammaSlice.clusterCorrectionVersion() )
TriggerFlags.EgammaSlice.calibMVAVersion.set_On()

from xAODEgamma.xAODEgammaParameters import xAOD


TrigElectronMVATool = ToolFactory(egammaMVACalibConf.egammaMVACalibTool,
                                  name = "TrigElectronMVATool",
                                  ParticleType = xAOD.EgammaParameters.electron,
                                  folder=TriggerFlags.EgammaSlice.calibMVAVersion(),
                                  use_layer_corrected = False)

TrigPhotonMVATool = ToolFactory(egammaMVACalibConf.egammaMVACalibTool,
                                name = "TrigPhotonMVATool",
                                ParticleType = xAOD.EgammaParameters.unconvertedPhoton,
                                folder=TriggerFlags.EgammaSlice.calibMVAVersion(),
                                use_layer_corrected = False)


TrigEgammaMVASvc =  ServiceFactory(egammaMVACalibConf.egammaMVASvc, name = "TrigEgammaMVASvc",
                                   ElectronTool = TrigElectronMVATool,
                                   UnconvertedPhotonTool = TrigPhotonMVATool)

del mlog
