#
#    Job options for running SiDigiAnalysis package (make root file with easy-to-get info 
#    from RDO (digitised hits) file)
#
execfile("ID_only.py")
execfile("geometry.py")

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from SiDigiAnalysis.SiDigiAnalysisConf import SiDigiAnalysis
job += SiDigiAnalysis("SiDigiAnalysis")

from VP1Algs.VP1AlgsConf import VP1Alg
job += VP1Alg()


theApp.EvtMax = -1
#
# POOL setup
#
import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
#
#    Input file
#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ "singleMuon_youSexyThing_slhc_Digits.pool.root" ]
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput.get_Value()
#
#    Output file
#
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["DIG_NTUPLE DATAFILE='SiDigAnalOutput.root' OPT='RECREATE'"]
