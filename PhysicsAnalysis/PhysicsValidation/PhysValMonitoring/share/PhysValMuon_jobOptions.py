from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")

from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool()
#tool1.IsData = True # uncomment to run on data
tool1.DoMuonTrackValidation = True
tool1.DoMuonSegmentValidation = True
tool1.DoBinnedResolutionPlots = True
#tool1.MuonTruthParticleContainerName = 'MuonTruthParticle' # uncomment for release 19
# tool1.DoTrigMuonValidation =True
# tool1.DoTrigMuonEFValidation = True
# tool1.DoTrigMuonL2Validation = True
# tool1.DoTrigMuonL1Validation = True

#Select Muon Authors... (empty: all authors, 1: combined, 2: STACO, 4: MuTagIMO, 5: Standalone, 6: MuGirl, 8: CaloTag)
tool1.SelectMuonAuthors = [ 1, 2, 4 ,5, 6, 8 ]

#Select Muon Categories... (origin of muons, empty: all categories, 0: ALL, 1: PROMPT, 2: IN-FLIGHT, 3: NON-ISOLATED, 4: REST)
#tool1.SelectMuonCategories = [ 0, 1, 2, 3, 4 ] # all possible categories
tool1.SelectMuonCategories = [ 0, 1 ] # lighter version, good for official validation tasks

tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
