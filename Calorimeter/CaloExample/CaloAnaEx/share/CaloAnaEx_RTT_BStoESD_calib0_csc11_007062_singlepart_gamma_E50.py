#
#
#

import os
print 'get_files checkFile.py'
os.system('get_files checkFile.py')

#
# BS to ESD
#

EvtMax=-1

if not 'BSRDOInput' in dir():
    BSRDOInput=["data_test.00007062.Single_Stream.daq.RAW._lb0000._Athena._0001.data"]
#    BSRDOInput=["data_test.00005200.Single_Stream.daq.RAW._lb0000._Athena._0001.data"]

from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteESD=True
doWriteRDO=False
doWriteAOD=False 
doAOD=False 
doWriteTAG=False 

# Read from ByteStream
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat="bytestream"
GlobalFlags.ConditionsTag="OFLCOND-CSC-00-01-00"

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')

include ("RecExCommon/RecExCommon_topOptions.py")

readBS=True

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

# hack starts here:

class DummyEventSelector   :
    InputCollections= []

class DummyServiceMgr  :
    EventSelector= DummyEventSelector

#create a dummy ServiceMgr
ServiceMgr=DummyServiceMgr()

# hack ends here
