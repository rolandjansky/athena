from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")

algseq = CfgMgr.AthSequencer("AthAlgSeq")
from InDetUsedInFitTrackDecoratorTool.InDetUsedInFitTrackDecoratorToolConf import InDet__InDetUsedInFitTrackDecoratorTool
PhysValMuUsedInFitDecoratorTool = InDet__InDetUsedInFitTrackDecoratorTool(name                 = "PhysValMuUsedInFitDecoratorTool",
                                                                          AMVFVerticesDecoName = "TTVA_AMVFVertices",
                                                                          AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                                                                          TrackContainer       = "InDetTrackParticles",
                                                                          VertexContainer      = "PrimaryVertices" )
ToolSvc += PhysValMuUsedInFitDecoratorTool
from InDetUsedInVertexFitTrackDecorator.InDetUsedInVertexFitTrackDecoratorConf import InDet__InDetUsedInVertexFitTrackDecorator
PhysValMuInDetUsedInFitDecorator = InDet__InDetUsedInVertexFitTrackDecorator(name                   = "PhysValMuInDetUsedInFitDecorator",
                                                                             UsedInFitDecoratorTool = ToolSvc.PhysValMuUsedInFitDecoratorTool)
algseq += PhysValMuInDetUsedInFitDecorator
from IsolationAlgs.IsoUpdatedTrackCones import GetUpdatedIsoTrackCones
if not hasattr(algseq,"IsolationBuilderTight500"):
    algseq += GetUpdatedIsoTrackCones()

from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
from RecExConfig.RecFlags import rec as recFlags

tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool( name = 'muphysval' )
tool1.IsData = not recFlags.doTruth()

#add if you need any of the following containers
#tool1.FwdTrackContainerName='InDetForwardTrackParticles'
tool1.SlowMuonContainerName = ''

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
tool1.SelectMuonCategories = [ 0, 1 ] # lighter version, good for official validation tasks overriden when in data mode

if not recFlags.doTruth():
    tool1.SelectMuonCategories = [0, 1, 4]

from IsolationSelection.IsolationSelectionConf import CP__IsolationSelectionTool
IsolationTool = CP__IsolationSelectionTool( "IsolationSelectionTool",
                                            MuonWP = "PflowTight_FixedRad")
ToolSvc += IsolationTool
tool1.IsoTool = IsolationTool
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

#Flag for saving a ROOT TTree in an output ntuple... default value is False --> no TTree is saved
#tool1.DoMuonTree = True

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
