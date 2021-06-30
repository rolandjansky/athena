# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2TauHypoTool')

from collections import namedtuple


# Here we need a large repository of configuration values
# The meaning of the configuration values is as follows:
# ('Id working point', 'pt threshold': ['Maximum number of tracks[0]', 'etmincalib[1]', 'Id level[2]'])
TauCuts = namedtuple('TauCuts','numTrackMax numTrackWideTrackMax EtCalibMin level')
thresholdsEF = {
    ('medium', 20): TauCuts(6, 1, 20000.0, 1),
    ('medium', 25): TauCuts(6, 1, 25000.0, 1),
    ('medium', 29): TauCuts(6, 1, 29000.0, 1),
    ('medium', 38): TauCuts(6, 1, 38000.0, 1),
    ('medium', 50): TauCuts(6, 1, 50000.0, 1),
    ('medium', 115): TauCuts(6, 1, 115000.0, 1),
    ('medium', 125): TauCuts(6, 1, 125000.0, 1),
    ('loose1', 20): TauCuts(3, 1, 20000.0, 1),
    ('loose1', 25): TauCuts(3, 1, 25000.0, 1),
    ('loose1', 29): TauCuts(3, 1, 29000.0, 1),
    ('loose1', 35): TauCuts(3, 1, 35000.0, 1),
    ('loose1', 38): TauCuts(3, 1, 38000.0, 1),
    ('loose1', 50): TauCuts(3, 1, 50000.0, 1),
    ('loose1', 60): TauCuts(3, 1, 60000.0, 1),
    ('loose1', 80): TauCuts(3, 1, 80000.0, 1),
    ('loose1', 115): TauCuts(3, 1, 115000.0, 1),
    ('loose1', 125): TauCuts(3, 1, 125000.0, 1), 
    ('loose1', 160): TauCuts(3, 1, 160000.0, 1),
    ('loose1', 200): TauCuts(3, 1, 200000.0, 1),
    ('medium1', 0): TauCuts(3, 1,  0000.0, 2), 
    ('medium1', 12): TauCuts(3, 1, 12000.0, 2),
    ('medium1', 20): TauCuts(3, 1, 20000.0, 2),
    ('medium1', 25): TauCuts(3, 1, 25000.0, 2),
    ('medium1', 29): TauCuts(3, 1, 29000.0, 2),
    ('medium1', 35): TauCuts(3, 1, 35000.0, 2),
    ('medium1', 38): TauCuts(3, 1, 38000.0, 2),
    ('medium1', 40): TauCuts(3, 1, 40000.0, 2),
    ('medium1', 50): TauCuts(3, 1, 50000.0, 2),
    ('medium1', 60): TauCuts(3, 1, 60000.0, 2),
    ('medium1', 80): TauCuts(3, 1, 80000.0, 2),
    ('medium1', 115): TauCuts(3, 1, 115000.0, 2),
    ('medium1', 125): TauCuts(3, 1, 125000.0, 2), 
    ('medium1', 160): TauCuts(3, 1, 160000.0, 2), 
    ('medium1', 200): TauCuts(3, 1, 200000.0, 2),
    ('tight1', 20): TauCuts(3, 1, 20000.0, 3),
    ('tight1', 25): TauCuts(3, 1, 25000.0, 3),
    ('tight1', 29): TauCuts(3, 1, 29000.0, 3),
    ('tight1', 35): TauCuts(3, 1, 35000.0, 3),
    ('tight1', 38): TauCuts(3, 1, 38000.0, 3),
    ('tight1', 50): TauCuts(3, 1, 50000.0, 3),
    ('tight1', 60): TauCuts(3, 1, 60000.0, 3),
    ('tight1', 80): TauCuts(3, 1, 80000.0, 3),
    ('tight1', 115): TauCuts(3, 1, 115000.0, 3),
    ('tight1', 125): TauCuts(3, 1, 125000.0, 3), 
    ('tight1', 160): TauCuts(3, 1, 160000.0, 3),
    ('tight1', 200): TauCuts(3, 1, 200000.0, 3),
    ('verylooseRNN', 20): TauCuts(3, 1, 20000.0, 0),
    ('verylooseRNN', 25): TauCuts(3, 1, 25000.0, 0),
    ('verylooseRNN', 29): TauCuts(3, 1, 29000.0, 0),
    ('verylooseRNN', 35): TauCuts(3, 1, 35000.0, 0),
    ('verylooseRNN', 38): TauCuts(3, 1, 38000.0, 0),
    ('verylooseRNN', 50): TauCuts(3, 1, 50000.0, 0),
    ('verylooseRNN', 60): TauCuts(3, 1, 60000.0, 0),
    ('verylooseRNN', 80): TauCuts(3, 1, 80000.0, 0),
    ('verylooseRNN', 115): TauCuts(3, 1, 115000.0, 0),
    ('verylooseRNN', 125): TauCuts(3, 1, 125000.0, 0), 
    ('verylooseRNN', 160): TauCuts(3, 1, 160000.0, 0),
    ('verylooseRNN', 200): TauCuts(3, 1, 200000.0, 0),
    ('looseRNN', 20): TauCuts(3, 1, 20000.0, 1),
    ('looseRNN', 25): TauCuts(3, 1, 25000.0, 1),
    ('looseRNN', 29): TauCuts(3, 1, 29000.0, 1),
    ('looseRNN', 35): TauCuts(3, 1, 35000.0, 1),
    ('looseRNN', 38): TauCuts(3, 1, 38000.0, 1),
    ('looseRNN', 50): TauCuts(3, 1, 50000.0, 1),
    ('looseRNN', 60): TauCuts(3, 1, 60000.0, 1),
    ('looseRNN', 80): TauCuts(3, 1, 80000.0, 1),
    ('looseRNN', 115): TauCuts(3, 1, 115000.0, 1),
    ('looseRNN', 125): TauCuts(3, 1, 125000.0, 1), 
    ('looseRNN', 160): TauCuts(3, 1, 160000.0, 1),
    ('looseRNN', 200): TauCuts(3, 1, 200000.0, 1),
    ('mediumRNN', 0): TauCuts(3, 1, 0000.0, 2), 
    ('mediumRNN', 12): TauCuts(3, 1, 12000.0, 2),
    ('mediumRNN', 20): TauCuts(3, 1, 20000.0, 2),
    ('mediumRNN', 25): TauCuts(3, 1, 25000.0, 2),
    ('mediumRNN', 29): TauCuts(3, 1, 29000.0, 2),
    ('mediumRNN', 35): TauCuts(3, 1, 35000.0, 2),
    ('mediumRNN', 38): TauCuts(3, 1, 38000.0, 2),
    ('mediumRNN', 40): TauCuts(3, 1, 40000.0, 2),
    ('mediumRNN', 50): TauCuts(3, 1, 50000.0, 2),
    ('mediumRNN', 60): TauCuts(3, 1, 60000.0, 2),
    ('mediumRNN', 80): TauCuts(3, 1,  80000.0, 2),
    ('mediumRNN', 100): TauCuts(3, 1, 100000.0, 2),
    ('mediumRNN', 115): TauCuts(3, 1, 115000.0, 2),
    ('mediumRNN', 125): TauCuts(3, 1, 125000.0, 2), 
    ('mediumRNN', 160): TauCuts(3, 1, 160000.0, 2), 
    ('mediumRNN', 180): TauCuts(3, 1, 180000.0, 2), 
    ('mediumRNN', 200): TauCuts(3, 1, 200000.0, 2),
    ('tightRNN', 20): TauCuts(3, 1, 20000.0, 3),
    ('tightRNN', 25): TauCuts(3, 1, 25000.0, 3),
    ('tightRNN', 29): TauCuts(3, 1, 29000.0, 3),
    ('tightRNN', 35): TauCuts(3, 1, 35000.0, 3),
    ('tightRNN', 38): TauCuts(3, 1, 38000.0, 3),
    ('tightRNN', 40): TauCuts(3, 1, 40000.0, 3),
    ('tightRNN', 50): TauCuts(3, 1, 50000.0, 3),
    ('tightRNN', 60): TauCuts(3, 1, 60000.0, 3),
    ('tightRNN', 80): TauCuts(3, 1, 80000.0, 3),
    ('tightRNN', 115): TauCuts(3, 1, 115000.0, 3),
    ('tightRNN', 125): TauCuts(3, 1, 125000.0, 3), 
    ('tightRNN', 160): TauCuts(3, 1, 160000.0, 3),
    ('tightRNN', 180): TauCuts(3, 1, 180000.0, 3),
    ('tightRNN', 200): TauCuts(3, 1, 200000.0, 3),
    ('perf',0)       : TauCuts(3, 1,0.,-1111),
    ('perf',25)      : TauCuts(3, 1,25000.,-1111),
    ('perf',35)      : TauCuts(3, 1,35000.,-1111),
    ('perf',160)  : TauCuts(3, 1,160000.,-1111),
    ('perf',200)  : TauCuts(3, 1,200000.,-1111),
    ('idperf',0)     : TauCuts(3,999, 0.,2),
    ('idperf',25)    : TauCuts(3,999,25000.,2),
    ('idperf',35)    : TauCuts(3,999,35000.,2),
    ('idperf',160): TauCuts(3,999,160000.,2),    
    ('idperf',200): TauCuts(3,999,200000.,2)
    }    

