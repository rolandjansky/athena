# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configure the AsgPhotonIsEMSelector with the quality cuts
              and allow for (re-)setting of all provided cuts."""

from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import (
    PhotonIsEMMap, photonPIDmenu)
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def AsgPhotonIsEMSelectorCfg(flags,
                             name,
                             quality,
                             menu=photonPIDmenu.menuCurrentCuts):

    mlog = logging.getLogger('AsgPhotonIsEMSelector')
    mlog.debug('Start configuration')
    acc = ComponentAccumulator()
    AsgPhotonIsEMSelector = CompFactory.AsgPhotonIsEMSelector

    try:
        ntuple = PhotonIsEMMap(quality, menu)
        mlog.debug('ntuple: %s', ntuple)
    except KeyError:
        mlog.error("Photon quality not found."
                   "Please use an egammaIDQuality"
                   "(ElectronPhotonSelectorTools/egammaPIDdefs.h)."
                   "This function only supports standard photon IDs,"
                   "and not electron IDs or forward IDs")
        raise

    # Create and instance of the tool
    tool = AsgPhotonIsEMSelector(name)

    # # Call the function and configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    mlog.debug('isEMMask: %x', tool.isEMMask)

    acc.setPrivateTools(tool)
    return acc
