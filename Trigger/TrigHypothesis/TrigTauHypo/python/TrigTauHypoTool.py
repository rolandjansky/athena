# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2TauHypoTool')

from collections import namedtuple


# Here we need a large repository of configuration values
# The meaning of the configuration values is as follows:
# ('Id working point', 'pt threshold': ['Maximum number of tracks[0]', 'etmincalib[1]', 'Id level[2]'])
TauCuts = namedtuple('TauCuts','numTrackMax numTrackWideTrackMax EtCalibMin level')
thresholdsEF = {
    ('looseRNN', 20): TauCuts(3, 1, 20000.0, 1),
    ('looseRNN', 25): TauCuts(3, 1, 25000.0, 1),
    ('looseRNN', 35): TauCuts(3, 1, 35000.0, 1),
    ('looseRNN', 40): TauCuts(3, 1, 40000.0, 1),
    ('looseRNN', 60): TauCuts(3, 1, 60000.0, 1),
    ('looseRNN', 80): TauCuts(3, 1, 80000.0, 1), 
    ('looseRNN', 160): TauCuts(3, 1, 160000.0, 1),
    ('looseRNN', 200): TauCuts(3, 1, 200000.0, 1),
    ('mediumRNN', 20): TauCuts(3, 1, 20000.0, 2),
    ('mediumRNN', 25): TauCuts(3, 1, 25000.0, 2),
    ('mediumRNN', 35): TauCuts(3, 1, 35000.0, 2),
    ('mediumRNN', 40): TauCuts(3, 1, 40000.0, 2),
    ('mediumRNN', 50): TauCuts(3, 1, 50000.0, 2),
    ('mediumRNN', 60): TauCuts(3, 1, 60000.0, 2),
    ('mediumRNN', 80): TauCuts(3, 1,  80000.0, 2),
    ('mediumRNN', 100): TauCuts(3, 1, 100000.0, 2),
    ('mediumRNN', 160): TauCuts(3, 1, 160000.0, 2), 
    ('mediumRNN', 180): TauCuts(3, 1, 180000.0, 2), 
    ('mediumRNN', 200): TauCuts(3, 1, 200000.0, 2),
    ('tightRNN', 20): TauCuts(3, 1, 20000.0, 3),
    ('tightRNN', 25): TauCuts(3, 1, 25000.0, 3),
    ('tightRNN', 35): TauCuts(3, 1, 35000.0, 3),
    ('tightRNN', 40): TauCuts(3, 1, 40000.0, 3),
    ('tightRNN', 60): TauCuts(3, 1, 60000.0, 3),
    ('tightRNN', 80): TauCuts(3, 1, 80000.0, 3),
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

def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    chainPart = chainDict['chainParts'][0]

    criteria  = chainPart['selection']
    threshold = chainPart['threshold']

    from AthenaConfiguration.ComponentFactory import CompFactory
    if criteria in ['verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN', 'idperf', 'perf'] :
    
        currentHypo = CompFactory.TrigEFTauMVHypoTool(name)

        if 'tauMon:online' in chainDict['monGroups']:
           from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
           monTool = GenericMonitoringTool('MonTool_' + name)
           monTool.HistPath = 'TrigTauRecMerged_TrigEFTauMVHypo/' + name

           # define quantities to be monitored
           monTool.defineHistogram("CutCounter", path='EXPERT',type='TH1I',title=';CutCounter; Entries', xbins=10, xmin=0.,xmax=10.) 
           monTool.defineHistogram("ptAccepted", path='EXPERT',type='TH1F',title=';ptAccepted; Entries', xbins=50, xmin=0.,xmax=500.)
           monTool.defineHistogram("nTrackAccepted", path='EXPERT',type='TH1F',title=';nTrackAccepted; Entries', xbins=10, xmin=0.,xmax=10.)
           monTool.defineHistogram("nWideTrackAccepted", path='EXPERT',type='TH1F',title=';nWideTrackAccepted; Entries', xbins=10, xmin=0.,xmax=10.)       
           monTool.defineHistogram("nInputTaus", path='EXPERT',type='TH1F',title=';nInputTaus; Entries', xbins=10, xmin=0.,xmax=10.) 
           monTool.defineHistogram("RNNJetScoreAccepted_0p", path='EXPERT',type='TH1F',title=';RNN score Accepted for 0 prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           monTool.defineHistogram("RNNJetScoreSigTransAccepted_0p", path='EXPERT',type='TH1F',title=';RNN score sig trans Accepted for 0 prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           monTool.defineHistogram("RNNJetScoreAccepted_1p", path='EXPERT',type='TH1F',title=';RNN score Accepted for 1 prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           monTool.defineHistogram("RNNJetScoreSigTransAccepted_1p", path='EXPERT',type='TH1F',title=';RNN score sig trans Accepted for 1 prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           monTool.defineHistogram("RNNJetScoreAccepted_mp", path='EXPERT',type='TH1F',title=';RNN score Accepted for multi prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           monTool.defineHistogram("RNNJetScoreSigTransAccepted_mp", path='EXPERT',type='TH1F',title=';RNN score sig trans Accepted for multi prong taus; Entries', xbins=40, xmin=0.,xmax=1.)
           currentHypo.MonTool = monTool

 
        # setup the Hypo parameter
        theThresh = thresholdsEF[(criteria, int(threshold))]
        currentHypo.numTrackMax = theThresh.numTrackMax
        currentHypo.numTrackWideTrackMax = theThresh.numTrackWideTrackMax
        currentHypo.EtCalibMin  = theThresh.EtCalibMin
        currentHypo.level       = theThresh.level
        currentHypo.method      = 1   
     
        if criteria in [ 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN' ]:
            currentHypo.highptidthr = 280000.
            currentHypo.method      = 1
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

    if 'tauMon:online' in chainDict['monGroups']:
       from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
       monTool = GenericMonitoringTool('MonTool_' + name)
       monTool.HistPath = 'TrigTauRecMerged_TrigTrackPreSelHypo/' + name

       # define quantities to be monitored
       monTool.defineHistogram("nTracksInCore", path='EXPERT', type='TH1I',title=';nTracksInCore; Entries', xbins=10, xmin=0.,xmax=10.)
       monTool.defineHistogram("nTracksInIso",  path='EXPERT', type='TH1I',title=';nTracksInIso; Entries',  xbins=10, xmin=0.,xmax=10.)
       monTool.defineHistogram("CutCounter",   path='EXPERT',  type='TH1I',title=';CutCounter; Entries',    xbins=10, xmin=0.,xmax=10.)
       currentHypo.MonTool = monTool

    currentHypo.AcceptAll = True

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