# ATR-22644 + ATR-23239
GeV = 1000.0
DiKaonCuts = namedtuple('DiKaonCuts','massTrkSysMin massTrkSysMax massTrkSysKaonMin massTrkSysKaonMax massTrkSysKaonPiMin massTrkSysKaonPiMax targetMassTrkSysKaonPi leadTrkPtMin EtCalibMin EMPOverTrkSysPMax')
thresholdsEF_dikaon = {
    ('dikaonmass', 25):      DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.987*GeV, 1.060*GeV, 0.0*GeV, 1000.0*GeV, 0.0*GeV,   15.0*GeV, 25.0*GeV, 1.5),
    ('dikaonmass', 35):      DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.987*GeV, 1.060*GeV, 0.0*GeV, 1000.0*GeV, 0.0*GeV,   25.0*GeV, 35.0*GeV, 1.5),
    ('kaonpi1', 25):         DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV,  0.79*GeV, 0.99*GeV,  0.89*GeV,  15.0*GeV, 25.0*GeV, 1.0),
    ('kaonpi1', 35):         DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV,  0.79*GeV, 0.99*GeV,  0.89*GeV,  25.0*GeV, 35.0*GeV, 1.0),
    ('kaonpi2', 25):         DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV,  1.8*GeV, 1.93*GeV,   1.865*GeV, 15.0*GeV, 25.0*GeV, 1.0),
    ('kaonpi2', 35):         DiKaonCuts(0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV,  1.8*GeV, 1.93*GeV,   1.865*GeV, 25.0*GeV, 35.0*GeV, 1.0),
    ('dipion1', 25):         DiKaonCuts(0.475*GeV, 1.075*GeV, 0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV, 0.0*GeV,   15.0*GeV, 25.0*GeV, 1.0),
    ('dipion2', 25):         DiKaonCuts(0.460*GeV, 0.538*GeV, 0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV, 0.0*GeV,   15.0*GeV, 25.0*GeV, 1.0),
    ('dipion3', 25):         DiKaonCuts(0.279*GeV, 0.648*GeV, 0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV, 0.0*GeV,   25.0*GeV, 25.0*GeV, 2.2),
    ('dipion4', 25):         DiKaonCuts(0.460*GeV, 1.075*GeV, 0.0*GeV, 1000.0*GeV,  0.0*GeV, 1000.0*GeV, 0.0*GeV,   15.0*GeV, 25.0*GeV, 1.0),
}
SinglePionCuts = namedtuple('SinglePionCuts','leadTrkPtMin EtCalibMin nTrackMax nWideTrackMax dRmaxMax etOverPtLeadTrkMin etOverPtLeadTrkMax')
thresholdsEF_singlepion = {
    ('singlepion', 25): SinglePionCuts(30.0*GeV, 25.0*GeV, 1, 0, 0.06, 0.4, 0.85)
}

def TrigPresTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from AthenaConfiguration.ComponentFactory import CompFactory
    currentHypo = CompFactory.TrigEFTauMVHypoTool(name)
    currentHypo.MonTool       = ""

    theThresh = thresholdsEF[(criteria, int(threshold))]
    currentHypo.numTrackMax = theThresh.numTrackMax
    currentHypo.EtCalibMin  = theThresh.EtCalibMin
    currentHypo.level       = -1111
    currentHypo.method      = 0

    if 'idperf' in criteria:
        currentHypo.AcceptAll = True

    return currentHypo


def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from AthenaConfiguration.ComponentFactory import CompFactory
    if criteria in ['medium', 'loose1', 'medium1', 'tight1', 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN', 'idperf', 'perf'] :
    
        currentHypo = CompFactory.TrigEFTauMVHypoTool(name)

        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool('MonTool_' + name)
        monTool.HistPath = 'TrigTauRecMerged_TrigEFTauMVHypo/' + name

        # define quantities to be monitored
        monTool.defineHistogram("CutCounter", path='EXPERT',type='TH1I',title=';CutCounter; Entries', xbins=10, xmin=0.,xmax=10.) 
        monTool.defineHistogram("ptAccepted", path='EXPERT',type='TH1F',title=';ptAccepted; Entries', xbins=50, xmin=0.,xmax=500.)
        monTool.defineHistogram("nTrackAccepted", path='EXPERT',type='TH1F',title=';nTrackAccepted; Entries', xbins=10, xmin=0.,xmax=10.)
        monTool.defineHistogram("nWideTrackAccepted", path='EXPERT',type='TH1F',title=';nWideTrackAccepted; Entries', xbins=10, xmin=0.,xmax=10.)       
        monTool.defineHistogram("nInputTaus", path='EXPERT',type='TH1F',title=';nInputTaus; Entries', xbins=10, xmin=0.,xmax=10.) 
        currentHypo.MonTool = monTool

 
        # setup the Hypo parameter
        theThresh = thresholdsEF[(criteria, int(threshold))]
        currentHypo.numTrackMax = theThresh.numTrackMax
        currentHypo.numTrackWideTrackMax = theThresh.numTrackWideTrackMax
        currentHypo.EtCalibMin  = theThresh.EtCalibMin
        currentHypo.level       = theThresh.level
        currentHypo.method      = 2
        
        if criteria in [ 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN' ]:
            currentHypo.numTrackMin = 0
            currentHypo.highptidthr = 280000.
            currentHypo.method      = 3
        elif 'idperf' in criteria: 
            currentHypo.AcceptAll = True
        elif 'perf' in criteria:
            currentHypo.method      = 0

    elif criteria in [ 'dikaonmass', 'kaonpi1', 'kaonpi2', 'dipion1', 'dipion2', 'dipion3', 'dipion4', 'singlepion' ]: # ATR-22644
        currentHypo = CompFactory.TrigEFTauDiKaonHypoTool(name)
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool('MonTool_' + name)
        monTool.HistPath = 'ComboHypo/' + name.replace("leg001_","")

        monTool.defineHistogram('massTrkSysAccepted',path='EXPERT',type='TH1F',title=';DiPion Mass [GeV]; Entries', xbins=50, xmin=0.,xmax=3.)
        monTool.defineHistogram('massTrkSysKaonAccepted',path='EXPERT',type='TH1F',title=';DiKaon Mass [GeV]; Entries', xbins=50, xmin=0.,xmax=3.)
        monTool.defineHistogram('massTrkSysKaonPiAccepted',path='EXPERT',type='TH1F',title=';KaonPion Mass [GeV]; Entries', xbins=50, xmin=0.,xmax=3.)
        monTool.defineHistogram('leadTrkPtAccepted',path='EXPERT',type='TH1F',title=';Leading Track p_{T} [GeV]; Entries', xbins=50, xmin=0.,xmax=300.)
        monTool.defineHistogram('ptAccepted',path='EXPERT',type='TH1F',title=';p_{T}(#tau) [GeV]; Entries', xbins=50, xmin=0.,xmax=300.)
        monTool.defineHistogram('nTrackAccepted',path='EXPERT',type='TH1F',title=';nTracks; Entries', xbins=10, xmin=-0.5,xmax=9.5)
        monTool.defineHistogram('nWideTrackAccepted',path='EXPERT',type='TH1F',title=';nWideTracks; Entries', xbins=10, xmin=-0.5,xmax=9.5)
        monTool.defineHistogram('dRAccepted',path='EXPERT',type='TH1F',title=';#DeltaR^{max}; Entries ', xbins=40, xmin=0.,xmax=0.4)
        monTool.defineHistogram('etOverPtLeadTrkAccepted',path='EXPERT',type='TH1F',title=';etOverPtLeadTrk; Entries', xbins=50, xmin=0.,xmax=5.)
        monTool.defineHistogram('EMOverTrkSysPAccepted',path='EXPERT',type='TH1F',title=';EMOverTrkSysP; Entries', xbins=50, xmin=0.,xmax=5.)  
        monTool.defineHistogram('nInputTaus',path='EXPERT',type='TH1F',title=';nInputTaus; Entries', xbins=10, xmin=-0.5,xmax=9.5)  

        currentHypo.MonTool       = monTool

        if criteria in [ 'dikaonmass', 'kaonpi1', 'kaonpi2', 'dipion1', 'dipion2', 'dipion3', 'dipion4']:
            theThresh = thresholdsEF_dikaon[(criteria, int(threshold))]
            currentHypo.massTrkSysMin          = theThresh.massTrkSysMin          
            currentHypo.massTrkSysMax          = theThresh.massTrkSysMax          
            currentHypo.massTrkSysKaonMin      = theThresh.massTrkSysKaonMin      
            currentHypo.massTrkSysKaonMax      = theThresh.massTrkSysKaonMax      
            currentHypo.massTrkSysKaonPiMin    = theThresh.massTrkSysKaonPiMin    
            currentHypo.massTrkSysKaonPiMax    = theThresh.massTrkSysKaonPiMax    
            currentHypo.targetMassTrkSysKaonPi = theThresh.targetMassTrkSysKaonPi 
            currentHypo.leadTrkPtMin           = theThresh.leadTrkPtMin
            currentHypo.EtCalibMin             = theThresh.EtCalibMin 
            currentHypo.EMPOverTrkSysPMax      = theThresh.EMPOverTrkSysPMax      
            
        elif criteria in ['singlepion']:
            theThresh = thresholdsEF_singlepion[(criteria, int(threshold))]
            currentHypo.leadTrkPtMin       = theThresh.leadTrkPtMin
            currentHypo.EtCalibMin         = theThresh.EtCalibMin 
            currentHypo.nTrackMax          = theThresh.nTrackMax              
            currentHypo.nWideTrackMax      = theThresh.nWideTrackMax          
            currentHypo.dRmaxMax           = theThresh.dRmaxMax               
            currentHypo.etOverPtLeadTrkMin = theThresh.etOverPtLeadTrkMin     
            currentHypo.etOverPtLeadTrkMax = theThresh.etOverPtLeadTrkMax     
        
    return currentHypo

def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']
    from AthenaConfiguration.ComponentFactory import CompFactory
    currentHypo = CompFactory.TrigTrackPreSelHypoTool(name)

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool_' + name)
    monTool.HistPath = 'TrigTauRecMerged_TrigTrackPreSelHypo/' + name

    # define quantities to be monitored
    monTool.defineHistogram("nTracksInCore", path='EXPERT', type='TH1I',title=';nTracksInCore; Entries', xbins=10, xmin=0.,xmax=10.)
    monTool.defineHistogram("nTracksInIso",  path='EXPERT', type='TH1I',title=';nTracksInIso; Entries',  xbins=10, xmin=0.,xmax=10.)
    monTool.defineHistogram("CutCounter",   path='EXPERT',  type='TH1I',title=';CutCounter; Entries',    xbins=10, xmin=0.,xmax=10.)
    currentHypo.MonTool = monTool

    if criteria == 'cosmic':
      currentHypo.LowerPtCut      = int(threshold)*1000.
      currentHypo.TracksInCoreCut = 9999
      currentHypo.TracksInIsoCut  = 9999
      currentHypo.DeltaZ0Cut      = 9999.      

    return currentHypo

def TrigTrkPrecHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from AthenaConfiguration.ComponentFactory import CompFactory
    currentHypo = CompFactory.TrigTrkPrecHypoTool(name)
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

    threshold = chainPart['threshold']
    from AthenaConfiguration.ComponentFactory import CompFactory
    currentHypo = CompFactory.TrigTauGenericHypo(name)
    currentHypo.MonTool  = ""
    currentHypo.Details  = [int(-1)]
    currentHypo.Formulas = ['y > '+threshold+'*1000.0']

    return currentHypo
