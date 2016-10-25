#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/RecExCommon_flags.py for more available flags
#
from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteESD=False
doWriteTAG=False
doWriteAOD=False

doTrackRecordFilter=False
doTrigger=False
doAOD=False
doAODLVL1=False

EvtMax=-1

PoolRDOInput= ["Dig.pool.root"]
doTopoClusterCBNT=True

include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-06-00-00'

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = Electrons100GeV\">&MYRTTINFOS.txt"
os.system(com)
