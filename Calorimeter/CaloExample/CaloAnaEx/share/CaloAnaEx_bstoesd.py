# steering file for BS->ESD step
# see myTopOptions.py for more info
#doCBNT=False

if not 'BSRDOInput' in dir():
    jp.AthenaCommonFlags.BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.lumi1E31.Lvl2Result.latest.data"]

if not 'doWriteESD' in dir():
    rec.doWriteESD=True

rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 

#doTrigger=False
#-------
# Read from ByteStream
#------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat="bytestream"
GlobalFlags.ConditionsTag="OFLCOND-CSC-00-01-00"

#readBS=True

#include ("RecExCommon/RecExCommon_flags.py")
# should be done afterwards so that TriggerFlags are configured ok

# has been run at RDO->BS step (even EF ?)
##doTrigger=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
