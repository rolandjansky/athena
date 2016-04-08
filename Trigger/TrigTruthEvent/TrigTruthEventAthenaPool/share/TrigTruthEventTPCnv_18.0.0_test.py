infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #TrigInDetTrackTruthMap_tlp2
    'TrigInDetTrackTruthMap',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

