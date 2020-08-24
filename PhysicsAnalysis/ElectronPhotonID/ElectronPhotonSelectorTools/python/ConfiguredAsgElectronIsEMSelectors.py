# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Name:        ConfiguredAsgElectronIsEMSelectors
  Author:      Karsten Koeneke(CERN), Jovan Mitrevski(UCSC)
  Created:     Dec. 2011
  Description: Apply the default configurations for the
  AsgElectronIsEMSelector,but allow for overwriting
  them with user-defined values."""

# Import the needed general stuff
from PATCore.HelperUtils import SetToolProperties
from AthenaCommon import CfgMgr
import sys


# Import the needed stuff specific to the ElectronPhotonSelectorTools
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import (
    ElectronIsEMMap, electronPIDmenu)


def ConfiguredAsgElectronIsEMSelector(
        name, quality, menu=electronPIDmenu.menuDC14, **kw):
    """
    Configure the AsgElectronIsEMSelector with the quality cuts
    and allow for (re-)setting of all provided cuts.
    """
    try:
        ntuple = ElectronIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write("Electron quality not found."
                         "Please use an egammaIDQuality"
                         "(ElectronPhotonSelectorTools/egammaPIDdefs.h).\n "
                         "This function only supports standard electron IDs, "
                         "and not photon or forward IDs\n")
        raise

    # Create and instance of the tool
    tool = CfgMgr.AsgElectronIsEMSelector(name, **kw)

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    # Get all provided properties and overwrite the default values with them
    SetToolProperties(tool, **kw)

    # print tool
    return tool
