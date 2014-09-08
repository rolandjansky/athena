# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredTPhotonIsEMSelectors
##
## Author:      Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
## Created:     Feb 2012
##
## Description: Apply the default configurations for the TPhotonIsEMSelector.
##
## Usage: In your ROOT session (after loading the libraries from RootCore):
##        TPython::LoadMacro("./path/to/this/file/ConfiguredTPhotonIsEMSelectors.py");
##        Root::TPhotonIsEMSelector* myTool = (void*)TPython::Eval("ConfiguredTPhotonIsEMSelector(ROOT.egammaPID.PhotonIDMediumPP)");
##        myTool->initialize();
##
##=============================================================================

# import the needed Reflex and ROOT stuff
import ROOT

from PhotonPhotonSelectorTools.PhotonIsEMSelectorMapping import PhotonIsEMMap, photonPIDmenu

def ConfiguredTPhotonIsEMSelector( quality, menu=photonPIDmenu.menu2012 ):
    """
    Configure the TPhotonIsEMSelector with the cuts of the given quality
    """    
    tool = ROOT.Root.TPhotonIsEMSelector()
    try:
        ntuple = PhotonIsEMMap(quality, menu)
    except KeyError:
        sys.stderr.write("Photon quality not found. Please use an egammaIDQuality (egammaEvent/egammaPIDdefs.h).\n This function only supports standard photon IDs, and not photon or forward IDs\n")
        raise

    # Configure it with the standard configuration
    ntuple[1](tool)
    tool.isEMMask = ntuple[0]
    return tool

