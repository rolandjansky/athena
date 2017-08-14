# Skeleton joboption for a simple analysis job and adding Pileup Reweighting Tool

# Says how many events to run over. Set to -1 for all events
theApp.EvtMax=-1

# Sets up reading of POOL files (e.g. xAODs)
import AthenaPoolCnvSvc.ReadAthenaPool
import glob             
svcMgr.EventSelector.InputCollections = glob.glob("*.root")

# Gets the main AthSequencer
algseq = CfgMgr.AthSequencer("AthAlgSeq")

ToolSvc += CfgMgr.CP__PileupReweightingTool("myTool",ConfigFiles=["my.prw.root"],LumiCalcFiles=["GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root","GoodRunsLists/data16_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"])

# Adds an instance of your alg to it
algseq += CfgMgr.VertexAnalysisAlgorithm(PRWTool=ToolSvc.myTool)

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["VertexAnalysis DATAFILE='PWR.root' OPT='RECREATE'"]
