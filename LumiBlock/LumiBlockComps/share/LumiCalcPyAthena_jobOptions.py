from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
import AthenaPoolCnvSvc.ReadAthenaPool
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

# Input AOD file
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/user/r/radbal/scratch0/data/TopMix/user.RichardHawkings.0108175.topmix_Muon.AOD.v4._00057.pool.root" ]

# Number of Events to process
if not "EvtMax" in dir(): EvtMax=5
theApp.EvtMax = EvtMax

# the Top Algorithm Sequence
topSequence = AlgSequence()

# Add the service and configure
from LumiBlockComps.LumiBlockCompsConf import LumiCalcSvc
LumiCalcSvc = LumiCalcSvc()
LumiCalcSvc.Triggers = ["EF_mu20"]
LumiCalcSvc.UseMC = True
# explicit list of LumiBlockCollections names
LumiCalcSvc.LBCollNames = ["LumiBlocks", "IncompleteLumiBlocks"]
LumiCalcSvc.Verbose = False
# NOTE: UseInputStore must be *False* by default BUT just for this example we set it to True
LumiCalcSvc.UseInputStore = True 
svcMgr += LumiCalcSvc

# Add LumiCalcPyAthena Alg
from LumiBlockComps.LumiCalcPyAthena import LumiCalcPyAthena
theJob = AlgSequence()
theJob += LumiCalcPyAthena()
LumiCalcPyAthena.OutputLevel           = INFO
