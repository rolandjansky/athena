# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool and service factories to instantiate MVA calibration"""

from egammaRec.Factories import ToolFactory, ServiceFactory

from egammaMVACalib import egammaMVACalibConf
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from xAODEgamma.xAODEgammaParameters import xAOD

# Calibration service for precision calo step
trigPrecCaloElectronMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecCaloElectronMVATool",
    ParticleType=xAOD.EgammaParameters.electron,
    folder=ConfigFlags.Trigger.egamma.Calib.precCaloMVAVersion)
trigPrecCaloUnconvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecCaloUnconvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
    folder=ConfigFlags.Trigger.egamma.Calib.precCaloMVAVersion)
trigPrecCaloConvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecCaloConvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.convertedPhoton,
    folder=ConfigFlags.Trigger.egamma.Calib.precCaloMVAVersion)
trigPrecCaloEgammaMVASvc = ServiceFactory(
    egammaMVACalibConf.egammaMVASvc,
    name = "trigPrecCaloEgammaMVASvc",
    ElectronTool=trigPrecCaloElectronMVATool,
    ConvertedPhotonTool=trigPrecCaloConvPhotonMVATool,
    UnconvertedPhotonTool=trigPrecCaloUnconvPhotonMVATool)

# Calibration service for precision electron and photon step
trigPrecElectronMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecElectronMVATool",
    ParticleType=xAOD.EgammaParameters.electron,
    folder=ConfigFlags.Trigger.egamma.Calib.precEgammaMVAVersion)

trigPrecUnconvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecUnconvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
    folder=ConfigFlags.Trigger.egamma.Calib.precEgammaMVAVersion)

trigPrecConvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="trigPrecConvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.convertedPhoton,
    folder=ConfigFlags.Trigger.egamma.Calib.precEgammaMVAVersion)

trigPrecEgammaMVASvc = ServiceFactory(
    egammaMVACalibConf.egammaMVASvc,
    name="trigPrecEgammaMVASvc",
    ElectronTool=trigPrecElectronMVATool,
    UnconvertedPhotonTool=trigPrecUnconvPhotonMVATool,
    ConvertedPhotonTool=trigPrecConvPhotonMVATool)
