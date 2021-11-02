# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


__doc__ = """Configure the AsgForwardElectronIsEMSelector with the quality cuts
            and allow for (re-)setting of all provided cuts."""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ElectronPhotonSelectorTools.ForwardElectronIsEMSelectorMapping import (
    ForwardElectronIsEMMap, forwardelectronPIDmenu)


def AsgForwardElectronIsEMSelectorCfg(flags,
                                      name,
                                      quality,
                                      menu=forwardelectronPIDmenu.menuMC15):

    mlog = logging.getLogger('AsgForwardElectronIsEMSelector')
    mlog.debug('Start configuration')
    acc = ComponentAccumulator()
    AsgForwardElectronIsEMSelector = CompFactory.AsgForwardElectronIsEMSelector

    try:
        ntuple = ForwardElectronIsEMMap(quality, menu)
        mlog.debug('ntuple: %s', ntuple)
    except KeyError:
        mlog.error("Fwd Electron quality not found."
                   "Please use an egammaIDQuality"
                   "(ElectronPhotonSelectorTools/egammaPIDdefs.h)."
                   "This function only supports forward IDs,"
                   "and not photon or standard electron IDs")
        raise

    # Create and instance of the tool
    tool = AsgForwardElectronIsEMSelector(name)

    # Call the function and configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    mlog.debug('isEMMask: %x', tool.isEMMask)

    acc.setPrivateTools(tool)
    return acc
