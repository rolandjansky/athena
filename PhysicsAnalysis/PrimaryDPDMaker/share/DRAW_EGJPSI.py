####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_EGJPSI_Seq = CfgMgr.AthSequencer("DRAW_EGJPSI_Seq")

sel_elec2 = 'Electrons.pt > 5*GeV && ( Electrons.Tight || Electrons.LHTight )'
draw_egcalib2 =  '( count(eeMass2 > 1*GeV && eeMass2 < 5*GeV ) >= 1 )'

# a list of triggers we might want to activate at some point 
#draw_egcalib_trigger_13TeV =  ' (HLT_e5_tight_e4_etcut || HLT_e5_tight_e4_etcut_Jpsiee || HLT_e9_tight_e4_etcut_Jpsiee ||  HLT_e9_etcut_e5_tight_Jpsiee ||
#HLT_e14_tight_e4_etcut_Jpsiee || HLT_e14_etcut_e5_tight_Jpsiee || HLT_e5_lhtight_e4_etcut HLT_e5_lhtight_e4_etcut_Jpsiee ||             
#HLT_e9_lhtight_e4_etcut_Jpsiee || HLT_e9_etcut_e5_lhtight_Jpsiee || HLT_e14_lhtight_e4_etcut_Jpsiee || HLT_e14_etcut_e5_lhtight_Jpsiee  ||                             
#HLT_e5_tight1_e4_etcut || HLT_e5_tight1_e4_etcut_Jpsiee || HLT_e5_tight_e4_etcut_L1JPSI-1M5 || HLT_e5_lhtight_e4_etcut_L1JPSI-1M5 ||                             
#HLT_e5_tight1_e4_etcut_L1JPSI-1M5 || HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5 || e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7 ||        
#HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7 || HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12 || HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12 ||
#HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5 || HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7 || HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7 ||   
#HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12 || HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12, HLT_e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5 ) ' 

# Event selection string

draw_egcalib = draw_egcalib2

# Invariant masses in various configurations
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool

eeMassTool2 = DerivationFramework__InvariantMassTool( name                       = "eeMassTool2",
                                                      ContainerName              = "Electrons",
                                                      ObjectRequirements         = sel_elec2,
                                                      MassHypothesis             = 0.511,
                                                      StoreGateEntryName         = "eeMass2")
ToolSvc += eeMassTool2

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_EGJPSI_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_EGJPSI_SkimmingTool",
                                                                        expression = draw_egcalib)

ToolSvc += DRAW_EGJPSI_SkimmingTool
print(DRAW_EGJPSI_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_EGJPSI_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_EGJPSIKernel",
                                                                  AugmentationTools = [eeMassTool2],
                                                                  SkimmingTools = [DRAW_EGJPSI_SkimmingTool]
                                                                )
topSequence += DRAW_EGJPSI_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_EGJPSI.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_EGJPSI )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_EGJPSI = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_EGJPSI.AddRequireAlgs(["DRAW_EGJPSIKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_EGJPSI.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
