# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaMVACalib.egammaMVACalibConf import egammaMVACalibTool, egammaMVASvc
from ROOT import xAOD
import cppyy
cppyy.loadDictionary('xAODEgammaDict')


def egammaMVASvcCfg(flags):

    acc = ComponentAccumulator()

    folder = flags.Egamma.Calib.MVAVersion

    electronMVATool = egammaMVACalibTool(name="electronMVATool",
                                         ParticleType=xAOD.EgammaParameters.electron,
                                         folder=folder)
    unconvertedPhotonMVATool = egammaMVACalibTool(name="unconvertedPhotonMVATool",
                                                  ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
                                                  folder=folder)
    convertedPhotonMVATool = egammaMVACalibTool(name="convertedPhotonMVATool",
                                                ParticleType=xAOD.EgammaParameters.convertedPhoton,
                                                folder=folder)

    egMVASvc = egammaMVASvc(ElectronTool=electronMVATool,
                            UnconvertedPhotonTool=unconvertedPhotonMVATool,
                            ConvertedPhotonTool=convertedPhotonMVATool)

    acc.addService(egMVASvc)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = ["valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r6699_10evt.pool.root"]
    ConfigFlags.lock()

    cfg = ComponentAccumulator()
    cfg.printConfig()
    acc = egammaMVASvcCfg(ConfigFlags)
    cfg.merge(acc)

    f = open("egmvatools.pkl", "w")
    cfg.store(f)
    f.close()
