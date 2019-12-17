# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure the AsgPhotonIsEMSelector with the quality cuts and allow for (re-)setting of all provided cuts."

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Import the needed stuff specific to the PhotonPhotonSelectorTools
AsgPhotonIsEMSelector=CompFactory.AsgPhotonIsEMSelector
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import PhotonIsEMMap, photonPIDmenu


def AsgPhotonIsEMSelectorCfg(flags, name, quality, menu=photonPIDmenu.menuDC14):

    acc = ComponentAccumulator()

    mlog = logging.getLogger('AsgPhotonIsEMSelector')
    mlog.debug('Start configuration')

    try:
        ntuple = PhotonIsEMMap(quality, menu)
        mlog.debug('ntuple: %s', ntuple)
    except KeyError:
        mlog.error("Photon quality not found. Please use an egammaIDQuality (ElectronPhotonSelectorTools/egammaPIDdefs.h). This function only supports standard photon IDs, and not electron IDs or forward IDs")
        raise

    # Create and instance of the tool
    tool = AsgPhotonIsEMSelector(name)

    # # Call the function and configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    mlog.debug('isEMMask: %x', tool.isEMMask)

    acc.setPrivateTools(tool)
    return acc
