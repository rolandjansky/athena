
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.JobProperties import jobproperties
flags = jobproperties.MuonPtCalibNtupleMaker_Flags
# THistSvc configuration
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='{}' OPT='RECREATE'".format( flags.outputFilename() ) ]

from AthenaCommon import CfgMgr
ToolSvc = ServiceMgr.ToolSvc

# Get Muon Selector Tool
if not hasattr( ToolSvc, 'MuonSelectorTool' ):
  ToolSvc += CfgMgr.CP__MuonSelectionTool( 'MuonSelectorTool' )
  ToolSvc.MuonSelectorTool.TurnOffMomCorr=True

# Get Trigger Decision Tool
if not hasattr( ToolSvc, 'TrigDecisionTool' ):
  ToolSvc += CfgMgr.Trig__TrigDecisionTool( 'TrigDecisionTool', TrigDecisionKey = 'xTrigDecision' )

# Get Trigger Matching Tool
if not hasattr( ToolSvc, 'TrigMuonMatching' ):
  from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
  ToolSvc += Trig__TrigMuonMatching( 'TrigMuonMatching' )
  #ToolSvc.TrigMuonMatching.TriggerTool = ToolSvc.TrigDecisionTool
  
from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsSelectorTool
SelectorTool = CalibMuonsSelectorTool( 'SelectorTool' ) 
SelectorTool.MuonSelectorTool = ToolSvc.MuonSelectorTool
ToolSvc += SelectorTool

# Setup algorithm
from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsNtupleMaker
NtupleMaker = CalibMuonsNtupleMaker( 'NtupleMaker' )
NtupleMaker.SelectorTool = SelectorTool
#NtupleMaker.TrigDecisionTool = ToolSvc.TrigDecisionTool
#NtupleMaker.TrigMuonMatching = ToolSvc.TrigMuonMatching
NtupleMaker.ExtendedVersion = False
theJob += NtupleMaker
