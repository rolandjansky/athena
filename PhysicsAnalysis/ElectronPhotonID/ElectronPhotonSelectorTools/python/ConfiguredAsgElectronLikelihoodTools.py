# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredAsgElectronIsEMSelectors
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC), Kurt Brendlinger (Penn)
## Created:     August 2013
##
## Description: Apply the default configurations for the AsgElectronIsEMSelector,
##              but allow for overwriting them with user-defined values.
##
##=============================================================================

# Import the needed general stuff
from PATCore.HelperUtils import *
from AthenaCommon import CfgMgr
import sys

# Import the needed stuff specific to the ElectronPhotonSelectorTools
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronLikelihoodTool
from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import ElectronLikelihoodMap, electronLHmenu

def ConfiguredAsgElectronLikelihoodTool( name, quality, menu=electronLHmenu.offlineMC16, **kw ):
    """
    Configure the AsgElectronIsEMSelector with the quality cuts
    and allow for (re-)setting of all provided cuts.
    """
    try:
        ntuple = ElectronLikelihoodMap(quality, menu)
    except KeyError:
        sys.stderr.write("Electron quality not found. Please use an egammaIDQuality (ElectronPhotonSelectorTools/egammaPIDdefs.h).\n This function only supports standard electron IDs, and not photon or forward IDs\n")
        raise

    # Get the label for user data
    tmpName = (ntuple[1]).func_name
    labelName = "isLH" + ((tmpName.split("Config")[0]).split("Likelihood")[1])

    # Create an instance of the tool
    tool = CfgMgr.AsgElectronLikelihoodTool(name, **kw)
    ntuple[1](tool)

    # Get all provided properties and overwrite the default values with them
    SetToolProperties( tool, **kw )

    #print tool
    return tool




