# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool and service factories to instantiate MVA calibration for trigger"
__author__ = "Jovan Mitrevski"


from egammaRec.Factories import ToolFactory, ServiceFactory

import logging
mlog = logging.getLogger ('TrigEgammaMVACalibFactories')

from egammaMVACalib import egammaMVACalibConf
from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
mlog.info("MVA version version %s", EgammaSliceFlags.calibMVAVersion() )
mlog.info("Cluster Correction version %s", EgammaSliceFlags.clusterCorrectionVersion() )
EgammaSliceFlags.calibMVAVersion.set_On()

from xAODEgamma.xAODEgammaParameters import xAOD


TrigElectronMVATool = ToolFactory(egammaMVACalibConf.egammaMVACalibTool,
                                  name = "TrigElectronMVATool",
                                  ParticleType = xAOD.EgammaParameters.electron,
                                  folder=EgammaSliceFlags.calibMVAVersion(),
                                  use_layer_corrected = False)

TrigPhotonMVATool = ToolFactory(egammaMVACalibConf.egammaMVACalibTool,
                                name = "TrigPhotonMVATool",
                                ParticleType = xAOD.EgammaParameters.unconvertedPhoton,
                                folder=EgammaSliceFlags.calibMVAVersion(),
                                use_layer_corrected = False)


TrigEgammaMVASvc =  ServiceFactory(egammaMVACalibConf.egammaMVASvc, name = "TrigEgammaMVASvc",
                                   ElectronTool = TrigElectronMVATool,
                                   UnconvertedPhotonTool = TrigPhotonMVATool)

del mlog
