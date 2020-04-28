# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2TauHypoTool')


# Here we need a large repository of configuration values
thresholdsL2Calo = {
    ('loose1', 20) : [15199.0, 0.8000],   
    ('loose1', 29) : [20696.0, 0.8000],   
    ('loose1', 70) : [50000.0, 0.8000],   
    ('loose1', 100): [80000.0, 0.8000],
    ('medium', 20): [ 15199.0, 0.7485],
    ('medium', 29): [ 20696.0, 0.7755],
    ('medium', 38): [ 29430.0, 0.8175],
    ('medium', 50): [ 40341.0, 0.8310],
    ('medium', 115): [100000.0, 0.8790],
    ('medium', 125): [109233.0, 0.8790],
    ('medium1', 20): [ 15199.0, 0.7485],
    ('medium1', 29): [ 20696.0, 0.7755],
    ('medium1', 38): [ 29430.0, 0.8175],
    ('medium1', 50): [ 40341.0, 0.8310],
    ('medium1', 115): [100000.0, 0.8790],
    ('medium1', 125): [109233.0, 0.8790] }

thresholdsL2ID = {
    ('loose1', 20): [4, 0.001, 0.001],
    ('loose1', 29): [4, 0.001, 0.001],
    ('loose1', 70): [4, 0.002, 0.002],
    ('loose1', 100):[4, 0.002, 0.002],    
    ('medium', 20): [7, 0.001, 0.099],
    ('medium', 29): [7, 0.001, 0.001],
    ('medium', 38): [7, 0.001, 0.001],
    ('medium', 50): [7, 0.001, 0.001],
    ('medium', 115):[7, 0.001, 0.001],
    ('medium', 125):[7, 0.001, 0.001],
    ('medium1', 20): [4, 0.001, 0.099],
    ('medium1', 29): [4, 0.001, 0.001],
    ('medium1', 38): [4, 0.001, 0.001],
    ('medium1', 50): [4, 0.001, 0.001],
    ('medium1', 115):[4, 0.001, 0.001],
    ('medium1', 125):[4, 0.001, 0.001] }

thresholdsL2 = {
    ('loose1', 20): [999.99, 999.99, 999.99, 999.99],
    ('loose1', 29): [999.99, 999.99, 999.99, 999.99],
    ('loose1', 70): [999.99, 999.99, 999.99, 999.99],
    ('loose1', 100): [999.99, 999.99, 999.99, 999.99],
    ('medium', 20): [  8.84, 2.84, 0.105, 0.073],
    ('medium', 29): [  9.72, 2.92, 0.086, 0.062],
    ('medium', 38): [  9.68, 3.04, 0.076, 0.059],
    ('medium', 50): [  8.22, 3.02, 0.062, 0.054],
    ('medium', 115): [999.99, 4.38, 0.057, 0.053],
    ('medium', 125): [999.99, 4.38, 0.057, 0.053],
    ('medium1', 20): [  8.84, 2.84, 0.105, 0.073],
    ('medium1', 29): [  9.72, 2.92, 0.086, 0.062],
    ('medium1', 38): [  9.68, 3.04, 0.076, 0.059],
    ('medium1', 50): [  8.22, 3.02, 0.062, 0.054],
    ('medium1', 115): [999.99, 4.38, 0.057, 0.053],
    ('medium1', 125): [999.99, 4.38, 0.057, 0.053] }

