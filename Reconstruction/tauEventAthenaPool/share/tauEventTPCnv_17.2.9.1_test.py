infile = 'aod/AOD-17.2.9.1/AOD-17.2.9.1-full.pool.root'
keys = [
    #TauJetContainer_p4
    'HLT_TrigTauRecCalo',
    'HLT_TrigTauRecMerged',
    'TauRecContainer',

    #TauDetailsContainer_tlp3
    # Dumped as part of the above.
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

include ('AthenaPoolUtilities/TPCnvTest.py')
