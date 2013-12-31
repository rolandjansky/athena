# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy

def getGroomedJetsConfig():
    
    dictsConf = []

    GroomedDictsTrimmingSmall = [
                     { 'Type' : 'Trimming', 'args' : { 'SmallR' : 0.2, 'PtFrac' : 0.03} }
                   ]

    GroomedDictsTrimmingLarge = [
                     { 'Type' : 'Trimming', 'args' : { 'SmallR' : 0.3, 'PtFrac' : 0.05} }
                   ]


    GroomedDictsFiltering = [
                     { 'Type' : 'BDRSFiltering', 'args' : { 'minSplitR' : 0., 'massFraction' : 0.67 } }
                   ]

#### AntiKt R=0.6 ####
#    
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.6,
#                  'JetInput'   :   'Truth',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
#
###
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.6,
#                  'JetInput'   :   'Track',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
#
###
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.6,
#                  'JetInput'   :   'LCTopo',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
#    
#
#
#
##### AntiKt R=0.4 ####
#    
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.4,
#                  'JetInput'   :   'Truth',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
###
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.4,
#                  'JetInput'   :   'Track',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
#
###
#    ParentDict = {
#                  'JetFinder'  :  'AntiKt',
#                  'JetdR'      :   0.4,
#                  'JetInput'   :   'LCTopo',
#                 }
#
#    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingSmall)]  ]
#    
#
##### AntiKt R=1.0 ####

    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'Truth',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]  

##
    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'Track',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]

##
    ParentDict = {
                  'JetFinder'  :  'AntiKt',
                  'JetdR'      :   1.0,
                  'JetInput'   :   'LCTopo',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge)]  ]


#### C/A R=1.2####
    
    ParentDict = {
                  'JetFinder'  :  'CamKt',
                  'JetdR'      :   1.2,
                  'JetInput'   :   'Truth',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge+GroomedDictsFiltering)]  ]

##
    ParentDict = {
                  'JetFinder'  :  'CamKt',
                  'JetdR'      :   1.2,
                  'JetInput'   :   'Track',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge+GroomedDictsFiltering)]  ]

##
    ParentDict = {
                  'JetFinder'  :  'CamKt',
                  'JetdR'      :   1.2,
                  'JetInput'   :   'LCTopo',
                 }

    dictsConf += [ [ParentDict, copy.deepcopy(GroomedDictsTrimmingLarge+GroomedDictsFiltering)]  ]
  
   
    return dictsConf
