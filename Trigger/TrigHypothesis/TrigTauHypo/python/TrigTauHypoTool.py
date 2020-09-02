# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2TauHypoTool')

from collections import namedtuple


# Here we need a large repository of configuration values
# The meaning of the configuration values is as follows:
# ('Id working point', 'pt threshold': ['Maximum number of tracks[0]', 'etmincalib[1]', 'Id level[2]'])
TauCuts = namedtuple('TauCuts','numTrackMax EtCalibMin level')
thresholdsEF = {
    ('medium', 20): TauCuts(6,  20000.0, 1),
    ('medium', 25): TauCuts(6,  25000.0, 1),
    ('medium', 29): TauCuts(6,  29000.0, 1),
    ('medium', 38): TauCuts(6,  38000.0, 1),
    ('medium', 50): TauCuts(6,  50000.0, 1),
    ('medium', 115): TauCuts(6, 115000.0, 1),
    ('medium', 125): TauCuts(6, 125000.0, 1),
    ('loose1', 20): TauCuts(3,  20000.0, 1),
    ('loose1', 25): TauCuts(3,  25000.0, 1),
    ('loose1', 29): TauCuts(3,  29000.0, 1),
    ('loose1', 35): TauCuts(3,  35000.0, 1),
    ('loose1', 38): TauCuts(3,  38000.0, 1),
    ('loose1', 50): TauCuts(3,  50000.0, 1),
    ('loose1', 60): TauCuts(3,  60000.0, 1),
    ('loose1', 80): TauCuts(3,  80000.0, 1),
    ('loose1', 115): TauCuts(3, 115000.0, 1),
    ('loose1', 125): TauCuts(3, 125000.0, 1), 
    ('loose1', 160): TauCuts(3, 160000.0, 1),
    ('loose1', 200): TauCuts(3, 200000.0, 1),
    ('medium1', 0): TauCuts(3,  0000.0, 2), 
    ('medium1', 12): TauCuts(3,  12000.0, 2),
    ('medium1', 20): TauCuts(3,  20000.0, 2),
    ('medium1', 25): TauCuts(3,  25000.0, 2),
    ('medium1', 29): TauCuts(3,  29000.0, 2),
    ('medium1', 35): TauCuts(3,  35000.0, 2),
    ('medium1', 38): TauCuts(3,  38000.0, 2),
    ('medium1', 40): TauCuts(3,  40000.0, 2),
    ('medium1', 50): TauCuts(3,  50000.0, 2),
    ('medium1', 60): TauCuts(3,  60000.0, 2),
    ('medium1', 80): TauCuts(3,  80000.0, 2),
    ('medium1', 115): TauCuts(3, 115000.0, 2),
    ('medium1', 125): TauCuts(3, 125000.0, 2), 
    ('medium1', 160): TauCuts(3, 160000.0, 2), 
    ('medium1', 200): TauCuts(3, 200000.0, 2),
    ('tight1', 20): TauCuts(3,  20000.0, 3),
    ('tight1', 25): TauCuts(3,  25000.0, 3),
    ('tight1', 29): TauCuts(3,  29000.0, 3),
    ('tight1', 35): TauCuts(3,  35000.0, 3),
    ('tight1', 38): TauCuts(3,  38000.0, 3),
    ('tight1', 50): TauCuts(3,  50000.0, 3),
    ('tight1', 60): TauCuts(3,  60000.0, 3),
    ('tight1', 80): TauCuts(3,  80000.0, 3),
    ('tight1', 115): TauCuts(3, 115000.0, 3),
    ('tight1', 125): TauCuts(3, 125000.0, 3), 
    ('tight1', 160): TauCuts(3, 160000.0, 3),
    ('tight1', 200): TauCuts(3, 200000.0, 3),
    ('verylooseRNN', 20): TauCuts(3,  20000.0, 0),
    ('verylooseRNN', 25): TauCuts(3,  25000.0, 0),
    ('verylooseRNN', 29): TauCuts(3,  29000.0, 0),
    ('verylooseRNN', 35): TauCuts(3,  35000.0, 0),
    ('verylooseRNN', 38): TauCuts(3,  38000.0, 0),
    ('verylooseRNN', 50): TauCuts(3,  50000.0, 0),
    ('verylooseRNN', 60): TauCuts(3,  60000.0, 0),
    ('verylooseRNN', 80): TauCuts(3,  80000.0, 0),
    ('verylooseRNN', 115): TauCuts(3, 115000.0, 0),
    ('verylooseRNN', 125): TauCuts(3, 125000.0, 0), 
    ('verylooseRNN', 160): TauCuts(3, 160000.0, 0),
    ('verylooseRNN', 200): TauCuts(3, 200000.0, 0),
    ('looseRNN', 20): TauCuts(3,  20000.0, 1),
    ('looseRNN', 25): TauCuts(3,  25000.0, 1),
    ('looseRNN', 29): TauCuts(3,  29000.0, 1),
    ('looseRNN', 35): TauCuts(3,  35000.0, 1),
    ('looseRNN', 38): TauCuts(3,  38000.0, 1),
    ('looseRNN', 50): TauCuts(3,  50000.0, 1),
    ('looseRNN', 60): TauCuts(3,  60000.0, 1),
    ('looseRNN', 80): TauCuts(3,  80000.0, 1),
    ('looseRNN', 115): TauCuts(3, 115000.0, 1),
    ('looseRNN', 125): TauCuts(3, 125000.0, 1), 
    ('looseRNN', 160): TauCuts(3, 160000.0, 1),
    ('looseRNN', 200): TauCuts(3, 200000.0, 1),
    ('mediumRNN', 0): TauCuts(3,  0000.0, 2), 
    ('mediumRNN', 12): TauCuts(3,  12000.0, 2),
    ('mediumRNN', 20): TauCuts(3,  20000.0, 2),
    ('mediumRNN', 25): TauCuts(3,  25000.0, 2),
    ('mediumRNN', 29): TauCuts(3,  29000.0, 2),
    ('mediumRNN', 35): TauCuts(3,  35000.0, 2),
    ('mediumRNN', 38): TauCuts(3,  38000.0, 2),
    ('mediumRNN', 40): TauCuts(3,  40000.0, 2),
    ('mediumRNN', 50): TauCuts(3,  50000.0, 2),
    ('mediumRNN', 60): TauCuts(3,  60000.0, 2),
    ('mediumRNN', 80): TauCuts(3,  80000.0, 2),
    ('mediumRNN', 115): TauCuts(3, 115000.0, 2),
    ('mediumRNN', 125): TauCuts(3, 125000.0, 2), 
    ('mediumRNN', 160): TauCuts(3, 160000.0, 2), 
    ('mediumRNN', 200): TauCuts(3, 200000.0, 2),
    ('tightRNN', 20): TauCuts(3,  20000.0, 3),
    ('tightRNN', 25): TauCuts(3,  25000.0, 3),
    ('tightRNN', 29): TauCuts(3,  29000.0, 3),
    ('tightRNN', 35): TauCuts(3,  35000.0, 3),
    ('tightRNN', 38): TauCuts(3,  38000.0, 3),
    ('tightRNN', 40): TauCuts(3,  40000.0, 3),
    ('tightRNN', 50): TauCuts(3,  50000.0, 3),
    ('tightRNN', 60): TauCuts(3,  60000.0, 3),
    ('tightRNN', 80): TauCuts(3,  80000.0, 3),
    ('tightRNN', 115): TauCuts(3, 115000.0, 3),
    ('tightRNN', 125): TauCuts(3, 125000.0, 3), 
    ('tightRNN', 160): TauCuts(3, 160000.0, 3),
    ('tightRNN', 200): TauCuts(3, 200000.0, 3),
    ('perf',0)       : TauCuts(3,0.,2),
    ('perf',25)      : TauCuts(3,25000.,2),    
    ('perf',160)  : TauCuts(3,160000.,2),
    ('idperf',0)     : TauCuts(3,0.,2),
    ('idperf',25)    : TauCuts(3,25000.,2),
    ('idperf',160): TauCuts(3,160000.,2)    
    }    

