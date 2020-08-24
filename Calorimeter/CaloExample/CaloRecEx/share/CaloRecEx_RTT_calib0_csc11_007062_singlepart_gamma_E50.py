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
#obsolte doAODLVL1=False
#doTruth=False

recAlgs.doMissingET=False


rec.CBNTAthenaAware=True 

doTopoClusterCBNT=True


# number of event to process
jp.AthenaCommonFlags.EvtMax=-1
#EvtMax=4

include ("RecExCond/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

#obsolete jobproperties.Global.DetDescrVersion='ATLAS-CSC-01-00-00'

#PoolRDOInput= ["/castor/cern.ch/atlas/project/dc2/preprod/g4dig805/dc2.002885.pyt_z_ee.g4dig805/data/dc2.002885.pyt_z_ee.g4dig805._0001.pool.root"]
#PoolRDOInput=["/castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/rtt/RTTdata/calib0_csc11.007062.singlepart_gamma_E50.digit.RDO.v12000301_tid003215._00001.pool.root"]

# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = Photons_calib0_csc11_007062_singlepart_gamma_E50'

import os
com="echo \"BEGIN_RTTINFO = Photons_calib0_csc11_007062_singlepart_gamma_E50\">&MYRTTINFOS.txt"
os.system(com)
