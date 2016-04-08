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

doWriteESD=True
doWriteTAG=False
doWriteAOD=False

doTrigger=False
doAOD=False
doAODLVL1=False

# number of event to process
EvtMax=-1
#EvtMax=10

doMissingET=False

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

RootNtupleOutput="ntuple_ref.root"

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = Photons_calib0_csc11_007062_singlepart_gamma_E50_ESDAOD\">&MYRTTINFOS.txt"
os.system(com)
