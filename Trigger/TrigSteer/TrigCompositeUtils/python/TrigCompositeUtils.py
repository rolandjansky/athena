# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Note: These definitions shadow those in TrigComposite_v1.cxx

INITIAL_ROI_STRING  = "initialRoI"
INITIAL_REC_ROI_STRING = "initialRecRoI"
ROI_STRING = "roi"
VIEW_STRING = "view"
FEATURE_STRING = "feature"
SEED_STRING = "seed"

L1_DECODER_NODE_NAME_STRING = "L1"
FILTER_NODE_NAME_STRING = "F"
INPUT_MAKER_NODE_NAME_STRING = "IM"
HYPO_ALG_NODE_NAME_STRING = "H"
COMBO_HYPO_ALG_NODE_NAME_STRING = "CH"

def legName(chainName, legCounter):
    return ("leg{:03d}_{}".format(legCounter, chainName))

def isLegId(chainName):
    return chainName.startswith("leg")

