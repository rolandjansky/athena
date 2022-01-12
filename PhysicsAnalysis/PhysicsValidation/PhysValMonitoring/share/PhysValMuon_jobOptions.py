from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")

algseq = CfgMgr.AthSequencer("AthAlgSeq")

from PyUtils.MetaReader import read_metadata
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
try:
    input_file = svcMgr.EventSelector.InputCollections[0]
except AttributeError:
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    input_file = athenaCommonFlags.FilesInput()[0]
metadata = read_metadata(input_file)
metadata = metadata[input_file]  # promote all keys one level up

isDAOD_PHYSVAL=False
for class_name, name in metadata['metadata_items'].items():
    if name == 'EventStreamInfo':
        if "DAOD_PHYSVAL" in class_name :
            print ("Running on DAOD_PHYSVAL - will not add TTVA decorations.")
            isDAOD_PHYSVAL=True
    
if not isDAOD_PHYSVAL:
    from IsolationAlgs.IsoUpdatedTrackCones import GetUpdatedIsoTrackCones
    if not hasattr(algseq,"IsolationBuilderNonprompt_All_MaxWeight500"):
        ToolSvc += CfgMgr.InDet__InDetUsedInFitTrackDecoratorTool(  name                    = algseq.name()+"_InDetUsedInFitDecoratorTool_forIso",
                                                                    AMVFVerticesDecoName    = 'TTVA_AMVFVertices',
                                                                    AMVFWeightsDecoName     = 'TTVA_AMVFWeights',
                                                                    TrackContainer          = 'InDetTrackParticles',
                                                                    VertexContainer         = 'PrimaryVertices' )
        algseq += CfgMgr.InDet__InDetUsedInVertexFitTrackDecorator(name                    = algseq.name()+"_InDetUsedInFitDecorator_forIso",
                                                                   UsedInFitDecoratorTool  = getattr(ToolSvc, algseq.name()+"_InDetUsedInFitDecoratorTool_forIso") )
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
tool1.SelectMuonWorkingPoints = [0, 1, 2 ]

#Select Muon Authors... (empty: all authors, 0: None, 1: combined, 2: STACO, 4: MuTagIMO, 5: Standalone, 6: MuGirl, 8: CaloTag, 10: CaloScore)
tool1.SelectMuonAuthors = [ 1, 2, 4, 5, 6, 8, 10 ]

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
