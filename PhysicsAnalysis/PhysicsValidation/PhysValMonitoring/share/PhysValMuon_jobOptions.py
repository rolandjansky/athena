from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")

from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
from RecExConfig.RecFlags import rec as recFlags

tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool( name = 'muphysval' )
tool1.IsData = not recFlags.doTruth()

#
tool1.MuonContainerName = 'Muons';
tool1.SlowMuonContainerName = '';
tool1.MuonTruthParticleContainerName = 'MuonTruthParticles';
tool1.DoBinnedResolutionPlots = True

#comment out if you don't need any of the following containers
tool1.TrackContainerName = 'InDetTrackParticles'
# #tool1.FwdTrackContainerName='InDetForwardTrackParticles'
tool1.MuonTrackContainerName = 'MuonSpectrometerTrackParticles'
tool1.MuonExtrapolatedTrackContainerName = 'ExtrapolatedMuonTrackParticles'
tool1.MuonOnlyExtrapolatedTrackContainerName = 'MSOnlyExtrapolatedMuonTrackParticles'
tool1.MuonSegmentContainerName = 'MuonSegments'


#tool1.MuonTruthParticleContainerName = 'MuonTruthParticle' # uncomment for release 19
# tool1.DoTrigMuonValidation =True
# tool1.DoTrigMuonEFValidation = True
# tool1.DoTrigMuonL2Validation = True
# tool1.DoTrigMuonL1Validation = True
tool1.SelectHLTMuonItems = [
  ["HLT_mu20","L1_MU20"],
  ["HLT_mu20_iloose_L1MU15","L1_MU15"],
  ["HLT_mu24","L1_MU20"],
  ["HLT_mu24_iloose_L1MU15","L1_MU15"],
  ["HLT_mu24_imedium","L1_MU20"],
  ["HLT_mu26","L1_MU20"],
  ["HLT_mu26_imedium","L1_MU20"],
  ["HLT_mu50","L1_MU20"]
]

tool1.SelectL1MuonItems = [
  "L1_MU4",
  "L1_MU6",
  "L1_MU10",
  "L1_MU11",
  "L1_MU15",
  "L1_MU20"
]

#Select Muon Working Points... (empty: all, -1: None, 0: Tight, 1: Medium, 2: Loose, 3: VeryLoose)
tool1.SelectMuonWorkingPoints = [ 1 ]

#Select Muon Authors... (empty: all authors, 0: None, 1: combined, 2: STACO, 4: MuTagIMO, 5: Standalone, 6: MuGirl, 8: CaloTag)
tool1.SelectMuonAuthors = [ 1, 2, 4 ,5, 6, 8, 9 ]

#Select Muon Categories... (origin of muons, empty: all categories, 0: ALL, 1: PROMPT, 2: IN-FLIGHT, 3: NON-ISOLATED, 4: REST)
#tool1.SelectMuonCategories = [ 0, 1, 2, 3, 4 ] # all possible categories
tool1.SelectMuonCategories = [ 0, 1 ] # lighter version, good for official validation tasks; overriden when in data mode

from IsolationSelection.IsolationSelectionConf import CP__IsolationSelectionTool
IsolationTool = CP__IsolationSelectionTool( "IsolationSelectionTool",
                                            CalibFileName = "IsolationSelection/v1/MC15_Z_Jpsi_cutMap.root",
                                            MuonWP = "Gradient")
ToolSvc += IsolationTool
tool1.IsoTool = IsolationTool
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
