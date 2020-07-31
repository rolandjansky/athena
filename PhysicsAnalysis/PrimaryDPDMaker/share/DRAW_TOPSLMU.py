####################################################
## Name: DRAW_TOPSLMU.py
##
## Author: anna bozena kowalewska
## Email:  anna.bozena.kowalewska@cern.ch
##
## Description: This defines the content of the 
##ttbar single tight muon DRAW output stream.
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_TOPSLMU_Seq = CfgMgr.AthSequencer("DRAW_TOPSLMU_Seq")


##============================================================================
## Define the skimming (event selection) for the DRAW_TOPSLMU output stream
##============================================================================
# Object selection strings
# FixedCutBEff_77 for btag
sel_muon = 'Muons.pt > 30.0*GeV && abs(Muons.eta) < 2.5 && (Muons.quality == 0)'
sel_jet  = 'AntiKt4EMTopoJets.pt > 25*GeV && abs(AntiKt4EMTopoJets.eta) < 2.5'
sel_bjet  = 'AntiKt4EMTopoJets.pt > 25*GeV && abs(AntiKt4EMTopoJets.eta) < 2.5 && BTagging_AntiKt4EMTopo.MV2c10_discriminant>0.645925'
sel_el  = 'Electrons.pt > 20*GeV && ( Electrons.Loose || Electrons.LHLoose ) && abs( Electrons.eta) < 2.47'

#met = 'MET_Reference_AntiKt4EMTopo["FinalClus"].met > 55*GeV'
met = 'MET_Reference_AntiKt4EMTopo["FinalTrk"].met > 45*GeV'

#triggers for periodD3 Run=302872, events=474
muTriggers = ['HLT_mu20_iloose_L1MU15','HLT_mu24_ivarmedium','HLT_mu24_imedium']

# Event selection string: 
draw_ttbar = '( count('+sel_muon+')==1 && count('+sel_jet+')>=3 && count('+sel_bjet+')>=1 && count('+sel_el+') == 0 && count('+met+') >=1 ) && (' + " || ".join(muTriggers) + ')'

print("This is the selection string that will be used for DRAW_TOPSLMU:")
print(draw_ttbar)

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DRAW_TOPSLMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_TOPSLMU_SkimmingTool",
                                                                        expression = draw_ttbar)

ToolSvc += DRAW_TOPSLMU_SkimmingTool

print("Skimming tool:")
print(DRAW_TOPSLMU_SkimmingTool)

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_TOPSLMU_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_TOPSLMUKernel",
                                                               SkimmingTools = [DRAW_TOPSLMU_SkimmingTool]
                                                               )

topSequence += DRAW_TOPSLMU_Seq


##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_TOPSLMU.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_TOPSLMU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass

StreamDRAW_TOPSLMU = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_TOPSLMU.AddRequireAlgs(["DRAW_TOPSLMUKernel"])
	
# Don't write an output RAW file if it is empty
StreamDRAW_TOPSLMU.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()	
	
#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
	
# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