thresholdsEF = {
    ('medium', 20): [6,  20000.0, 1],
    ('medium', 25): [6,  25000.0, 1],
    ('medium', 29): [6,  29000.0, 1],
    ('medium', 38): [6,  38000.0, 1],
    ('medium', 50): [6,  50000.0, 1],
    ('medium', 115): [6, 115000.0, 1],
    ('medium', 125): [6, 125000.0, 1],
    ('loose1', 20): [3,  20000.0, 1],
    ('loose1', 25): [3,  25000.0, 1],
    ('loose1', 29): [3,  29000.0, 1],
    ('loose1', 35): [3,  35000.0, 1],
    ('loose1', 38): [3,  38000.0, 1],
    ('loose1', 50): [3,  50000.0, 1],
    ('loose1', 60): [3,  60000.0, 1],
    ('loose1', 80): [3,  80000.0, 1],
    ('loose1', 115): [3, 115000.0, 1],
    ('loose1', 125): [3, 125000.0, 1], 
    ('loose1', 160): [3, 160000.0, 1],
    ('loose1', 200): [3, 200000.0, 1],
    ('medium1', 0): [3,  0000.0, 2], 
    ('medium1', 12): [3,  12000.0, 2],
    ('medium1', 20): [3,  20000.0, 2],
    ('medium1', 25): [3,  25000.0, 2],
    ('medium1', 29): [3,  29000.0, 2],
    ('medium1', 35): [3,  35000.0, 2],
    ('medium1', 38): [3,  38000.0, 2],
    ('medium1', 40): [3,  40000.0, 2],
    ('medium1', 50): [3,  50000.0, 2],
    ('medium1', 60): [3,  60000.0, 2],
    ('medium1', 80): [3,  80000.0, 2],
    ('medium1', 115): [3, 115000.0, 2],
    ('medium1', 125): [3, 125000.0, 2], 
    ('medium1', 160): [3, 160000.0, 2], 
    ('medium1', 200): [3, 200000.0, 2],
    ('tight1', 20): [3,  20000.0, 3],
    ('tight1', 25): [3,  25000.0, 3],
    ('tight1', 29): [3,  29000.0, 3],
    ('tight1', 35): [3,  35000.0, 3],
    ('tight1', 38): [3,  38000.0, 3],
    ('tight1', 50): [3,  50000.0, 3],
    ('tight1', 60): [3,  60000.0, 3],
    ('tight1', 80): [3,  80000.0, 3],
    ('tight1', 115): [3, 115000.0, 3],
    ('tight1', 125): [3, 125000.0, 3], 
    ('tight1', 160): [3, 160000.0, 3],
    ('tight1', 200): [3, 200000.0, 3],
    ('verylooseRNN', 20): [3,  20000.0, 0],
    ('verylooseRNN', 25): [3,  25000.0, 0],
    ('verylooseRNN', 29): [3,  29000.0, 0],
    ('verylooseRNN', 35): [3,  35000.0, 0],
    ('verylooseRNN', 38): [3,  38000.0, 0],
    ('verylooseRNN', 50): [3,  50000.0, 0],
    ('verylooseRNN', 60): [3,  60000.0, 0],
    ('verylooseRNN', 80): [3,  80000.0, 0],
    ('verylooseRNN', 115): [3, 115000.0, 0],
    ('verylooseRNN', 125): [3, 125000.0, 0], 
    ('verylooseRNN', 160): [3, 160000.0, 0],
    ('verylooseRNN', 200): [3, 200000.0, 0],
    ('looseRNN', 20): [3,  20000.0, 1],
    ('looseRNN', 25): [3,  25000.0, 1],
    ('looseRNN', 29): [3,  29000.0, 1],
    ('looseRNN', 35): [3,  35000.0, 1],
    ('looseRNN', 38): [3,  38000.0, 1],
    ('looseRNN', 50): [3,  50000.0, 1],
    ('looseRNN', 60): [3,  60000.0, 1],
    ('looseRNN', 80): [3,  80000.0, 1],
    ('looseRNN', 115): [3, 115000.0, 1],
    ('looseRNN', 125): [3, 125000.0, 1], 
    ('looseRNN', 160): [3, 160000.0, 1],
    ('looseRNN', 200): [3, 200000.0, 1],
    ('mediumRNN', 0): [3,  0000.0, 2], 
    ('mediumRNN', 12): [3,  12000.0, 2],
    ('mediumRNN', 20): [3,  20000.0, 2],
    ('mediumRNN', 25): [3,  25000.0, 2],
    ('mediumRNN', 29): [3,  29000.0, 2],
    ('mediumRNN', 35): [3,  35000.0, 2],
    ('mediumRNN', 38): [3,  38000.0, 2],
    ('mediumRNN', 40): [3,  40000.0, 2],
    ('mediumRNN', 50): [3,  50000.0, 2],
    ('mediumRNN', 60): [3,  60000.0, 2],
    ('mediumRNN', 80): [3,  80000.0, 2],
    ('mediumRNN', 115): [3, 115000.0, 2],
    ('mediumRNN', 125): [3, 125000.0, 2], 
    ('mediumRNN', 160): [3, 160000.0, 2], 
    ('mediumRNN', 200): [3, 200000.0, 2],
    ('tightRNN', 20): [3,  20000.0, 3],
    ('tightRNN', 25): [3,  25000.0, 3],
    ('tightRNN', 29): [3,  29000.0, 3],
    ('tightRNN', 35): [3,  35000.0, 3],
    ('tightRNN', 38): [3,  38000.0, 3],
    ('tightRNN', 40): [3,  40000.0, 3],
    ('tightRNN', 50): [3,  50000.0, 3],
    ('tightRNN', 60): [3,  60000.0, 3],
    ('tightRNN', 80): [3,  80000.0, 3],
    ('tightRNN', 115): [3, 115000.0, 3],
    ('tightRNN', 125): [3, 125000.0, 3], 
    ('tightRNN', 160): [3, 160000.0, 3],
    ('tightRNN', 200): [3, 200000.0, 3],
    ('perf',0)       : [3,0.,2],
    ('perf',25)      : [3,25000.,2],    
    ('perf',160)  : [3,160000.,2],
    ('idperf',0)     : [3,0.,2],
    ('idperf',25)    : [3,25000.,2],
    ('idperf',160): [3,160000.,2]    
    }    

