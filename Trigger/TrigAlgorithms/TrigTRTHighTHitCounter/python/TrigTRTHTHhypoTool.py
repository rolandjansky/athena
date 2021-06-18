# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHhypoTool

def _IncTool(name):
    
    tool = TrigTRTHTHhypoTool( name ) 
    tool.AcceptAll = False
    tool.MinTRTHTHitsRoad = 20
    tool.MinHTRatioRoad = 0.4
    tool.MinTRTHTHitsWedge = 30
    tool.MinHTRatioWedge = 0.4
    tool.DoWedge = True
    tool.DoRoad = False 
    return tool

def TrigTRTHTHhypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    name = d['chainName'] 
    return _IncTool( name ) 

