# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the EMPIDBuilder
from ROOT import LikeEnum
from ROOT import egammaPID
from AthenaCommon.Logging import logging
import traceback

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu

# import base class
from egammaTools.egammaToolsConf import EMPIDBuilder

import cppyy
cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')


class EMPIDBuilderElectronBase (EMPIDBuilder):
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderElectronBase"):
        EMPIDBuilder.__init__(self, name)
        mlog = logging.getLogger(name+'::__init__')
        mlog.debug("entering")

       # Electron Selectors
        try:
            # Cut based
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
            LooseElectronSelector = ConfiguredAsgElectronIsEMSelector(
                "LooseElectronSelector", egammaPID.ElectronIDLoosePP)
            MediumElectronSelector = ConfiguredAsgElectronIsEMSelector(
                "MediumElectronSelector", egammaPID.ElectronIDMediumPP)
            TightElectronSelector = ConfiguredAsgElectronIsEMSelector(
                "TightElectronSelector", egammaPID.ElectronIDTightPP)

            # Likelihood
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
            LooseLHSelector = ConfiguredAsgElectronLikelihoodTool(
                "LooseLHSelector", LikeEnum.Loose)
            LooseLHSelector.primaryVertexContainer = "PrimaryVertices"
            MediumLHSelector = ConfiguredAsgElectronLikelihoodTool(
                "MediumLHSelector", LikeEnum.Medium)
            MediumLHSelector.primaryVertexContainer = "PrimaryVertices"
            TightLHSelector = ConfiguredAsgElectronLikelihoodTool(
                "TightLHSelector", LikeEnum.Tight)
            TightLHSelector.primaryVertexContainer = "PrimaryVertices"

            # Multi Lepton
            from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronMultiLeptonSelector
            MultiLeptonSelector = AsgElectronMultiLeptonSelector(
                "MultiLeptonSelector")

        except:
            mlog.error("could not get configure tools")
            traceback.print_exc()
            return False

        self.electronIsEMselectors = [
            LooseElectronSelector, MediumElectronSelector, TightElectronSelector]
        self.electronIsEMselectorResultNames = ["Loose", "Medium", "Tight"]
        self.electronLHselectors = [
            LooseLHSelector, MediumLHSelector, TightLHSelector]
        self.electronLHselectorResultNames = ["LHLoose", "LHMedium", "LHTight"]
        self.genericIsEMselectors = [MultiLeptonSelector]
        self.genericIsEMselectorResultNames = ["MultiLepton"]
        self.LuminosityTool = None


class EMPIDBuilderPhotonBase (EMPIDBuilder):
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderPhotonBase"):
        EMPIDBuilder.__init__(self, name)
        mlog = logging.getLogger(name+'::__init__')
        mlog.debug("entering")

        # photon Selectors
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
            LoosePhotonSelector = ConfiguredAsgPhotonIsEMSelector(
                "LoosePhotonSelector", egammaPID.PhotonIDLoose)
            TightPhotonSelector = ConfiguredAsgPhotonIsEMSelector(
                "TightPhotonSelector", egammaPID.PhotonIDTight)

        except:
            mlog.error("could not get configure tools")
            traceback.print_exc()
            return False

        self.photonIsEMselectors = [LoosePhotonSelector, TightPhotonSelector]
        self.photonIsEMselectorResultNames = ["Loose", "Tight"]
        self.LuminosityTool = None
