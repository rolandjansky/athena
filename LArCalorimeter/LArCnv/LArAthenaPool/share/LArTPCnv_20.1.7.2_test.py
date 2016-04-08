infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #LArNoisyROSummary_p3
    'LArNoisyROSummary',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

