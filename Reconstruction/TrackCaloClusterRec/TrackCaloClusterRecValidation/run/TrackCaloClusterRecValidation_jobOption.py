# $Id: TrackCaloClusterRecValidation_jobOptions.py $

# Set up the reading of the input xAOD:
import getpass
#FNAME = "XAOD_Wprime.pool.root"

#FNAME = ["root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_WPrime2000_1.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_WPrime2000_2.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_WPrime2000_3.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_WPrime2000_4.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_WPrime2000_5.pool.root",]
#name = "WPrime2000"

#FNAME = ["root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_JZ5W_1.pool.root",
	 ##"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_JZ5W_2.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_JZ5W_3.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_JZ5W_4.pool.root",
	 #"root://eosatlas.cern.ch//eos/atlas/user/n/ncalace/Samples_Physics/XAOD_Processed_Mar18/XAOD_JZ5W_5.pool.root",]
#name = "JZ5W"

FNAME = ["/afs/cern.ch/work/r/rjansky/TCC3/run/XAOD_WprimedRMatch.pool.root"]
name = "XAOD_WprimedRMatch"

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = FNAME

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/TCCPlotDef.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "MyTCC"
topSequence += monMan

from TrackCaloClusterRecValidation.TrackCaloClusterRecValidationConf import TrackCaloClusterRecValidationTool
tool = TrackCaloClusterRecValidationTool()
tool.JetTruthContainerName        = "AntiKt10TruthJets"
tool.TopoJetReferenceName         = "MyAntiKt10LCTopoJets"
tool.TopoTrimmedJetReferenceName  = "MyAntiKt10LCTopoTrimmedJets"
tool.JetContainerNames = ["MyAntiKt10LCTopoJets"                  ,"MyAntiKt10LCTopoTrimmedJets",
			  "AntiKt10TrackCaloClustersAllJets"      ,"AntiKt10TrackCaloClustersAllTrimmedJets",    
			  "AntiKt10TrackCaloClustersChargedJets"  ,"AntiKt10TrackCaloClustersChargedTrimmedJets",
			  "AntiKt10TrackCaloClustersJets"         ,"AntiKt10TrackCaloClustersTrimmedJets"]
tool.OutputLevel = INFO
print tool
ToolSvc += tool
monMan.AthenaMonTools += [tool]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MyTCC DATAFILE='TCC_Output_"+name+".root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
###theApp.EvtMax = 1000
theApp.EvtMax = -1
