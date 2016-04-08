#
# from ESD to BS
#

#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolESDInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/ESD.541908._000015_200ev.pool.root.1'])
rec.readESD.set_Value_and_Lock(True)

from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteBS=True
doWriteBSRDO=True
BSRDOOuput='AppName=Offline'
BSRDOOuput='test.data'

doWriteESD=False
doWriteAOD=False
doWriteTAG=False

doAOD=False
doESD=False
doTrigger=False

# number of event to process
#EvtMax=-1
EvtMax=15

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

include ("RecExCommon/RecExCommon_topOptions.py")

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/RTTdata/calib0_csc11.007062.singlepart_gamma_E50.digit.RDO.v12000301_tid003215._00001.pool.root'])
#if not 'BSRDOInput' in dir():
#    BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.latest.LB0000.Athena._0001.Lvl2Result.data"]


#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = ESDtoBS_mc11_top\">&MYRTTINFOS.txt"
os.system(com)
