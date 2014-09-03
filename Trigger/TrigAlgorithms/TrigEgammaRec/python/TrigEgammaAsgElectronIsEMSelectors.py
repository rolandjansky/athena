# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        TrigEgammaAsgElectronIsEMSelectors
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC), Ryan White (UTFSM)
## Created:     Dec. 2011
## Modified:    June 2014
##
## Description: Apply the default configurations for the AthElectronIsEMSelector,
##              but allow for overwriting them with user-defined values.
##
##=============================================================================

# Import the needed general stuff
from PATCore.HelperUtils import *
from AthenaCommon import CfgMgr
import sys

# Import the needed stuff specific to the ElectronPhotonSelectorTools
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronIsEMSelector
from TrigEgammaHypo.TrigEgammaElectronIsEMSelectorMapping import TrigEgammaElectronIsEMMap, electronPIDmenu

def TrigEgammaAsgElectronIsEMSelector( name, quality, menu=electronPIDmenu.menuTrigDC14, **kw ):
    """
    Configure the AthElectronIsEMSelector with the quality cuts
    and allow for (re-)setting of all provided cuts.
    """
    try:
        ntuple = TrigEgammaElectronIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write("Electron quality not found. Please use an egammaIDQuality (egammaEvent/egammaPIDdefs.h).\n This function only supports standard electron IDs, and not photon or forward IDs\n")
        raise

    # Get the label for user data
    tmpName = (ntuple[1]).func_name
    labelName = "is" + ((tmpName.split("Selector")[0]).split("IsEM")[1])

    # Create and instance of the tool
    tool = CfgMgr.AsgElectronIsEMSelector(name, **kw)

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]

    # Get all provided properties and overwrite the default values with them
    SetToolProperties( tool, **kw )

    #print tool
    return tool




