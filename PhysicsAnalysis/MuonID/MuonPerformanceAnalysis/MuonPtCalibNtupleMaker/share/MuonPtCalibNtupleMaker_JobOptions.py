# Set up the file reading:
import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.InputCollections = [ '/afs/cern.ch/work/d/dzhang/public/MUON_DxAOD_test/mc14_8TeV.208001.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu4mu4.merge.AOD.e1331_s1896_s1912_r5591_r5625_tid01522846_00/AOD.01522846._002986.pool.root.1' ]
#ServiceMgr.EventSelector.InputCollections = [ '/afs/cern.ch/work/d/dzhang/public/MUON_DxAOD_test/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.DAOD/DAOD_MUON0.PoOct02b.pool.root' ]
ServiceMgr.EventSelector.InputCollections  = [ '/afs/cern.ch/work/g/gartoni/work/Analysis/TestingMyPackages/Run/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v111._000197.4' ]

# Setup the global algorithm
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# THistSvc configuration
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='MCPCalibNtuple_xAOD.root' OPT='RECREATE'" ]

# Setup tools
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
ServiceMgr.MessageSvc.defaultLimit = 1000000
