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
rec.doEgamma=False

doWriteESD=False
doWriteTAG=False
doWriteAOD=False

doTrackRecordFilter=False
doTrigger=False
doAOD=False
doAODLVL1=False
#doTruth=False

CBNTAthenaAware=True 

doTopoClusterCBNT=True

# number of event to process
EvtMax=-1
#EvtMax=2

doMissingET=False

include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

#obsolete jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-02-00"

# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = Photons_calib1_csc11_007063_singlepart_gamma_E100'

import os
com="echo \"BEGIN_RTTINFO = Photons_calib1_csc11_007063_singlepart_gamma_E100\">&MYRTTINFOS.txt"
os.system(com)
