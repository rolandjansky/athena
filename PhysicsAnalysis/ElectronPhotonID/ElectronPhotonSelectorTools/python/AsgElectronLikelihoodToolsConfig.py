# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "    Configure the AsgElectronLikelihoodTool with the quality cuts and allow for (re-)setting of all provided cuts."

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Import the needed stuff specific to the ElectronPhotonSelectorTools
AsgElectronLikelihoodTool=CompFactory.AsgElectronLikelihoodTool
from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import ElectronLikelihoodMap, electronLHmenu


def AsgElectronLikelihoodToolCfg(flag, name, quality, menu=electronLHmenu.offlineMC16):

    acc = ComponentAccumulator()

    mlog = logging.getLogger('AsgElectronLikelihoodTool')
    mlog.debug('Start configuration')

    try:
        ntuple = ElectronLikelihoodMap(quality, menu)
        mlog.debug('ntuple: %s', ntuple)
    except KeyError:
        mlog.error("ElectronLH quality not found. Please use an egammaIDQuality (ElectronPhotonSelectorTools/egammaPIDdefs.h). This function only supports standard electronLH IDs, and not standard electron IDs or photon or forward IDs")
        raise

    # Create an instance of the tool
    tool = AsgElectronLikelihoodTool(name)

    # Call the function and configure it with the standard configuration
    ntuple[1](tool)

    acc.setPrivateTools(tool)
    return acc
