# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaMVACalibTool, egammaMVASvc=CompFactory.getComps("egammaMVACalibTool","egammaMVASvc",)
from xAODEgamma.xAODEgammaParameters import xAOD


def egammaMVASvcCfg(flags, name="egammaMVASvc"):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if flags.Egamma.Calib.MVAVersion is not None:
        folder = flags.Egamma.Calib.MVAVersion
        mlog.debug('MVA version: %s', folder)
    else:
        raise KeyError("Egamma.Calib.MVAVersion is not set")

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

    f = open("egmvatools.pkl", "wb")
    cfg.store(f)
    f.close()
