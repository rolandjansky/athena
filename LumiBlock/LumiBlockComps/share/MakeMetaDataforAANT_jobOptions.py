# Basic setup for Athena job
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence


# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# the Top Algorithm Sequence
topSequence = AlgSequence()

###
# Modify the following to run over exactly the AOD files being used in your analysis
###
ServiceMgr.EventSelector.InputCollections = ["/afs/atlass01.physik.uni-bonn.de/data/share/top/topmix/AOD/user.RichardHawkings.0108173.topmix_Egamma.AOD.v2/user.RichardHawkings.0108173.topmix_Egamma.AOD.v2._00139.pool.root"]


# Number of Events to process
if not "EvtMax" in dir(): EvtMax=-1
theApp.EvtMax = EvtMax

# Number of Events to skip
if not "SkipEvents" in dir(): SkipEvents=0
ServiceMgr.EventSelector.SkipEvents = SkipEvents

from LumiBlockComps.LumiBlockCompsConf import CreateAANTFromLumiBlockCollection
CreateAANTFromLB = CreateAANTFromLumiBlockCollection("CreateAANTFromLB")
topSequence += CreateAANTFromLB

###################### Output Ntuple files
# User CBNTMaker Tool to create TTree
from GaudiSvc.GaudiSvcConf import THistSvc
from AnalysisTools.AnalysisToolsConf import AANTupleStream

# create instance of THistSvc
ServiceMgr += THistSvc()

# ntuples: AANT  (Athena Aware NTuples)
ntupleFile = "testlumi.root"
ServiceMgr.THistSvc.Output += [ "AANT DATAFILE='%s' OPT='RECREATE'" % ntupleFile  ]

# also add default CollectionTree
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
AANTupleStream.OutputName = ntupleFile
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING
topSequence += AANTupleStream

# Root Ntuple output file and name
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

# Dump list of containers on StoreGate to output log
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO

# Print the job schedule/sequence on output
print topSequence
