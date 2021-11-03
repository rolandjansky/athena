# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


__doc__ = """Configure the AsgElectronIsEMSelector with the quality cuts
         and allow for (re-)setting of all provided cuts."""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import (
    ElectronIsEMMap, electronPIDmenu)


def AsgElectronIsEMSelectorCfg(
        flags,
        name,
        quality,
        menu=electronPIDmenu.menuDC14):

    mlog = logging.getLogger('AsgElectronIsEMSelector')
    mlog.debug('Start configuration')

    AsgElectronIsEMSelector = CompFactory.AsgElectronIsEMSelector
    acc = ComponentAccumulator()
    try:
        ntuple = ElectronIsEMMap(quality, menu)
        mlog.debug('ntuple: %s', ntuple)
    except KeyError:
        mlog.error("Electron quality not found. Please use an egammaIDQuality"
                   "(ElectronPhotonSelectorTools/egammaPIDdefs.h)."
                   "This function only supports standard electron IDs,"
                   "and not photon or forward IDs")
        raise

    # Create and instance of the tool
    tool = AsgElectronIsEMSelector(name)

    # Call the function and configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    mlog.debug('isEMMask: %x', tool.isEMMask)

    acc.setPrivateTools(tool)
    return acc
