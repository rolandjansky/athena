# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def TrigL2TauHypoToolFromDict( chainDict ):

    name = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    part='calo'
    threshold='25' # chainPart['threshold'] else not quite testing anything, leaving to experts
    criteria= 'medium1' # likely chainPart['selection']
    strategy='tracktwo' # do not know which chain property maps here, experts need to look
    print "TrigL2TauHypoTool: name = ", name

    # Simple implementation of 2015 pre-selection
    #currentHypoKey = 'l2'+part+'_tau'+threshold+'_'+criteria+'_'+strategy

    # Re-define the calo part using the generic hypo
    if part == 'calo':
       from TrigTauHypo.TrigTauHypoConf import TrigTauGenericHypoMT
       currentHypo = TrigTauGenericHypoMT(name)
       currentHypo.MonTool = ""

       # pT cut always defined: ugly string-to-int-to-string conversion, sorry :)
       myThreshold = str(int(threshold)*1000.0)
       theDetails  = [int(-1)]
       theFormulas = ['y > '+myThreshold]

       if strategy =='calo' or strategy == 'caloonly' or strategy == 'tracktwocalo' or strategy == 'trackcalo':
          # centFrac cut (detail #24: 2nd order fit, turn-off at ~ 55 GeV, 95% efficiency)
          theDetails += [24]
          theFormulas += ['x > (0.945 - (1.26e-05*TMath::Min(y, 50000.)) + (1.05e-10*TMath::Min(y, 50000.)*TMath::Min(y, 50000.)))']
          # centFrac cut (detail #24: 2nd order fit, turn-off at 50 GeV, 90% efficiency)
          # theFormulas += ['x > TMath::Max(0.936 - (8.5e-06*y) + (6.54e-11*y*y), 0.660)']
       currentHypo.Details = theDetails
       currentHypo.Formulas = theFormulas

    return currentHypo


def TrigTauHypoProvider( name, conf ):
    from AthenaCommon.Constants import DEBUG
    """ Configure a b-jet hypo tool from chain name. """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName

    decoder = DictFromChainName()
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info
    decodedDict['chainName'] = name # override

    return TrigL2TauHypoToolFromDict( decodedDict )
