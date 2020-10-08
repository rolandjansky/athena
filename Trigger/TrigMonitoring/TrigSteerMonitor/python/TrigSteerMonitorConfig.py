# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def SchedulerMonSvcCfg(flags, name='SchedulerMonSvc'):
    monsvc = CompFactory.SchedulerMonSvc(name)
    monsvc.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)

    # Plots vs snapshot number can be enabled with flags.SchedulerMonSvc.enablePlotsVsSnapNumber
    # They are disabled by default as they don't give much information on top of the plots vs walltime
    enablePlotsVsSnapNumber = False
    if flags.hasCategory('SchedulerMonSvc') and flags.SchedulerMonSvc.hasFlag('enablePlotsVsSnapNumber'):
        enablePlotsVsSnapNumber = flags.SchedulerMonSvc.enablePlotsVsSnapNumber

    # From GaudiHive AlgsExecutionStates::State enum
    stateNames = ['INITIAL', 'CONTROLREADY', 'DATAREADY', 'RESOURCELESS',
                  'SCHEDULED', 'EVTACCEPTED', 'EVTREJECTED', 'ERROR']
    activeStates = ['CONTROLREADY', 'DATAREADY', 'RESOURCELESS', 'SCHEDULED']

    # Helper functions
    def defineHist1D(varName, varLabel, labels=None, **kwargs):
        monsvc.MonTool.defineHistogram(varName, path='EXPERT', type='TH1D',
                                title=varLabel+';'+varLabel+';Snapshots',
                                xlabels=labels, opt='kCanRebin', **kwargs)

    def defineHistVsWallTime(varName, varLabel, _type, labels=None, **kwargs):
        monsvc.MonTool.defineHistogram(
            'WallTimeSeconds,'+varName, path='EXPERT', type=_type,
            title=varLabel+' vs time;Time [s];'+varLabel,
            xbins=600, xmin=0, xmax=60, ylabels=labels, opt='kCanRebin', **kwargs)

    def defineHistVsSnapNumber(varName, varLabel, _type, labels=None, **kwargs):
        monsvc.MonTool.defineHistogram(
            'SnapNumber,'+varName, path='EXPERT', type=_type,
            title=varLabel+' vs time (snap number);Snap number;'+varLabel,
            xbins=1000, xmin=0, xmax=1000, ylabels=labels, opt='kCanRebin', **kwargs)

    def defineStandardHistogramSet(varName, varLabel, nbins, min, max, labels=None, type2D='TProfile', **kwargs):
        defineHist1D(varName, varLabel, labels, xbins=nbins, xmin=min, xmax=max, **kwargs)
        defineHistVsWallTime(varName, varLabel, type2D, labels, ybins=nbins, ymin=min, ymax=max, **kwargs)
        if enablePlotsVsSnapNumber:
            defineHistVsSnapNumber(varName, varLabel, type2D, labels, ybins=nbins, ymin=min, ymax=max, **kwargs)

    # Histogram definitions
    defineStandardHistogramSet('AlgStates', 'Algorithm state', 8, -0.5, 7.5, labels=stateNames, type2D='TH2D', weight='StateTotalCounts')
    defineStandardHistogramSet('FreeSlots', 'Number of free slots', 10, 0, 10)
    for state in stateNames:
        defineStandardHistogramSet(state, 'N algs in '+state+' state', 100, 0, 100)
    for state in activeStates:
        defineStandardHistogramSet(state+'_Over_Threads', 'N '+state+' / N threads', 100, 0, 10)
        defineStandardHistogramSet(state+'_Over_Active', 'N '+state+' / N active states', 100, 0, 1)

    monsvc.MonTool.defineHistogram('TIME_monCallback', path='EXPERT', type='TH1D',
                                    title='Time of callback calls;Time [us];Calls',
                                    xbins=500, xmin=0, xmax=5000)

    acc = ComponentAccumulator()
    acc.addService(monsvc)
    return acc

def getTrigErrorMonTool(name='TrigErrorMonTool'):
    errorMonTool = CompFactory.TrigErrorMonTool(name)
    errorMonTool.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)

    errorMonTool.MonTool.defineHistogram(
        'ErrorAlgName,ErrorCode', path='EXPERT', type='TH2I',
        title='Error StatusCodes per algorithm;Algorithm name;StatusCode',
        xbins=1, xmin=0, xmax=1, ybins=1, ymin=0, ymax=1)

    return errorMonTool
