#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Digitization
#--------------------------------------------------------------

#
# POOL setup
#
import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()

execfile("ID_only.py")
execfile("geometry.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsInput=["singleMuon_youSexyThing_slhc_Hits.pool.root"]
athenaCommonFlags.PoolRDOOutput="singleMuon_youSexyThing_slhc_Digits.pool.root"
athenaCommonFlags.EvtMax = -1

from Digitization.DigitizationFlags import jobproperties 
jobproperties.Digitization.doInDetNoise = False

include ("Digitization/Digitization.py")
ServiceMgr.MessageSvc.OutputLevel = WARNING

include("InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py")

MessageSvc = Service("MessageSvc")
MessageSvc.defaultLimit = 200
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