def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    chainPart = chainDict['chainParts'][0]

    strategy  = chainPart['preselection']
    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    currentHypo = TrigEFTauMVHypoTool(name)
    currentHypo.MonTool       = ""

    theThresh = thresholdsEF[(criteria, int(threshold))]
    currentHypo.numTrackMax = theThresh[0]
    currentHypo.EtCalibMin  = theThresh[1]
    currentHypo.level       = theThresh[2]
    currentHypo.method      = 2

    if criteria in [ 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN' ]:
      theThresh = thresholdsEF[(criteria, int(threshold))]
      currentHypo.numTrackMax = theThresh[0]
      currentHypo.EtCalibMin  = theThresh[1]
      currentHypo.level       = theThresh[2]
      currentHypo.numTrackMin = 0
      currentHypo.highptidthr = 280000.
      currentHypo.method      = 3
    elif 'perf' in criteria: 
      currentHypo.AcceptAll = True

    return currentHypo

def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    strategy  = chainPart['preselection']
    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    currentHypo = TrigTrackPreSelHypoTool(name)
    currentHypo.MonTool = ""

    if criteria == 'cosmic':
      currentHypo.LowerPtCut      = int(threshold)*1000.;
      currentHypo.TracksInCoreCut = 9999
      currentHypo.TracksInIsoCut  = 9999
      currentHypo.DeltaZ0Cut      = 9999.

    return currentHypo


def TrigL2TauHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    strategy  = chainPart['preselection']
    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from TrigTauHypo.TrigTauHypoConf import TrigTauGenericHypoMT
    currentHypo = TrigTauGenericHypoMT(name)
    currentHypo.MonTool = ""
    currentHypo.AcceptAll = False

    if strategy in [ 'ptonly', 'track', 'tracktwo', 'tracktwoEF', 'tracktwoMVA', 'tracktwoEFmvaTES' ]:
       currentHypo.Details  = [int(-1)]
       currentHypo.Formulas = ['y > '+threshold+'*1000.0']
    elif strategy == 'perf':
       currentHypo.AcceptAll = True
    return currentHypo
