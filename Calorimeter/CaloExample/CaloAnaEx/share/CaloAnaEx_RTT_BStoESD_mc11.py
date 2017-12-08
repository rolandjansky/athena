import os
print 'get_files checkFile.py'
os.system('get_files checkFile.py')

#
# from BS to ESD
#
##if not 'BSRDOInput' in dir():
#    BSRDOInput=["data_test.00007062.Single_Stream.daq.RAW._lb0000._Athena._0001.data"]
#    BSRDOInput=["data_test.00005200.Single_Stream.daq.RAW._lb0000._Athena._0001.data"] 
##BSRDOInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/RTTdata/calib0_csc11.007062.singlepart_gamma_E50.digit.RDO.v12000301_tid003215._00001.pool.root"]

#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolBSRDOInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/RDO.541908._000015.pool.root.1'])
#rec.readBSRDO.set_Value_and_Lock(True)


#if not 'BSRDOInput' in dir():    
#    BSRDOINPUT=["/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/RDO.541908._000015.pool.root.1"]

#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloAnaEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1272_s1274_d580_tid541908_00/RDO.541908._000015.pool.root.1'])

from RecExConfig.RecFlags import rec
rec.doEgamma=False

doWriteESD=True
doWriteAOD=False
doWriteTAG=False
doWriteRDO=False

doAOD=False 

doMissingET=False

#EvtMax=-1
EvtMax=5


# Read from ByteStream
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat="bytestream"
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
#GlobalFlags.ConditionsTag="OFLCOND-SDR-BS7T-05-14"

include ("RecExCommon/RecExCommon_flags.py")
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
#globalflags.ConditionsTag="OFLCOND-SDR-BS7T-05-14"

#globalflags.InputFormat="bytestream"
#default pool

include ("RecExCommon/RecExCommon_topOptions.py")

readBS=True

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

import os
com="echo \"BEGIN_RTTINFO = BStoESD_mc11_top\">&MYRTTINFOS.txt"
os.system(com)

