from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")

from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
from RecExConfig.RecFlags import rec as recFlags

tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool( name = 'slowmuphysval' )
tool1.IsData = not recFlags.doTruth()

#
tool1.MuonContainerName = ''; # Must be blank for SlowMuons
tool1.SlowMuonContainerName = 'SlowMuons';
tool1.MuonTruthParticleContainerName = 'MuonTruthParticles';
tool1.DoBinnedResolutionPlots = False

tool1.TrackContainerName = ''
tool1.FwdTrackContainerName=''
tool1.MuonTrackContainerName = ''
tool1.MuonExtrapolatedTrackContainerName = ''
tool1.MuonOnlyExtrapolatedTrackContainerName = ''
tool1.MuonSegmentContainerName = ''

#Select Muon Working Points... (empty: all, -1: None, 0: Tight, 1: Medium, 2: Loose, 3: VeryLoose)
tool1.SelectMuonWorkingPoints = [ -1 ]

#Select Muon Authors... (empty: all authors, 0: None, 1: combined, 2: STACO, 4: MuTagIMO, 5: Standalone, 6: MuGirl, 8: CaloTag)
tool1.SelectMuonAuthors = [ 0 ] # only one author is defined in this case... no need to specify

#Select Muon Categories... (origin of muons, empty: all categories, 0: ALL, 1: PROMPT, 2: IN-FLIGHT, 3: NON-ISOLATED, 4: REST)
#tool1.SelectMuonCategories = [ 0, 1, 2, 3, 4 ] # all possible categories
tool1.SelectMuonCategories = [ 0, 1 ] # lighter version, good for official validation tasks; overriden when in data mode

tool1.IsoTool = IsolationTool
tool1.EnableLumi = False
tool1.OutputLevel = ERROR
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
