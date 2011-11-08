NTupleSvc = Service( "NTupleSvc" )
doTrigger=False
doWriteTAG=False
#doESD=False # uncomment if do not run ESD making algorithms
doWriteESD=False # uncomment if do not write ESD
#doAOD=False # uncomment if do not run AOD making algorithms
doWriteAOD=False # uncomment if do not write AOD
doWriteTAG=False # uncomment if do not write TAG
doCBNT=False
#AllAlgs = False # if false, all algorithms are switched off by defaults
doCaloCluster = True
#doCaloTopoCluster = False
doMuonIDCombined = True
doMuGirl=True
doBtagging=False
doMissingET=True
doObjMissingET=True
doAtlfast=False
doTriggerConfigOnly=False


from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.FastSimulation=False
AODFlags.ParticleJet=False
AODFlags.TruthParticleJet=False
AODFlags.JetTag=False
AODFlags.MissingEtTruth   = False



PoolRDOInput =["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00001.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00002.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00003.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00004.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00005.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00006.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00007.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00008.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00009.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00010.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00011.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00013.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00014.pool.root"];
PoolRDOInput +=["rfio:/castor/cern.ch/user/r/rtt/RTTdata/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550/calib0_csc11.007233.singlepart_mu100.digit.RDO.v12003101_tid003550._00015.pool.root"];




EvtMax=-1
#include ("RecExCommon/RecExCommon_flags.py")
#include ("RegionSelector/RegionSelector_jobOptions.py")
include ("RecExCommon/RecExCommon_topOptions.py")

Service("MessageSvc").OutputLevel = ERROR
Service("AthenaSealSvc").OutputLevel = ERROR

#include("MuGirl/MuGirl_jobOptions.py")
from MuGirl.MuGirlConf import MuGirl

MuGirl.doCSC = True
MuGirl.doTruth  = True
MuGirl.doGlobalFit  = True
#MuGirl.doClusterRot=False
MuGirl.doStau=False
MuGirl.doANNSelection=True
MuGirl.doParticleCreator=True
MuGirl.doLHR=True
MuGirl.doNTuple=True
MuGirl.OutputLevel = INFO


from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
MuGirlCandidateTool = MuGirlNS__CandidateToolConfig()
ToolSvc += MuGirlCandidateTool
MuGirlCandidateTool.OutputLevel = WARNING

from MuGirlPerformanceTools.MuGirlPerformanceToolsConf import MuGirlNS__PerformanceTruthTool
MuGirlPerformanceTruthTool = MuGirlNS__PerformanceTruthTool()
ToolSvc += MuGirlPerformanceTruthTool
MuGirlPerformanceTruthTool.OutputLevel = DEBUG


from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
MuGirlGlobalFitTool = MuGirlNS__GlobalFitTool()
ToolSvc += MuGirlGlobalFitTool
#MuGirlGlobalFitTool.UseTrackSummaryTool=True
MuGirlGlobalFitTool.OutputLevel = DEBUG
#MuGirlGlobalFitTool.ParticleCreatorTool=PublicToolHandle('Trk::ParticleCreatorTool/ParticleCreatorTool'), # GaudiHandle

from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
MuGirlParticleCreatorTool = MuGirlNS__MuGirlParticleCreatorTool()
ToolSvc += MuGirlParticleCreatorTool
MuGirlParticleCreatorTool.OutputLevel = ERROR

MuGirlNTupleFile = "singleMuonsPt100.root"
NTupleSvc.Output += [ "MUGIRL DATAFILE='"+MuGirlNTupleFile+"' TYP='ROOT' OPT='NEW'" ]

