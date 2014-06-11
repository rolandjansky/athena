###############################################
# Defines a dummy track steering strategy
#  for use in the new steering code.  This is
#  only for testing - it will not efficiently
#  find muons for analysis!!
###############################################

from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MuPatCandidateTool
mpct = Muon__MuPatCandidateTool()
ToolSvc += mpct

from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperTool
meht = Muon__MuonEDMHelperTool()
ToolSvc += meht

from MuonTrackSteeringTools.MuonTrackSteeringToolsConf import Muon__MuonTrackSteering
mtst = Muon__MuonTrackSteering()

mtst.SegSeedQCut = 1
mtst.Seg2ndQCut  = 0

# Allowed options for the strategy currently are:
#   CutSeedsOnTracks
#   CombineSegInStation
#   DynamicSeeding
#   PreferOutsideIn
#   AllowOneSharedHit

simpleStrategy = [ 
    "BarrelSmall[]:BMS;BOS;BIS" ,
    "BarrelLarge[]:BML;BOL;BIL" ,
    "EndcapSmall[]:EMS;EOS;EIS" ,
    "EndcapLarge[]:EML;EOL;EIL" ]

mtst.StrategyList = simpleStrategy

