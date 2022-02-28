# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from xAODEgamma.xAODEgammaParameters import xAOD


def egammaMVAToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.egammaMVACalibTool(**kwargs))
    return acc


def egammaMVASvcCfg(flags, name="egammaMVASvc", **kwargs):

    mlog = logging.getLogger(name)
    acc = ComponentAccumulator()

    if flags.Egamma.Calib.MVAVersion is not None:
        folder = flags.Egamma.Calib.MVAVersion
        mlog.info('egamma MVA calib version: %s', folder)
    else:
        raise KeyError("Egamma.Calib.MVAVersion is not set")

    if "ElectronTool" not in kwargs:
        kwargs["ElectronTool"] = acc.popToolsAndMerge(
            egammaMVAToolCfg(
                flags,
                name="electronMVATool",
                ParticleType=xAOD.EgammaParameters.electron,
                folder=folder)
        )

    if "UnconvertedPhotonTool" not in kwargs:
        kwargs["UnconvertedPhotonTool"] = acc.popToolsAndMerge(
            egammaMVAToolCfg(
                flags,
                name="unconvertedPhotonMVATool",
                ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
                folder=folder)
        )

    if "ConvertedPhotonTool" not in kwargs:
        kwargs["ConvertedPhotonTool"] = acc.popToolsAndMerge(
            egammaMVAToolCfg(
                flags,
                name="convertedPhotonMVATool",
                ParticleType=xAOD.EgammaParameters.convertedPhoton,
                folder=folder)
        )

    acc.addService(
        CompFactory.egammaMVASvc(
            name=name,
            **kwargs), primary=True)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("egammaMVASvcConfigTest")
    mlog.info("Configuring egammaMVASvc :")
    printProperties(mlog, cfg.getPrimaryAndMerge(
        egammaMVASvcCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    cfg.printConfig()

    f = open("egmvatools.pkl", "wb")
    cfg.store(f)
    f.close()
