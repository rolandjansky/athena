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
                             labels=['n_execs', 'n_valid'] + inputKeys )
    filterAlg.MonTool = monTool