def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    currentHypo = TrigEFTauMVHypoTool(name)
    currentHypo.MonTool       = ""

    theThresh = thresholdsEF[(criteria, int(threshold))]
    currentHypo.numTrackMax = theThresh.numTrackMax
    currentHypo.EtCalibMin  = theThresh.EtCalibMin
    currentHypo.level       = theThresh.level
    currentHypo.method      = 2

    if criteria in [ 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN' ]:
      currentHypo.numTrackMin = 0
      currentHypo.highptidthr = 280000.
      currentHypo.method      = 3
    elif 'perf' in criteria: 
      currentHypo.AcceptAll = True

    return currentHypo

def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    currentHypo = TrigTrackPreSelHypoTool(name)
    currentHypo.MonTool = ""

    if criteria == 'cosmic':
      currentHypo.LowerPtCut      = int(threshold)*1000.
      currentHypo.TracksInCoreCut = 9999
      currentHypo.TracksInIsoCut  = 9999
      currentHypo.DeltaZ0Cut      = 9999.

    return currentHypo


def TrigL2TauHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigTauGenericHypoMT
    currentHypo = TrigTauGenericHypoMT(name)
    currentHypo.MonTool  = ""
    currentHypo.Details  = [int(-1)]
    currentHypo.Formulas = ['y > '+threshold+'*1000.0']

    if 'perf' in criteria:
       currentHypo.AcceptAll = True

    return currentHypo
