infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #BCM_RDO_Container_p0
    'BCM_CompactDOs',
    'BCM_RDOs',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

