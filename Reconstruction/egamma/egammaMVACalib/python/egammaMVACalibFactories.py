# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool and service factories to instantiate MVA calibration"""
__author__ = "Jovan Mitrevski"


from egammaRec.Factories import ToolFactory, ServiceFactory

from egammaMVACalib import egammaMVACalibConf
from egammaRec.egammaRecFlags import jobproperties
from xAODEgamma.xAODEgammaParameters import xAOD

electronMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="electronMVATool",
    ParticleType=xAOD.EgammaParameters.electron,
    folder=jobproperties.egammaRecFlags.calibMVAVersion())

unconvPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="unconvPhotonMVATool",
    ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
    folder=jobproperties.egammaRecFlags.calibMVAVersion())

convertedPhotonMVATool = ToolFactory(
    egammaMVACalibConf.egammaMVACalibTool,
    name="convertePhotonMVATool",
    ParticleType=xAOD.EgammaParameters.convertedPhoton,
    folder=jobproperties.egammaRecFlags.calibMVAVersion())

egammaMVASvc = ServiceFactory(
    egammaMVACalibConf.egammaMVASvc,
    ElectronTool=electronMVATool,
    UnconvertedPhotonTool=unconvPhotonMVATool,
    ConvertedPhotonTool=convertedPhotonMVATool)
