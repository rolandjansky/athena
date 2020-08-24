####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
### Written having as example DRAW_EGCalib.py
### Nikolaos Rompotis, May 2015
### Contact: nikolaos.rompotis@cern.ch
###
### Updated: Aug 18,2015
###          - medium muon ID (Muons.quality <= 1)
###          - tau BDT score cut from 0.4 to 0.1
###          - isolation etcone/pt 0.06 -> 0.1
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_TAUMUH_Seq = CfgMgr.AthSequencer("DRAW_TAUMUH_Seq")


# Object selection strings

# muon selection, comment: muon ID has been removed  "Muons.DFCommonGoodMuon"
sel_mu = '(Muons.pt > 27.0*GeV) && (Muons.pt < 1450.0*GeV) && (Muons.etcone40/Muons.pt < 0.1) && (Muons.ptcone40/Muons.pt < 0.2) && Muons.passesIDCuts && (Muons.quality <= 1)'
muRequirement = '( count( '+sel_mu+'  ) == 1 )'

# tau selection
sel_tau        = '(TauJets.pt > 19.0*GeV)  && ( TauJets.nTracks >= 0) && ( TauJets.nTracks <= 4) && (TauJets.BDTJetScore > 0.1)'
tauRequirement = '( count( '+sel_tau+'  ) >= 1 )'

# event selection string
draw_taumuh =  muRequirement+' && '+tauRequirement + '&& ( count (TAUMUH_DeltaR > 2.9) >=1 )'

# DeltaR tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
TAUMUH_DeltaRTool = DerivationFramework__DeltaRTool(  name                    = "TAUMUH_DeltaRTool",
                                                      ContainerName           = "Muons",
                                                      ObjectRequirements =    sel_mu,
                                                      SecondContainerName     = "TauJets",
                                                      SecondObjectRequirements= sel_tau,
                                                      StoreGateEntryName      = "TAUMUH_DeltaR"
                                                      )
ToolSvc += TAUMUH_DeltaRTool
print(TAUMUH_DeltaRTool)
# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_TAUMUH_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_TAUMUH_SkimmingTool",
                                                                        expression = draw_taumuh)

ToolSvc += DRAW_TAUMUH_SkimmingTool
print(DRAW_TAUMUH_SkimmingTool)


# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_TAUMUH_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_TAUMUHKernel",
                                                                AugmentationTools = [TAUMUH_DeltaRTool],
                                                                SkimmingTools = [DRAW_TAUMUH_SkimmingTool]
                                                                )
topSequence += DRAW_TAUMUH_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_TAUMUH.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_TAUMUH )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_TAUMUH = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_TAUMUH.AddRequireAlgs(["DRAW_TAUMUHKernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_TAUMUH.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

