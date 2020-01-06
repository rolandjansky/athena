#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# 

def setupFilterMonitoring( filterAlg ):    
    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    
    inputKeys = filterAlg.Input
    
    nbins = 2+len(inputKeys)
    monTool.HistPath="HLTFramework/Filters"
    monTool.defineHistogram( 'counts;'+filterAlg.name(),  path='EXPERT', type='TH1I', title='Count of calls and inputs',
                             xbins=nbins, xmin=0, xmax=nbins,
                             xlabels=['n_execs', 'n_valid'] + inputKeys )
    monTool.defineHistogram( 'inputPresent,inputName;'+filterAlg.name()+"_InputEff",  path='EXPERT', type='TEfficiency', title='Input activity fraction;collection name;presence fraction',
                             xbins=len(inputKeys), xmin=0, xmax=len(inputKeys), xlabels=inputKeys )

    filterAlg.MonTool = monTool

def TriggerSummaryAlg( name ):
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg as Alg
    alg = Alg( name )
    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
    monTool.defineHistogram('TIME_SinceEventStart', path='EXPERT', type='TH1F',
                                   title='Time since beginning of event processing;time [ms]',
                                   xbins=100, xmin=0, xmax=3.5e3   )
    alg.MonTool = monTool
    return alg
