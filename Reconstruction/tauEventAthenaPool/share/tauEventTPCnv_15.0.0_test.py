infile = 'aod/AOD-15.0.0/AOD-15.0.0-full.pool.root'
keys = [
    #TauJetContainer_p3
    'HLT_TrigTauRecCalo',
    'AtlfastTauJetContainer',
    'AtlfastTauJet1p3pContainer',
    'HLT_TrigTauRecMerged',
    'TauRecContainer',

    #TauDetailsContainer_tlp1
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
