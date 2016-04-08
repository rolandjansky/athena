#
# RDO to BS
#

from RecExConfig.RecFlags import rec
rec.doEgamma=False

doTrigger=False

doESD=False
doCBNT=False
doWriteRDO=False
doESD=False
doWriteESD=False
doWriteAOD=False
doAOD=False
doWriteTAG=False

doWriteBS=True

#BSRDOOuput='AppName=Offline'
#BSRDOOuput='test.data'

EvtMax=-1

include ("RecExCommon/RecExCommon_flags.py")

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')

include ("RecExCommon/RecExCommon_topOptions.py")

#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/RTTdata/calib0_csc11.007062.singlepart_gamma_E50.digit.RDO.v12000301_tid003215._00001.pool.root'])

#if not 'BSRDOInput' in dir():
#    BSRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.lumi1E31.latest.LB0000.Athena._0001.Lvl2Result.data"]


#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = Photons_calib0_csc11_007062_singlepart_gamma_E50_BStoESD\">&MYRTTINFOS.txt"
os.system(com)
