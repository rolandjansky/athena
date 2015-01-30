from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool()
tool1.DoMuonTrackValidation = True
tool1.DoMuonSegmentValidation = True
tool1.DoBinnedResolutionPlots = False
# tool1.MuonTruthParticleContainerName = 'MuonTruthParticle' # uncomment for release 19
tool1.DoTrigMuonValidation = False
tool1.DoTrigMuonEFValidation = True
tool1.DoTrigMuonL2Validation = False
tool1.DoTrigMuonL1Validation = True

#Select Muon Authors... (empty: all authors, 1: combined, 4: MuTagIMO, 5: Standalone, 6: MuGirl)
tool1.SelectMuonAuthors = [ 1, 4, 5, 6, 8, 9  ]

#Select Muon Categories... (origin of muons, empty: all categories, 0: ALL, 1: PROMPT, 2: IN-FLIGHT, 3: NON-ISOLATED, 4: REST)
tool1.SelectMuonCategories = [ 0, 1 ]

tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
