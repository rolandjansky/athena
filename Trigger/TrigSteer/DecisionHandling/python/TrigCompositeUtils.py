# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Note: These definitions shadow those in TrigComposite_v1.cxx

INITIAL_ROI_STRING  = "initialRoI"
INITIAL_REC_ROI_STRING = "initialRecRoI"
ROI_STRING = "roi"
VIEW_STRING = "view"
FEATURE_STRING = "feature"
SEED_STRING = "seed"

def legName(chainName, legCounter):
    return ("leg{:03d}_{}".format(legCounter, chainName))

def chainNameFromLegName(legName):
    return legName[7:] if isLegId(legName) else legName

def isLegId(chainName):
    return chainName.startswith("leg")

