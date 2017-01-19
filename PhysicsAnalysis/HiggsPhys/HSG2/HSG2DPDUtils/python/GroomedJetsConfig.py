# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Copied from PhysicsAnalysis/D3PDMaker/QcdD3PDMaker/python/GroomedJetsConfig.py

import copy

def getGroomedJetsConfig():
    
    dictsConf = []

    GroomedDictsTrimmingLarge = [
                     { 'Type' : 'Trimming', 'args' : { 'SmallR' : 0.3, 'PtFrac' : 0.05} }
                   ]

##### AntiKt R=1.0 ####
    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'Truth',
                 }
    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]  


    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'Track',
                 }
    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]


    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'LCTopo',
                 }
    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]

    return dictsConf
