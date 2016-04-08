infile = 'aod/AOD-15.8.0/AOD-15.8.0-full.pool.root'
keys = [
    #TrigInDetTrackTruthMap_tlp1
    'TrigInDetTrackTruthMap',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

