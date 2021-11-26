# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool and service factories to instantiate MVA calibration"""
__author__ = "Jovan Mitrevski"


from egammaRec.Factories import ToolFactory, ServiceFactory

from egammaMVACalib import egammaMVACalibConf
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from xAODEgamma.xAODEgammaParameters import xAOD

electronMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="electronMVATool",
    ParticleType=xAOD.EgammaParameters.electron,
    folder=ConfigFlags.Egamma.Calib.MVAVersion)

unconvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="unconvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
    folder=ConfigFlags.Egamma.Calib.MVAVersion)

convertedPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="convertePhotonMVATool",
    ParticleType=xAOD.EgammaParameters.convertedPhoton,
    folder=ConfigFlags.Egamma.Calib.MVAVersion)

egammaMVASvc = ServiceFactory(
    egammaMVACalibConf.egammaMVASvc,
    name="egammaMVASvc_LegacyConfig",
    ElectronTool=electronMVATool,
    UnconvertedPhotonTool=unconvPhotonMVATool,
    ConvertedPhotonTool=convertedPhotonMVATool)
