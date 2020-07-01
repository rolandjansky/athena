# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """
Name:        ConfiguredAsgPhotonIsEMSelectors

Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
Created:     Dec. 2011

Description: Apply the default configurations for the AsgPhotonIsEMSelector,
             but allow for overwriting them with user-defined values.
"""

# Import the needed general stuff
from PATCore.HelperUtils import SetToolProperties
from AthenaCommon import CfgMgr
import sys

# Import the needed stuff specific to the PhotonPhotonSelectorTools
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import (
    PhotonIsEMMap, photonPIDmenu)


def ConfiguredAsgPhotonIsEMSelector(
        name, quality, menu=photonPIDmenu.menuCurrentCuts, **kw):
    """
    Configure the AsgPhotonIsEMSelector with the quality cuts
    and allow for (re-)setting of all provided cuts.
    """
    try:
        ntuple = PhotonIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write(
            " Photon quality not found."
            " Please use an egammaIDQuality"
            " (ElectronPhotonSelectorTools/egammaPIDdefs.h).\n"
            " This function only supports standard photon IDs,"
            " and not photon or forward IDs\n")
        raise

    # Create and instance of the tool
    tool = CfgMgr.AsgPhotonIsEMSelector(name, **kw)

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    # Get all provided properties and overwrite the default values with them
    SetToolProperties(tool, **kw)

    # print tool
    return tool
