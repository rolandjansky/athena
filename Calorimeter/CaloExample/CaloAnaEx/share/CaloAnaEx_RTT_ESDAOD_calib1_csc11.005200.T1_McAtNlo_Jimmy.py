#

import os
print 'get_files checkFile.py'
os.system('get_files checkFile.py')

from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteESD=True
doWriteTAG=False
doWriteAOD=True

doTrackRecordFilter=False
doTrigger=False
doAODLVL1=False

doMissingET=False

doCBNT=False

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ParticleJet=False
AODFlags.FastSimulation=False

EvtMax=-1

include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
#DetDescrVersion="ATLAS-CSC-01-02-00"

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = top_ESDAOD_calib1_csc11.005200.T1_McAtNlo_Jimmy\">&MYRTTINFOS.txt"
os.system(com)
