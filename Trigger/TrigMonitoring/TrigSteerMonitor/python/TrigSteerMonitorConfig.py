# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def SchedulerMonSvcCfg(flags, name='SchedulerMonSvc'):
    monsvc = CompFactory.SchedulerMonSvc(name)
    monsvc.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)

    # From GaudiHive AlgsExecutionStates::State enum
    stateNames = ['INITIAL', 'CONTROLREADY', 'DATAREADY', 'RESOURCELESS',
                  'SCHEDULED', 'EVTACCEPTED', 'EVTREJECTED', 'ERROR']

    monsvc.MonTool.defineHistogram('SnapNumber,AlgStates', weight='StateTotalCounts', path='EXPERT', type='TH2D',
                            title='Scheduler algorithm states vs time (snap number);Snap number;Algorithm state',
                            xbins=1000, xmin=0, xmax=1000,
                            ybins=8, ymin=-0.5, ymax=7.5, ylabels=stateNames,
                            opt='kCanRebin')
    monsvc.MonTool.defineHistogram('WallTimeSeconds,AlgStates', weight='StateTotalCounts', path='EXPERT', type='TH2D',
                            title='Scheduler algorithm states vs time;Time [s];Algorithm state',
                            xbins=600, xmin=0, xmax=60,
                            ybins=8, ymin=-0.5, ymax=7.5, ylabels=stateNames,
                            opt='kCanRebin')
    monsvc.MonTool.defineHistogram('SnapNumber,FreeSlots', path='EXPERT', type='TProfile',
                            title='Number of free slots vs time (snap number);Snap number;Number of free slots',
                            xbins=1000, xmin=0, xmax=1000,
                            ybins=10, ymin=0, ymax=10,
                            opt='kCanRebin')
    monsvc.MonTool.defineHistogram('WallTimeSeconds,FreeSlots', path='EXPERT', type='TProfile',
                            title='Number of free slots vs time ;Time [s];Number of free slots',
                            xbins=600, xmin=0, xmax=60,
                            ybins=10, ymin=0, ymax=10,
                            opt='kCanRebin')
    monsvc.MonTool.defineHistogram('AlgStates', weight='StateTotalCounts', path='EXPERT', type='TH1D',
                            title='Scheduler algorithm states;Algorithm states;Snapshots',
                            xbins=8, xmin=-0.5, xmax=7.5, xlabels=stateNames)
    monsvc.MonTool.defineHistogram('FreeSlots', path='EXPERT', type='TH1D',
                            title='Number of free slots;Free slots;Snapshots',
                            xbins=10, xmin=0, xmax=10, opt='kCanRebin')
    monsvc.MonTool.defineHistogram('TIME_monCallback', path='EXPERT', type='TH1D',
                            title='Time of callback calls;Time [us];Calls',
                            xbins=500, xmin=0, xmax=5000)

    acc = ComponentAccumulator()
    acc.addService(monsvc)
    return acc
