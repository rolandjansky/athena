
#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolRDOInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc12/mc12_14TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4033_tid01014997_00/RDO.01014997._000011.pool.root.1'])


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

globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-20-00-01')
globalflags.ConditionsTag = "OFLCOND-MC12-SDR-06"

# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = ESD_mc12_top\">&MYRTTINFOS.txt"
os.system(com)
