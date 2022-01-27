# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure the electron and photon selectors."

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID


def EMPIDBuilderElectronCfg(flags, name='EMPIDBuilderElectron', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    # Electron Selectors
    # Cut based
    from ROOT import LikeEnum
    from ElectronPhotonSelectorTools.AsgElectronIsEMSelectorsConfig import (
        AsgElectronIsEMSelectorCfg)
    if "electronIsEMselectors" not in kwargs:
        LooseElectronSelectorAcc = AsgElectronIsEMSelectorCfg(
            flags, "LooseElectronSelector", egammaPID.ElectronIDLoosePP)
        MediumElectronSelectorAcc = AsgElectronIsEMSelectorCfg(
            flags, "MediumElectronSelector", egammaPID.ElectronIDMediumPP)
        TightElectronSelectorAcc = AsgElectronIsEMSelectorCfg(
            flags, "TightElectronSelector", egammaPID.ElectronIDTightPP)

        kwargs["electronIsEMselectors"] = [
            LooseElectronSelectorAcc.popPrivateTools(),
            MediumElectronSelectorAcc.popPrivateTools(),
            TightElectronSelectorAcc.popPrivateTools()]
        kwargs["electronIsEMselectorResultNames"] = [
            "Loose", "Medium", "Tight"]
        acc.merge(LooseElectronSelectorAcc)
        acc.merge(MediumElectronSelectorAcc)
        acc.merge(TightElectronSelectorAcc)

    # Likelihood
    from ElectronPhotonSelectorTools.AsgElectronLikelihoodToolsConfig import (
        AsgElectronLikelihoodToolCfg)
    if "electronLHselectors" not in kwargs:
        LooseLHSelectorAcc = AsgElectronLikelihoodToolCfg(
            flags, "LooseLHSelector", LikeEnum.Loose)
        LooseLHSelectorAcc.primaryVertexContainer = "PrimaryVertices"
        MediumLHSelectorAcc = AsgElectronLikelihoodToolCfg(
            flags, "MediumLHSelector", LikeEnum.Medium)
        MediumLHSelectorAcc.primaryVertexContainer = "PrimaryVertices"
        TightLHSelectorAcc = AsgElectronLikelihoodToolCfg(
            flags, "TightLHSelector", LikeEnum.Tight)
        TightLHSelectorAcc.primaryVertexContainer = "PrimaryVertices"
        kwargs["electronLHselectors"] = [LooseLHSelectorAcc.popPrivateTools(),
                                         MediumLHSelectorAcc.popPrivateTools(),
                                         TightLHSelectorAcc.popPrivateTools()]
        kwargs["electronLHselectorResultNames"] = [
            "LHLoose", "LHMedium", "LHTight"]
        acc.merge(LooseLHSelectorAcc)
        acc.merge(MediumLHSelectorAcc)
        acc.merge(TightLHSelectorAcc)

    tool = CompFactory.EMPIDBuilder(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


def EMPIDBuilderPhotonCfg(flags, name='EMPIDBuilderPhoton', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    # photon Selectors
    from ElectronPhotonSelectorTools.AsgPhotonIsEMSelectorsConfig import (
        AsgPhotonIsEMSelectorCfg)
    LoosePhotonSelectorAcc = AsgPhotonIsEMSelectorCfg(
        flags, "LoosePhotonSelector", egammaPID.PhotonIDLoose)
    TightPhotonSelectorAcc = AsgPhotonIsEMSelectorCfg(
        flags, "TightPhotonSelector", egammaPID.PhotonIDTight)

    if "photonIsEMselectors" not in kwargs:
        kwargs["photonIsEMselectors"] = [
            LoosePhotonSelectorAcc.popPrivateTools(),
            TightPhotonSelectorAcc.popPrivateTools()]
        kwargs["photonIsEMselectorResultNames"] = ["Loose", "Tight"]

        acc.merge(LoosePhotonSelectorAcc)
        acc.merge(TightPhotonSelectorAcc)

    tool = CompFactory.EMPIDBuilder(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("EMPIDBuilderConfigTest")
    mlog.info("Configuring  EMPIDBuilderElectron: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMPIDBuilderElectronCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    mlog.info("Configuring  EMPIDBuilderPhoton: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMPIDBuilderPhotonCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("empidbuilder.pkl", "wb")
    cfg.store(f)
    f.close()
