####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# needed for dynamic determination of lowest-unprescaled single-muon and dimuon triggers
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_DIMU_Seq = CfgMgr.AthSequencer("DRAW_DIMU_Seq")

# Object selection strings
sel_muon1  = 'Muons.pt > 3.5*GeV'
sel_muon2  = 'Muons.pt > 3.5*GeV'

# Event selection string
dimuonMassString = '( count (  DRDiMuMass > 0.1*GeV   &&  DRDiMuMass < 20*GeV ) >= 1 )'

# Invariant masses in various configurations
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
DRDiMuMassTool = DerivationFramework__InvariantMassTool(name = "DRDiMuMassTool",
                                                      ContainerName              = "Muons",
                                                      ObjectRequirements         = sel_muon1,
                                                      SecondObjectRequirements   = sel_muon2,
                                                      MassHypothesis             = 105.66,
                                                      SecondMassHypothesis       = 105.66,
                                                      StoreGateEntryName         = "DRDiMuMass")
ToolSvc += DRDiMuMassTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
dimuonMassSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_DIMU_DiMuonMass_SkimmingTool",
                                                                     expression = dimuonMassString)
ToolSvc += dimuonMassSkimmingTool

# Muon selector tool
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
goodMuonTool = CP__MuonSelectionTool(name = "DRAW_DIMU_MuonsSelector")
goodMuonTool.MaxEta = 3.
goodMuonTool.MuQuality = 3
# turn of the momentum correction which is not needed for IDHits cut and Preselection
goodMuonTool.TurnOffMomCorr = True
ToolSvc += goodMuonTool
print(goodMuonTool)
from PrimaryDPDMaker.PrimaryDPDMakerConf import DerivationFramework__DRAW_ZMUMUSkimmingTool
goodMuonSkimmingTool = DerivationFramework__DRAW_ZMUMUSkimmingTool(name = "DRAW_DIMU_GoodMuon_SkimmingTool",
                                                               MuonContainerKey = "Muons",
                                                               MuonSelectorTool = goodMuonTool,
                                                               MinimumNumberOfMuons = 1,
                                                               MuonPtCut = 3.5) 
ToolSvc += goodMuonSkimmingTool

periods = TriggerPeriod.future | TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018
allUnprescaledTriggers = TriggerAPI.getLowestUnprescaledAnyPeriod(periods, TriggerType.mu)
print("DRAW_DIMU: will skim on an OR of the following muon triggers (list provided at run-time by the TriggerAPI):")
print(allUnprescaledTriggers)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
triggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "DRAWDIMUTriggerSkimmingTool", 
                                                               TriggerListOR = allUnprescaledTriggers)
ToolSvc += triggerSkimmingTool

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
DRAW_DIMU_SkimmingTool = DerivationFramework__FilterCombinationAND( name = "DRAW_DIMU_FinalFilter",
                                                                     FilterList=[dimuonMassSkimmingTool, 
                                                                                 goodMuonSkimmingTool] )
ToolSvc += DRAW_DIMU_SkimmingTool
print(DRAW_DIMU_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_DIMU_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_DIMUKernel",
                                                               AugmentationTools = [DRDiMuMassTool],
                                                               SkimmingTools = [DRAW_DIMU_SkimmingTool]
                                                               )
topSequence += DRAW_DIMU_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteRAWPerfDPD_DIMU.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_DIMU )
#streamName = primDPD.WriteDRAW_DIMU.StreamName
#fileName   = buildFileName( primDPD.WriteDRAW_DIMU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_DIMU = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_DIMU.AddRequireAlgs(["DRAW_DIMUKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_DIMU.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
