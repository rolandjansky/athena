# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredTElectronLikelihoodTools
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC), Kurt Brendlinger (Penn)
## Created:     Oct 2013
##
## Description: Apply the default configurations for the TElectronLikelihoodTool.
##
## Usage: In your ROOT session (after loading the libraries from RootCore):
##        TPython::LoadMacro("./path/to/this/file/ConfiguredTElectronIsEMSelectors.py");
## 2012 Medium
##        Root::* myTool = (void*)TPython::Eval("ConfiguredTElectronIsEMSelector(ROOT.egammaPID.ElectronIDLoosePP,electronPIDmenu.menuH4l2011)");
## 2012 MediumPP
##        Root::TElectronIsEMSelector* myTool = (void*)TPython::Eval("ConfiguredTElectronIsEMSelector(ROOT.egammaPID.ElectronIDMediumPP,electronPIDmenu.menu2011)");
##        myTool->initialize();
##
##=============================================================================

# import the needed Reflex and ROOT stuff
import ROOT

#
# For some reason there are problems unless I include the line below.
#
#import ConfiguredTElectronIsEMSelectors

from ElectronLikelihoodToolMapping import ElectronLikelihoodMap, electronLHmenu

def ConfiguredTElectronLikelihoodTool( quality, menu=electronLHmenu.offline2015 ):
    """
    Configure the TElectronIsEMSelector with the cuts of the given quality
    """    
    tool = ROOT.Root.TElectronLikelihoodTool()
    try:
        ntuple = ElectronLikelihoodMap(quality, menu)
    except KeyError:
        sys.stderr.write("Electron LH enum not found. Please use a valid LikeEnum (ElectronPhotonSelectorTools/TElectronLikelihoodTool).\n")
        raise

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.OperatingPoint = ntuple[0]
    return tool

