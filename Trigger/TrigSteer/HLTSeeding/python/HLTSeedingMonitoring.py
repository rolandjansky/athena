#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
import math


def CTPUnpackingMonitoring(maxItems, maxChains):
    tool = GenericMonitoringTool('MonTool')
    tool.HistPath="HLTFramework/HLTSeeding"
    tool.defineHistogram('TAVItems', path='EXPERT', type='TH1F', title='Number of active L1 TAV items;N Items;N Events',
                         xbins=maxItems, xmin=0, xmax=maxItems),
    tool.defineHistogram('Chains', path='EXPERT', type='TH1F', title='Number of activated Chains;N Chains;N Events',
                         xbins=maxChains, xmin=0, xmax=maxChains)
    return tool


def RoIsUnpackingMonitoring(prefix, maxCount, maxEta=3.):
    tool = GenericMonitoringTool('MonTool')
    tool.HistPath = f'HLTFramework/HLTSeeding/RoIs_{prefix}'
    tool.defineHistogram('count', path='EXPERT', type='TH1F', title=f'Number of {prefix} RoIs;N RoIs;N Events',
                         xbins=maxCount, xmin=0, xmax=maxCount),
    tool.defineHistogram('eta', path='EXPERT', type='TH1F', title=f'{prefix} RoIs eta;eta;N RoIs',
                         xbins=60, xmin=-maxEta, xmax=maxEta),
    tool.defineHistogram('phi', path='EXPERT', type='TH1F', title=f'{prefix} RoIs phi;phi;N RoIs',
                         xbins=64, xmin=-math.pi, xmax=math.pi),
    tool.defineHistogram('eta,phi', path='EXPERT', type='TH2F', title=f'{prefix} RoIs eta-phi;eta;phi;N RoIs',
                         xbins=30, xmin=-maxEta, xmax=maxEta,
                         ybins=32, ymin=-math.pi, ymax=math.pi)
    return tool


def PrescalingMonitoring():
    tool = GenericMonitoringTool('MonTool')
    tool.HistPath="HLTFramework/HLTSeeding"
    tool.defineHistogram('LB,HLTPSK', path='EXPERT', type='TH2I', opt='kAddBinsDynamically',
                         title='HLT prescale key used in LB;Lumiblock;HLTPSK',
                         xbins=100, xmin=0, xmax=100, ybins=5, ymin=0, ymax=5)
    tool.defineHistogram('Random', path='EXPERT', type='TH1F',
                         title='Random number distribution;random number',
                         xbins=100, xmin=0, xmax=1)
    return tool


def L1DataConsistencyMonitoring(flags):
    tool = GenericMonitoringTool('MonTool')
    tool.HistPath="HLTFramework/HLTSeeding/L1DataConsistencyChecker"

    tool.defineHistogram('TIME_consistencyCheck', path='EXPERT', type='TH1F',
                         title='Time of consistencyCheck() calls;Time [ms];N calls',
                         xbins=100, xmin=0, xmax=50)
    tool.defineHistogram('MissingTOBs', path='EXPERT', type='TH1F',
                         title='L1 thresholds with missing TOBs;Threshold name;Events with missing TOBs',
                         xbins=1, xmin=0, xmax=1)
    tool.defineHistogram('ThrName,CtpCount', path='EXPERT', type='TH2F',
                         title='Max threshold multiplicity accepted by CTP;Threshold name;Multiplicity',
                         xbins=1, xmin=0, xmax=1, ybins=20, ymin=0, ymax=20)
    tool.defineHistogram('ThrName,TobCount', path='EXPERT', type='TH2F',
                         title='Number of TOBs passing thresholds;Threshold name;Number of TOBs',
                         xbins=1, xmin=0, xmax=1, ybins=200, ymin=0, ymax=200)
    tool.defineHistogram('ThrName,TobMinusCtpCount', path='EXPERT', type='TH2F',
                         title='Diff. between N TOBs and CTP multiplicity;Threshold name;N TOBs - CTP multiplicity',
                         xbins=1, xmin=0, xmax=1, ybins=250, ymin=-50, ymax=200)
    return tool
