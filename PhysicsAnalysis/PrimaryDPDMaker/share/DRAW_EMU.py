####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_EMU_Seq = CfgMgr.AthSequencer("DRAW_EMU_Seq")

# Object selection strings
sel_muon1  = 'Muons.pt > 20*GeV && Muons.ptcone40/Muons.pt < 0.3'
sel_muon2  = 'Muons.pt > 15*GeV && Muons.ptcone40/Muons.pt < 0.3'
sel_el1  = 'Electrons.pt > 25*GeV && ( Electrons.Medium || Electrons.LHMedium )'
sel_el2  = 'Electrons.pt > 15*GeV && ( Electrons.Medium || Electrons.LHMedium )'

# Event selection string
draw_emu = '( (count('+sel_muon1+')>=1 && count('+sel_el2+')>=1) || (count('+sel_el1+')>=1 && count('+sel_muon2+')>=1) )'

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_EMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_EMU_SkimmingTool",
                                                                        expression = draw_emu)

ToolSvc += DRAW_EMU_SkimmingTool
print(DRAW_EMU_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_EMU_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_EMUKernel",
                                                               SkimmingTools = [DRAW_EMU_SkimmingTool]
                                                               )
topSequence += DRAW_EMU_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_EMU.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_EMU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_EMU = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_EMU.AddRequireAlgs(["DRAW_EMUKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_EMU.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

