infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #TauJetContainer_p5
    'HLT_TrigTauRecCalo',
    'HLT_TrigTauRecMerged',
    'TauRecContainer',

    #TauDetailsContainer_tlp4
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
