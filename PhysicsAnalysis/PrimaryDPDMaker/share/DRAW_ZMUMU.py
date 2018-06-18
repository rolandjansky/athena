####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_ZMUMU_Seq = CfgMgr.AthSequencer("DRAW_ZMUMU_Seq")

# Object selection strings
sel_muon1  = 'Muons.pt > 25*GeV && Muons.ptcone40/Muons.pt < 0.3'
sel_muon2  = 'Muons.pt > 20*GeV && Muons.ptcone40/Muons.pt < 0.3'

# Event selection string
draw_zmumu = '( count (  DRZmumuMass > 70*GeV   &&  DRZmumuMass < 110*GeV ) >= 1 )'

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

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_ZMUMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_ZMUMU_SkimmingTool",
                                                                        expression = draw_zmumu)

ToolSvc += DRAW_ZMUMU_SkimmingTool
print DRAW_ZMUMU_SkimmingTool

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
