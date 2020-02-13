# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2TauHypoTool')


def TrigEFTauMVHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    from TrigTauHypo.TrigTauHypoConf import TrigEFTauMVHypoTool
    currentHypo = TrigEFTauMVHypoTool(name)
    currentHypo.level = 1
    currentHypo.method = 3
    currentHypo.MonTool = ""

    return currentHypo

def TrigTauTrackHypoToolFromDict( chainDict ):

    name = chainDict['chainName']

    from TrigTauHypo.TrigTauHypoConf import TrigTrackPreSelHypoTool
    currentHypo = TrigTrackPreSelHypoTool(name)
    currentHypo.MonTool = ""

    return currentHypo


def TrigL2TauHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    part='calo'
    threshold=chainPart['threshold']
    strategy=chainPart['preselection']

    # Simple implementation of 2015 pre-selection
    #currentHypoKey = 'l2'+part+'_tau'+threshold+'_'+criteria+'_'+strategy

    # Re-define the calo part using the generic hypo
    if part == 'calo':
       from TrigTauHypo.TrigTauHypoConf import TrigTauGenericHypoMT
       currentHypo = TrigTauGenericHypoMT(name)
       currentHypo.MonTool = ""
       currentHypo.AcceptAll = False
       if 'idperf' in name:
          currentHypo.AcceptAll = True

       # pT cut always defined: ugly string-to-int-to-string conversion, sorry :)
       myThreshold = str(int(threshold)*1000.0)
       theDetails  = [int(-1)]
       theFormulas = ['y > '+myThreshold]

       if strategy in [ 'calo', 'ptonly', 'track', 'tracktwo', 'tracktwoEF', 'tracktwoMVA', 'FTK', 'FTKRefit', 'FTKNoPrec' , 'tracktwoEFmvaTES' ]:
          # centFrac cut (detail #24: 2nd order fit, turn-off at ~ 55 GeV, 95% efficiency)
          theDetails += [24]
          theFormulas += ['x > (0.945 - (1.26e-05*TMath::Min(y, 50000.)) + (1.05e-10*TMath::Min(y, 50000.)*TMath::Min(y, 50000.)))']
          # centFrac cut (detail #24: 2nd order fit, turn-off at 50 GeV, 90% efficiency)
          # theFormulas += ['x > TMath::Max(0.936 - (8.5e-06*y) + (6.54e-11*y*y), 0.660)']
       currentHypo.Details = theDetails
       currentHypo.Formulas = theFormulas

    return currentHypo
