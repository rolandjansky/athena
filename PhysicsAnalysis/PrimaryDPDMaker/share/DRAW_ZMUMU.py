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
DRAW_ZMUMU_Seq = CfgMgr.AthSequencer("DRAW_ZMUMU_Seq")

# Object selection strings
sel_muon1  = 'Muons.pt > 25*GeV && Muons.ptcone40/Muons.pt < 0.3'
sel_muon2  = 'Muons.pt > 20*GeV && Muons.ptcone40/Muons.pt < 0.3'

# Event selection string
dimuonMassString = '( count (  DRZmumuMass > 70*GeV   &&  DRZmumuMass < 110*GeV ) >= 1 )'

# Invariant masses in various configurations
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
DRZmumuMassTool = DerivationFramework__InvariantMassTool(name = "DRZmumuMassTool",
                                                      ContainerName              = "Muons",
                                                      ObjectRequirements         = sel_muon1,
                                                      SecondObjectRequirements   = sel_muon2,
                                                      MassHypothesis             = 105.66,
                                                      SecondMassHypothesis       = 105.66,
                                                      StoreGateEntryName         = "DRZmumuMass")
ToolSvc += DRZmumuMassTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
dimuonMassSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_ZMUMU_DiMuonMass_SkimmingTool",
                                                                     expression = dimuonMassString)
ToolSvc += dimuonMassSkimmingTool

# Muon selector tool
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
goodMuonTool = CP__MuonSelectionTool(name = "DRAW_ZMUMU_MuonsSelector")
goodMuonTool.MaxEta = 3.
goodMuonTool.MuQuality = 3
# turn of the momentum correction which is not needed for IDHits cut and Preselection
goodMuonTool.TurnOffMomCorr = True
ToolSvc += goodMuonTool
print(goodMuonTool)
from PrimaryDPDMaker.PrimaryDPDMakerConf import DerivationFramework__DRAW_ZMUMUSkimmingTool
goodMuonSkimmingTool = DerivationFramework__DRAW_ZMUMUSkimmingTool(name = "DRAW_ZMUMU_GoodMuon_SkimmingTool",
                                                               MuonContainerKey = "Muons",
                                                               MuonSelectorTool = goodMuonTool,
                                                               MinimumNumberOfMuons = 1,
                                                               MuonPtCut = 20.0) 
ToolSvc += goodMuonSkimmingTool

periods = TriggerPeriod.future | TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017
allUnprescaledTriggers = TriggerAPI.getLowestUnprescaledAnyPeriod(periods, TriggerType.mu)
print("DRAW_ZMUMU: will skim on an OR of the following muon triggers (list provided at run-time by the TriggerAPI):")
print(allUnprescaledTriggers)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
triggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "DRAWZMUMUTriggerSkimmingTool", 
                                                               TriggerListOR = allUnprescaledTriggers)
ToolSvc += triggerSkimmingTool

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
DRAW_ZMUMU_SkimmingTool = DerivationFramework__FilterCombinationAND( name = "DRAW_ZMUMU_FinalFilter",
                                                                     FilterList=[dimuonMassSkimmingTool, 
                                                                                 goodMuonSkimmingTool, 
                                                                                 triggerSkimmingTool] )
ToolSvc += DRAW_ZMUMU_SkimmingTool
print(DRAW_ZMUMU_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_ZMUMU_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_ZMUMUKernel",
                                                               AugmentationTools = [DRZmumuMassTool],
                                                               SkimmingTools = [DRAW_ZMUMU_SkimmingTool]
                                                               )
topSequence += DRAW_ZMUMU_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteRAWPerfDPD_ZMUMU.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_ZMUMU )
#streamName = primDPD.WriteDRAW_ZMUMU.StreamName
#fileName   = buildFileName( primDPD.WriteDRAW_ZMUMU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_ZMUMU = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_ZMUMU.AddRequireAlgs(["DRAW_ZMUMUKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_ZMUMU.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
