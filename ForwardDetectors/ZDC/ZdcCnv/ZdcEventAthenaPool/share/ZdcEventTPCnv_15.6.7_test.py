infile = 'esd/ESD-15.6.7-data.pool.root'
keys = [
    #ZdcDigitsCollection_p1
    'ZdcDigitsCollection'
         ]

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value('data')

noMuon = True
noID = True
include ('AthenaPoolUtilities/TPCnvTest.py')

# Force using DBRelease for offline test builds.
import os
if os.environ['SITEROOT'] == '/xyzzy':
    svcMgr.DBReplicaSvc.COOLSQLiteVetoPattern=''
