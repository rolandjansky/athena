# Set up the file reading:
from glob import glob
import AthenaPoolCnvSvc.ReadAthenaPool
#DataPath = '/home/gartoni/CalibFit/mc14_8TeV.208010.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6p5mu6p5.recon.AOD.e1966_s1933_s1911_r6242_tid05235159_00/'
#ServiceMgr.EventSelector.InputCollections = [ "*.*" ] #glob( '*.root*' ) 
ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/g/gabarone/work/public/TestMuons/data/testFile.root" ]
#ServiceMgr.EventSelector.InputCollections = [ '/data/atlas/atlasdata/artoni/mcp/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.DAOD_MUON0.e3601_s2576_s2132_r7267_r6282_p2471_tid07115862_00/DAOD_MUON0.07115862._000007.pool.root.1' ]
#ServiceMgr.EventSelector.InputCollections += [ '/data/atlas/atlasdata/artoni/mcp/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.DAOD_MUON0.e3601_s2576_s2132_r7267_r6282_p2471_tid07115862_00/DAOD_MUON0.07115862._000017.pool.root.1' ]
#ServiceMgr.EventSelector.InputCollections += [ '/data/atlas/atlasdata/artoni/mcp/mc15_13TeV.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.merge.DAOD_MUON0.e3735_s2726_r7267_r6282_p2471_tid07133886_00/DAOD_MUON0.07133886._000010.pool.root.1' ]
#ServiceMgr.EventSelector.InputCollections += [ '/data/atlas/atlasdata/artoni/mcp/mc15_13TeV.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.merge.DAOD_MUON0.e3735_s2726_r7267_r6282_p2471_tid07133886_00/DAOD_MUON0.07133886._000044.pool.root.1' ]

#ServiceMgr.EventSelector.InputCollections += ["/afs/cern.ch/user/g/gabarone/work/public/TestMuons/data/DAOD_MUON0.07725843._000010.pool.root.1"]

# Setup the global algorithm
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# THistSvc configuration
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='MCPCalibNtuple_xAOD.root' OPT='RECREATE'" ]

#ToolSvc += CfgMgr.TrigConf__xAODConfigTool( 'MyConfig' )
#ToolSvc += CfgMgr.Trig__TrigDecisionTool( ConfigTool = ToolSvc.MyConfig, TrigDecisionKey = 'xTrigDecision' )
ToolSvc += CfgMgr.Trig__TrigDecisionTool( TrigDecisionKey = 'xTrigDecision' )

# Setup GRL
#GRL = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPtCalibNtupleMaker/grl/data12_8TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_CombinedPerf_Muon_Muon_calo.xml' )
#ToolSvc += CfgMgr.GoodRunsListSelectionTool( 'GRLTool' , GoodRunsListVec = [ GRL ] )

# Setup MST
ToolSvc += CfgMgr.CP__MuonSelectionTool( 'MuonSelectorTool', OutputLevel = INFO )

from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsSelectorTool
SelectorTool = CalibMuonsSelectorTool( 'SelectorTool', OutputLevel = INFO ) 
SelectorTool.MuonSelectorTool = ToolSvc.MuonSelectorTool
ToolSvc += SelectorTool

# Setup algorithm
from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsNtupleMaker
NtupleMaker = CalibMuonsNtupleMaker( 'NtupleMaker', OutputLevel = INFO )
NtupleMaker.SelectorTool = SelectorTool
NtupleMaker.ExtendedVersion = False
theJob += NtupleMaker

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 100
