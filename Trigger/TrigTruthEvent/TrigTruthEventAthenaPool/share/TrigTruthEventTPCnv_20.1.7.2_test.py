infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #TrigInDetTrackTruthMap
    'TrigInDetTrackTruthMap',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

