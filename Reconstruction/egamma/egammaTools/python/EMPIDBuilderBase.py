# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# default configuration of the EMPIDBuilder
from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID
from AthenaCommon.Logging import logging
# import base class
from egammaTools.egammaToolsConf import EMPIDBuilder


class EMPIDBuilderPhotonBase (EMPIDBuilder):
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderPhotonBase"):
        EMPIDBuilder.__init__(self, name)
        mlog = logging.getLogger(name+'::__init__')
        mlog.debug("entering")

        # photon Selectors

        from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors \
            import ConfiguredAsgPhotonIsEMSelector
        LoosePhotonSelector = ConfiguredAsgPhotonIsEMSelector(
            "LoosePhotonSelector", egammaPID.PhotonIDLoose)
        TightPhotonSelector = ConfiguredAsgPhotonIsEMSelector(
            "TightPhotonSelector", egammaPID.PhotonIDTight)

        self.photonIsEMselectors = [LoosePhotonSelector, TightPhotonSelector]
        self.photonIsEMselectorResultNames = ["Loose", "Tight"]
        self.LuminosityTool = None
