from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock('none')

NTupleSvc = Service( "NTupleSvc" )
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
doTauRec=False
doTrigger=False
doWriteRDO=False
#if not 'doWriteESD' in dir():
    #doWriteESD=True
doWriteAOD=False
doAOD=False 
doWriteTAG=False

#doESD=False # uncomment if do not run ESD making algorithms
doWriteESD=False # uncomment if do not write ESD
#doAOD=False # uncomment if do not run AOD making algorithms
doCBNT=False
#AllAlgs = False # if false, all algorithms are switched off by defaults
doCaloCluster = True

#doCaloTopoCluster = False
#doMuonIDCombined = True
#doMoore=True
doMuGirl=True
doMuBoy=False
doBtagging=False
doMissingET=False
doObjMissingET=False
doAtlfast=False
doTriggerConfigOnly=False
doJetRec=False
doJetMissingETTag=False


from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.FastSimulation=False
AODFlags.ParticleJet=False
AODFlags.TruthParticleJet=False
AODFlags.JetTag=False
AODFlags.MissingEtTruth   = False

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.SelectReco = ['']

##################### VALGRIND
from Valkyrie.ValkyrieConf import ValgrindSvc
valgrindSvc = ValgrindSvc( OutputLevel = VERBOSE )
valgrindSvc.ProfiledAlgs += ["MuGirl"]
valgrindSvc.IgnoreFirstNEvents = 2      # to start instrumentation after the second event only
#
# Add service and turn on auditing
svcMgr += valgrindSvc
theApp.CreateSvc += [svcMgr.ValgrindSvc.getFullJobOptName()]
theApp.AuditAlgorithms = True
theApp.AuditTools      = True
theApp.AuditServices   = True
##############################

EvtMax=2500
DetDescrVersion="ATLAS-GEO-08-00-02"
#SkipEvents = 25
#include ("RecExCommon/RecExCommon_flags.py")
#include ("RegionSelector/RegionSelector_jobOptions.py")
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents
include ("RecExCommon/RecExCommon_topOptions.py")
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents

#Service("MessageSvc").OutputLevel = ERROR
#Service("MessageSvc").OutputLevel = DEBUG
Service("AthenaSealSvc").OutputLevel = ERROR

#include("MuGirl/MuGirl_jobOptions.py")
#from MuGirl.MuGirlConfig import MuGirl

#MuGirl.MuGirlCandidate.doCSC = True
MuGirl.doTruth  = True
MuGirl.doStau  = False
MuGirl.doGlobalFit  = True
MuGirl.doMSRefit  = True
#MuGirl.doClusterRot=False
MuGirl.doANNSelection=True
MuGirl.doParticleCreator=True
MuGirl.doLHR=True
MuGirl.doNTuple=True
MuGirl.OutputLevel = DEBUG

#ToolSvc.MuGirlCandidateTool.OutputLevel = DEBUG

#ToolSvc.MuGirlPerformanceTruthTool.OutputLevel = DEBUG
from MuidStatistics.MuidStatisticsConf import MuidStatistics
MuidStatistics.OutputLevel = DEBUG
ToolSvc.iPatFitter.OutputLevel = DEBUG
#ToolSvc.SoftMuonTag.OutputLevel=ERROR

from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
MuGirlGlobalFitTool = MuGirlNS__GlobalFitTool()
ToolSvc += MuGirlGlobalFitTool
#MuGirlGlobalFitTool.UseTrackSummaryTool=True
MuGirlGlobalFitTool.OutputLevel = INFO
#MuGirlGlobalFitTool.ParticleCreatorTool=PublicToolHandle('Trk::ParticleCreatorTool/ParticleCreatorTool'), 
# GaudiHandle
#MuGirlGlobalFitTool.OutputLevel = DEBUG
#MuGirlGlobalFitTool.OutputLevel = ERROR
#MuGirlGlobalFitTool.ProbabilityCut = 0.5

#ToolSvc.MuGirlParticleCreatorTool.OutputLevel = DEBUG
#ToolSvc.MuGirlParticleCreatorTool.OverlapFileName = "T2overlap.txt"

#MuGirlNTupleFile = "singleMuonsPt100_0.5.root"
MuGirlNTupleFile = "T1.root"
NTupleSvc.Output += [ "MUGIRL DATAFILE='"+MuGirlNTupleFile+"' TYP='ROOT' OPT='NEW'" ]


