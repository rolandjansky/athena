####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_EGZ_Seq = CfgMgr.AthSequencer("DRAW_EGZ_Seq")

# Object selection strings

# This is for the Zee
sel_elec1 = 'Electrons.pt > 20*GeV && ( Electrons.Medium || Electrons.LHMedium )'
draw_egz1 = ' ( count(eeMass1 > 55*GeV) >= 1 )'

# This is for Zeegam
sel_elec3  = 'Electrons.pt > 15*GeV && ( Electrons.Medium || Electrons.LHMedium )'
draw_egz3 =  '( ( count ( eeMass3 > 20*GeV && eeMass3 < 90*GeV ) >= 1 ) && ( count (Photons.pt > 7*GeV && Photons.Tight )>=1  ) )'

#This is for Zmmgam
sel_muon4  = 'Muons.pt > 15 *GeV'
draw_egz4 = ' ( (  count (  DREmumuMass > 20*GeV   &&  DREmumuMass < 90*GeV ) >= 1 ) && ( count (Photons.pt > 7*GeV && Photons.Tight ) >=1 ))'

# Event selection string
draw_egz = draw_egz1+' || '+draw_egz3+' || '+draw_egz4

# Invariant masses in various configurations
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool

eeMassTool1 = DerivationFramework__InvariantMassTool( name                      = "eeMassTool1",
                                                      ContainerName             = "Electrons",
                                                      ObjectRequirements        = sel_elec1,
                                                      MassHypothesis            = 0.511,
                                                      StoreGateEntryName        = "eeMass1")
ToolSvc += eeMassTool1


eeMassTool3 = DerivationFramework__InvariantMassTool( name                       = "eeMassTool3",
                                                      ContainerName              = "Electrons",
                                                      ObjectRequirements         = sel_elec3,
                                                      MassHypothesis             = 0.511,
                                                      StoreGateEntryName         = "eeMass3")
ToolSvc += eeMassTool3

mmMassTool = DerivationFramework__InvariantMassTool( name                       = "mmMassTool",
                                                     ContainerName              = "Muons",
                                                     ObjectRequirements         = sel_muon4,
                                                     MassHypothesis             = 105.66,
                                                     StoreGateEntryName         = "DREmumuMass")
ToolSvc += mmMassTool

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_EGZ_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_EGZ_SkimmingTool",
                                                                        expression = draw_egz)

ToolSvc += DRAW_EGZ_SkimmingTool
print(DRAW_EGZ_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_EGZ_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_EGZKernel",
                                                                  AugmentationTools = [eeMassTool1,eeMassTool3,mmMassTool],
                                                                  SkimmingTools = [DRAW_EGZ_SkimmingTool]
                                                                )
topSequence += DRAW_EGZ_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_EGZ.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_EGZ )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_EGZ = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_EGZ.AddRequireAlgs(["DRAW_EGZKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_EGZ.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
