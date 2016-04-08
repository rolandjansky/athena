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


from RecExConfig.RecFlags  import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecAlgsFlags import recAlgs

rec.readRDO.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(False)
tuple_name = "ntuple.root"

rec.AutoConfiguration=['everything']
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)


from RecExConfig.RecAlgsFlags import recAlgs
from MuonRecExample.MuonRecFlags import muonRecFlags
rec.doEgamma=False

recAlgs.doTrackRecordFilter=False
rec.doTrigger=False

doMuid=False
recAlgs.doMuGirl=False
doMuonIdCombined=False
doMuonIdStandalone=False
recAlgs.doMuTag=False


# number of event to process
athenaCommonFlags.EvtMax.set_Value_and_Lock(1000)
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

# D3PDMaker calo block

from D3PDMakerCoreComps.MakerAlg import *
from CaloD3PDMaker.ClusterD3PDObject import *
from CaloD3PDMaker.CaloCellD3PDObject import *
from CaloD3PDMaker.CaloInfoD3PDObject import *

# switch off?? LArDigit JT. 09.10.2012
# from CaloD3PDMaker.LArDigitD3PDObject import *

from EventCommonD3PDMaker.EventInfoD3PDObject import *

alg = MakerAlg("caloD3PD", topSequence, file = tuple_name , D3PDSvc = 'D3PD::RootD3PDSvc')
alg += EventInfoD3PDObject (1)

# alg += AllCaloCellD3PDObject (10)
#alg += SelCaloCellD3PDObject (Param)
# alg += EMCellD3PDObject (10)
# alg += HECCellD3PDObject (10)
# alg += FCALCellD3PDObject (10)
# alg += TileCellD3PDObject (10)

alg += CaloInfoD3PDObject (1)

alg += ClusterD3PDObject (2)

# switch off LArDigitObj JT 09.10.2012
#alg += LArDigitD3PDObject (2)



#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = top_valid1_105200_T1_McAtNlo_Jimmy'
import os
com="echo \"BEGIN_RTTINFO = top_valid1_105200_T1_McAtNlo_Jimmy\">&MYRTTINFOS.txt"
os.system(com)



from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc()

jobproperties.PerfMonFlags.doDetailedMonitoring = True


import os
os.system('get_files perfmon.py')
