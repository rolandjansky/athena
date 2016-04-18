infile = 'aod/AOD-15.6.9/AOD-15.6.9-full.pool.root'
keys = [
    #TauJetContainer_p3
    'HLT_TrigTauRecCalo',
    'AtlfastTauJetContainer',
    'HLT_TrigTauRecMerged',
    'TauRecContainer',

    #TauDetailsContainer_tlp2
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
