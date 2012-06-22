# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy

def getGroomedJetsConfig():
    
    dictsConf = []

    ParentDict = {
                  'JetFinder'  :  'CamKt',
                  'JetdR'      :   1.2,
                  'JetInput'   :   'LCTopo',
                 }                                                                                                          

    GroomedDicts = [
                     #{ 'Type' : 'BDRSFiltering', 'args' : { 'SaveSubjets' : True } } #,
                     { 'Type' : 'BDRSFiltering', 'args' : { 'SaveSubjets' : True  , 'minSplitR' :0.} }
                   ]


    dictsConf += [ [ParentDict, GroomedDicts]  ]

    ParentDict = {
                  'JetFinder'  :  'CamKt',
                  'JetdR'      :   1.2,
                  'JetInput'   :   'Truth',
                  }

    dictsConf += [ [ParentDict, GroomedDicts]  ]


    return dictsConf
