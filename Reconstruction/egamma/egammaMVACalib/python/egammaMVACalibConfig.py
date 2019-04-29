# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaMVACalib.egammaMVACalibConf import egammaMVACalibTool, egammaMVASvc
from ROOT import xAOD
import cppyy
cppyy.loadDictionary('xAODEgammaDict')


def egammaMVASvcCfg(flags, name="egammaMVASvc"):

    acc = ComponentAccumulator()

    mlog = logging.getLogger('egammaMVACalibConfig')

    if flags.Egamma.Calib.MVAVersion is not None:
        folder = flags.Egamma.Calib.MVAVersion
        mlog.debug('MVA version: %s', folder)
    else:
        mlog.error("Egamma.Calib.MVAVersion is not set")

    electronMVATool = egammaMVACalibTool(name="electronMVATool",
                                         ParticleType=xAOD.EgammaParameters.electron,
                                         folder=folder)
    unconvertedPhotonMVATool = egammaMVACalibTool(name="unconvertedPhotonMVATool",
                                                  ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
                                                  folder=folder)
    convertedPhotonMVATool = egammaMVACalibTool(name="convertedPhotonMVATool",
                                                ParticleType=xAOD.EgammaParameters.convertedPhoton,
                                                folder=folder)

    egMVASvc = egammaMVASvc(name=name,
                            ElectronTool=electronMVATool,
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
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/myRDO.pool.root"]
    ConfigFlags.lock()

    cfg = ComponentAccumulator()
    cfg.printConfig()
    acc = egammaMVASvcCfg(ConfigFlags)
    cfg.merge(acc)

    f = open("egmvatools.pkl", "w")
    cfg.store(f)
    f.close()
