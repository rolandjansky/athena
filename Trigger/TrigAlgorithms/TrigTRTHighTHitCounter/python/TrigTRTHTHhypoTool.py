# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHhypoTool
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

def _IncTool(name):
    
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [ defineHistogram('HTRatioRoad', type='TH1F', path='EXPERT', title="TrigTRTHTH Hypo HTRatioRoad", xbins=10, xmin=0.0, xmax=0.9),
                           defineHistogram('HTRatioWedge', type='TH1F', path='EXPERT', title="TrigTRTHTH Hypo HTRatioWedge", xbins=10, xmin=0.0, xmax=0.9),
                           defineHistogram('TRTHTHitsRoad', type='TH1F', path='EXPERT', title="TrigTRTHTH Hypo TRTHTHitsRoad", xbins=100, xmin=0, xmax=100),
                           defineHistogram('TRTHTHitsWedge', type='TH1F', path='EXPERT', title="TrigTRTHTH Hypo TRTHTHitsWedge", xbins=100, xmin=0, xmax=100),]

    tool = TrigTRTHTHhypoTool( name ) 
    tool.AcceptAll = False
    tool.MinTRTHTHitsRoad = 20
    tool.MinHTRatioRoad = 0.4
    tool.MinTRTHTHitsWedge = 30
    tool.MinHTRatioWedge = 0.4
    tool.DoWedge = True
    tool.DoRoad = False
    monTool.HistPath = 'TrigTRTHTHhypo/'+tool.getName()
    tool.MonTool = monTool 
    return tool

def TrigTRTHTHhypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    name = d['chainName'] 
    return _IncTool( name ) 

