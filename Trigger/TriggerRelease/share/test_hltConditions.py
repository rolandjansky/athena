###########################################################################
# Job options to test HLT conditions updates based on local sqlite file
#
# To be used to test HLT conditions updates (in sqlite files) before
# deployment in the online DB.
#
# Usage:
#  athena.py -c "sqlite='noise.db'" TriggerRelease/test_hltConditions.py
#  - Other options can be found below
#
# Author: Frank Winklmeier
###########################################################################

# Update these values to your need:
if not 'sqlite' in dir(): sqlite = 'noise.db'
if not 'onldb' in dir(): onldb = 'ATLAS_COOLONL_CALO'
if not 'folder' in dir(): folder = '/CALO/Noise/CellNoise'

# No updates required past this point
testCurrentMenu=True
EvtMax=10
BSRDOInput='root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data15_cos.00251876.physics_CosmicMuons.merge.RAW._lb0240._SFO-ALL._0001.1'
include('TriggerRelease/runHLT_standalone.py')

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.forceRunNumber = 9999999

sqlite_tmp = 'cool_tmp.db'
tag = svcMgr.IOVDbSvc.GlobalTag
conddb.addMarkup(folder,'<db>sqlite://;schema=%s;dbname=CONDBR2</db>' % sqlite_tmp)

import os, sys
# Delete temporary file
try:
   os.remove(sqlite_tmp)
except:
   pass

# Copy folder from online DB
rc = os.system("AtlCoolCopy.exe 'oracle://ATLAS_COOLPROD;schema=%s;dbname=CONDBR2' 'sqlite://;schema=%s;dbname=CONDBR2' -create -tag %s -hitag -prunetags -folder %s -r 9999999" % (onldb,sqlite_tmp,tag,folder))
if rc!=0: sys.exit(rc)

# Merge with user given sqlite file (in case only some channels have been updated)
rc = os.system("AtlCoolCopy.exe 'sqlite://;schema=%s;dbname=CONDBR2' 'sqlite://;schema=%s;dbname=CONDBR2' -folder %s -r 9999999" % (sqlite,sqlite_tmp,folder))
if rc!=0: sys.exit(rc)
