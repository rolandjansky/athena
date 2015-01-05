# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredTElectronIsEMSelectors
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
## Created:     Feb 2012
##
## Description: Apply the default configurations for the TElectronIsEMSelector.
##
## Usage: In your ROOT session (after loading the libraries from RootCore):
##        TPython::LoadMacro("./path/to/this/file/ConfiguredTElectronIsEMSelectors.py");
## 2011 H4l
##        Root::TElectronIsEMSelector* myTool = (void*)TPython::Eval("ConfiguredTElectronIsEMSelector(ROOT.egammaPID.ElectronIDLoosePP,electronPIDmenu.menuH4l2011)");
## 2012 MediumPP
##        Root::TElectronIsEMSelector* myTool = (void*)TPython::Eval("ConfiguredTElectronIsEMSelector(ROOT.egammaPID.ElectronIDMediumPP,electronPIDmenu.menu2011)");
##        myTool->initialize();
##
##=============================================================================

# import the needed Reflex and ROOT stuff
import ROOT

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import ElectronIsEMMap, electronPIDmenu

def ConfiguredTElectronIsEMSelector( quality, menu=electronPIDmenu.menuDC14 ):
    """
    Configure the TElectronIsEMSelector with the cuts of the given quality
    """    
    tool = ROOT.Root.TElectronIsEMSelector()
    try:
        ntuple = ElectronIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write("Electron quality not found. Please use an egammaIDQuality (egammaEvent/egammaPIDdefs.h).\n This function only supports standard electron IDs, and not photon or forward IDs\n")
        raise

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]
    return tool

