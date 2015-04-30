# Set up the file reading:
from glob import glob
import AthenaPoolCnvSvc.ReadAthenaPool
#DataPath = '/home/gartoni/CalibFit/data12_8TeV.00202987.physics_Muons.merge.AOD.r6614_p2319_tid05309873_00/'
#DataPath = '/home/gartoni/CalibFit/data12_8TeV.00205017.physics_Muons.merge.AOD.r6614_p2319_tid05309950_00/'
#DataPath = '/home/gartoni/CalibFit/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e1852_s1896_s1912_r6259_tid05324222_00/'
DataPath = '/home/gartoni/CalibFit/data/'
#DataPath = '/home/gartoni/CalibFit/mc14_8TeV.208010.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6p5mu6p5.recon.AOD.e1966_s1933_s1911_r6242_tid05235159_00/'
ServiceMgr.EventSelector.InputCollections = glob( DataPath + '*.root*' ) 
#ServiceMgr.EventSelector.InputCollections = [ '/home/gartoni/CalibFit/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e1852_s1896_s1912_r6259_tid05324222_00/AOD.05324222._000007.pool.root.1' ]

# Setup the global algorithm
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# THistSvc configuration
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='MCPCalibNtuple_xAOD.root' OPT='RECREATE'" ]

# Setup GRL
GRL = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPtCalibNtupleMaker/grl/data12_8TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_CombinedPerf_Muon_Muon_calo.xml' )
ToolSvc += CfgMgr.GoodRunsListSelectionTool( 'GRLTool' , GoodRunsListVec = [ GRL ] )

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
ServiceMgr.MessageSvc.defaultLimit = 1000000
