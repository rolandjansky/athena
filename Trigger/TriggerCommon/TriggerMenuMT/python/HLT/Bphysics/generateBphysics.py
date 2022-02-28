# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLT.Config.MenuComponents import Chain

def generateChains( flags, chainDict ):
    thresholds = [p["L1threshold"] for p in chainDict['chainParts'] if p['signature'] == 'Bphysics' ]
    chain = Chain( name=chainDict['chainName'], L1Thresholds=thresholds, ChainSteps=[ ] )
    return chain