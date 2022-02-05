# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLT.Menu.MenuComponents import Chain

def generateChains( flags, chainDict ):
    chain = Chain( name=chainDict['chainName'], L1Thresholds=['FSNOSEED'], ChainSteps=[ ] )
    return chain