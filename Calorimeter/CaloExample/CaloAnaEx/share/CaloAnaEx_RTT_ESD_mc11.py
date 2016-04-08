
#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolRDOInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/RDO.541908._000015.pool.root.1'])


from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteESD=True
doWriteAOD=False
doWriteTAG=False

doAOD=False
doAODLVL1=False
doTrigger=False
doMissingET=False


# number of event to process
#EvtMax=-1
EvtMax=10

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = ESD_mc11_top\">&MYRTTINFOS.txt"
os.system(com)
