# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
###########################################################################
# Job options to test HLT conditions updates based on local sqlite file
#
# To be used to test HLT conditions updates (in sqlite files) before
# deployment in the online DB.
#
# Usage:
#  athena.py -c "sqlite='noise.db'" TriggerJobOpts/test_hltConditions.py
#  - Other options can be found below
#
# Author: Frank Winklmeier
###########################################################################


# Update these values to your need:
if 'sqlite' not in dir():
   sqlite = 'noise.db'
if 'onldb' not in dir():
   onldb = 'ATLAS_COOLONL_CALO'
if 'folder' not in dir():
   folder = '/CALO/Noise/CellNoise'

# No updates required past this point
testCurrentMenu=True
EvtMax=10
BSRDOInput='/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/data16_13TeV.00307126.physics_eb_zmm_egz.merged.RAW.selected._0001.data'

from AthenaCommon.Include import include
myfolder = folder  # 'folder' seems to be overwritten somewhere in the include
include('TriggerJobOpts/runHLT_standalone.py')
folder = myfolder

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from IOVDbSvc.CondDB import conddb  # noqa: F401 configuration by import, old Run-2 job options
svcMgr.IOVDbSvc.forceRunNumber = 9999999

sqlite_tmp = 'cool_tmp.db'
tag = svcMgr.IOVDbSvc.GlobalTag
conddb.addMarkup(folder,'<db>sqlite://;schema=%s;dbname=CONDBR2</db>' % sqlite_tmp)

import os
import sys
# Delete temporary file
try:
   os.remove(sqlite_tmp)
except Exception:
   pass

# Copy folder from online DB
rc = os.system("AtlCoolCopy 'oracle://ATLAS_COOLPROD;schema=%s;dbname=CONDBR2' 'sqlite://;schema=%s;dbname=CONDBR2' -create -tag %s -hitag -prunetags -folder %s -r 9999999" % (onldb,sqlite_tmp,tag,folder))
if rc!=0:
   sys.exit(rc)

# Merge with user given sqlite file (in case only some channels have been updated)
rc = os.system("AtlCoolCopy 'sqlite://;schema=%s;dbname=CONDBR2' 'sqlite://;schema=%s;dbname=CONDBR2' -folder %s -r 9999999" % (sqlite,sqlite_tmp,folder))
if rc!=0:
   sys.exit(rc)
