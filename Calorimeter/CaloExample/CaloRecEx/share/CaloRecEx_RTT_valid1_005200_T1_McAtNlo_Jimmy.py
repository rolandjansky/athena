import os

print ('Hostname :')
os.system('hostname')
print ('CPU infos :')
os.system('cat /proc/cpuinfo')
print ('MEM infos :')
os.system('cat /proc/meminfo')


#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/RecExCommon_flags.py for more available flags
#

# readG3 and AllAlgs needs be set before the include, since several
# secondary flags are configured according to that one
#
# readG3=True # true if read g3 data
# AllAlgs = False # if false, all algorithms are switched off by defaults 
# doiPatRec = False

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
rec.doEgamma=False

rec.doWriteESD=False
rec.doWriteTAG=False
rec.doWriteAOD=False

recAlgs.doTrackRecordFilter=False
rec.doTrigger=False
rec.doAOD=False
#obsolete doAODLVL1=False
#doTruth=False


doMuid=False
recAlgs.doMuGirl=False
doMuonIdCombined=False
doMuonIdStandalone=False
recAlgs.doMuTag=False


rec.CBNTAthenaAware=True 
rec.doCBNT=True
doTopoClusterCBNT=True

# number of event to process
jp.AthenaCommonFlags.EvtMax=-1
#EvtMax=2

recAlgs.doMissingET=False

include ("RecExCond/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()


#jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-02-00"
#jobproperties.Global.DetDescrVersion="ATLAS-GEO-02-01-00"
#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')


# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = top_valid1_005200_T1_McAtNlo_Jimmy'
import os
com="echo \"BEGIN_RTTINFO = top_valid1_005200_T1_McAtNlo_Jimmy\">&MYRTTINFOS.txt"
os.system(com)



from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc()

jobproperties.PerfMonFlags.doDetailedMonitoring = True


import os
os.system('get_files perfmon.py')
