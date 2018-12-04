
def TrigTauHypoProvider( name, conf ):
    part='calo'
    threshold='25'
    criteria='medium1'
    strategy='tracktwo'

    print "TrigL2TauHypoTool: name = ", name

    # Simple implementation of 2015 pre-selection
    currentHypoKey = 'l2'+part+'_tau'+threshold+'_'+criteria+'_'+strategy

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
