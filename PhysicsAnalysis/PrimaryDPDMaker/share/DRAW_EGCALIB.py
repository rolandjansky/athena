####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_EGCALIB_Seq = CfgMgr.AthSequencer("DRAW_EGCALIB_Seq")

# Object selection strings
sel_elec1 = 'Electrons.pt > 20*GeV && ( Electrons.isEMMedium || Electrons.isEMLHMedium )'
draw_egcalib1 = 'count(eeMass1 > 55*GeV) >= 1'

sel_elec2 = 'Electrons.pt > 5*GeV && ( Electrons.isEMTight || Electrons.isEMLHTight )'
draw_egcalib2 =  'count(eeMass2 > 1*GeV && eeMass2 < 5*GeV ) >= 1'

sel_elec3  = 'Electrons.pt > 15*GeV && ( Electrons.isEMMedium || Electrons.isEMLHMedium )'
sel_phot3  = 'Photons.pt > 7*GeV && Photons.isEMTight'
draw_egcalib3 =  '( count ( eeMass3 > 45*GeV && eeMass3 < 90*GeV ) >= 1 ) && ( count ( epMass > 40* GeV ) > 1 )'

sel_muon4  = 'Muons.pt > 15 *GeV'
sel_phot4  = 'Photons.pt > 7*GeV && Photons.isEMTight'
draw_egcalib4 = '( count (  mumuMass > 45*GeV   &&  mumuMass < 90*GeV ) >= 1 ) && ( count( pmuMass > 40*GeV) > 1 )'

# Event selection string
draw_egcalib = draw_egcalib1+' || '+draw_egcalib2+' || '+draw_egcalib3+' || '+draw_egcalib4

# Invariant masses in various configurations
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
eeMassTool1 = DerivationFramework__InvariantMassTool( name                      = "eeMassTool1",
                                                      ContainerName             = "Electrons",
                                                      ObjectRequirements        = sel_elec1,
                                                      MassHypothesis            = 0.511,
                                                      StoreGateEntryName        = "eeMass1")
ToolSvc += eeMassTool1

eeMassTool2 = DerivationFramework__InvariantMassTool( name                       = "eeMassTool2",
                                                      ContainerName              = "Electrons",
                                                      ObjectRequirements         = sel_elec2,
                                                      MassHypothesis             = 0.511,
                                                      StoreGateEntryName         = "eeMass2")
ToolSvc += eeMassTool2

eeMassTool3 = DerivationFramework__InvariantMassTool( name                       = "eeMassTool3",
                                                      ContainerName              = "Electrons",
                                                      ObjectRequirements         = sel_elec3,
                                                      MassHypothesis             = 0.511,
                                                      StoreGateEntryName         = "eeMass3")
ToolSvc += eeMassTool3

epMassTool = DerivationFramework__InvariantMassTool( name = "epMassTool",
                                                     ContainerName              = "Electrons",
                                                     SecondContainerName        = "Photons",
                                                     ObjectRequirements         = sel_elec3,
                                                     SecondObjectRequirements   = sel_phot3,
                                                     MassHypothesis             = 0.511,
                                                     SecondMassHypothesis       = 0.0,
                                                     StoreGateEntryName         = "epMass")
ToolSvc += epMassTool

mmMassTool = DerivationFramework__InvariantMassTool( name                       = "mmMassTool3",
                                                     ContainerName              = "Muons",
                                                     ObjectRequirements         = sel_muon4,
                                                     MassHypothesis             = 105.66,
                                                     StoreGateEntryName         = "mumuMass")
ToolSvc += mmMassTool

pmuMassTool = DerivationFramework__InvariantMassTool(name = "pmuMassTool",
                                                     ContainerName              = "Muons",
                                                     SecondContainerName        = "Photons",
                                                     ObjectRequirements         = sel_muon4,
                                                     SecondObjectRequirements   = sel_phot4,
                                                     MassHypothesis             = 105.66,
                                                     SecondMassHypothesis       = 0.0,
                                                     StoreGateEntryName         = "pmuMass")
ToolSvc += pmuMassTool

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_EGCALIB_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_EGCALIB_SkimmingTool",
                                                                        expression = draw_egcalib)

ToolSvc += DRAW_EGCALIB_SkimmingTool
print DRAW_EGCALIB_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_EGCALIB_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_EGCALIBKernel",
                                                                  AugmentationTools = [eeMassTool1,eeMassTool2,eeMassTool3,epMassTool,mmMassTool,pmuMassTool],
                                                                  SkimmingTools = [DRAW_EGCALIB_SkimmingTool]
                                                                )
topSequence += DRAW_EGCALIB_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_EGCALIB.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_EGCALIB )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_EGCALIB = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_EGCALIB.AddRequireAlgs(["DRAW_EGCALIBKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_EGCALIB.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
