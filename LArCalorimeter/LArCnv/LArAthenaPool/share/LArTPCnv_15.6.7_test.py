infile = 'esd/ESD-15.6.7-data.pool.root'
keys = [
    #LArDigitContainer_p1
    'LArDigitContainer_EMClust',
    'LArDigitContainer_Thinned',

    #LArNoisyROSummary_p1
    'LArNoisyROSummary',
         ]

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value('data')

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

# Force using DBRelease for offline test builds.
import os
if os.environ['SITEROOT'] == '/xyzzy':
    svcMgr.DBReplicaSvc.COOLSQLiteVetoPattern=''
