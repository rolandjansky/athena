#********************************************************************
# EXOT13.py 
# reductionConf flag EXOT13 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT13Stream )
EXOT13Stream = MSMgr.NewPoolRootStream( streamName, fileName )

#attach the output stream to kernel
EXOT13Stream.AcceptAlgs(["EXOT13Kernel"])

#Create a sequence for a specific stream
exot13Seq = CfgMgr.AthSequencer("EXOT13Sequence")

#=====================
# TRIGGER NAV THINNING
#=====================
triggerNavChains = "HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb|HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30|HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30|HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY|HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY|HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO|HLT_j30_jes_PS_llp_L1TAU30|HLT_j30_jes_PS_llp_L1TAU40|HLT_j30_jes_PS_llp_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO|HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH|HLT_j30_jes_cleanLLP_PS_llp_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO|HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO|HLT_3mu6_msonly|HLT_3mu6_msonly_L1MU4_EMPTY|HLT_3mu6_msonly_L1MU6_EMPTY|HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO|HLT_e24_lhmedium_L1EM18VH|HLT_e24_lhmedium_L1EM20VH|HLT_e60_lhmedium|HLT_e24_lhtight_nod0_ivarloose|HLT_e26_lhtight_nod0_ivarloose|HLT_e60_lhmedium_nod0|HLT_mu20_iloose_L1MU15|HLT_mu40|HLT_mu24_ivarloose|HLT_mu24_ivarloose_L1MU15|HLT_mu40|HLT_mu24_ivarmedium| HLT_mu24_imedium| HLT_mu50|HLT_mu26_ivarmedium|HLT_mu26_imedium| HLT_xe70_mht|HLT_xe90_mht_L1XE50|HLT_xe110_mht_L1XE50|HLT_xe110_pufit_L1XE50|HLT_j25|HLT_j35|HLT_j45|HLT_j60|HLT_j85|HLT_j110|HLT_j145|HLT_j175|HLT_j260|HLT_j340|HLT_j360|HLT_j380|HLT_j400|HLT_j420"

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT13ThinningHelper = ThinningHelper("EXOT13ThinningHelper")
EXOT13ThinningHelper.TriggerChains = triggerNavChains
EXOT13ThinningHelper.AppendToStream(EXOT13Stream)

#=======================================
# SKIMMING   
#=======================================
expression = 'HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30||    HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO||    HLT_j30_jes_PS_llp_L1TAU30||    HLT_j30_jes_PS_llp_L1TAU40||    HLT_j30_jes_PS_llp_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO||    HLT_3mu6_msonly||    HLT_3mu6_msonly_L1MU4_EMPTY||    HLT_3mu6_msonly_L1MU6_EMPTY||    HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO||    HLT_e24_lhmedium_L1EM18VH||    HLT_e24_lhmedium_L1EM20VH||    HLT_e60_lhmedium||    HLT_e24_lhtight_nod0_ivarloose||    HLT_e26_lhtight_nod0_ivarloose||    HLT_e60_lhmedium_nod0||    HLT_mu20_iloose_L1MU15||    HLT_mu40||    HLT_mu24_ivarloose||    HLT_mu24_ivarloose_L1MU15||    HLT_mu40||    HLT_mu24_ivarmedium||     HLT_mu24_imedium||     HLT_mu50||    HLT_mu26_ivarmedium||    HLT_mu26_imedium||     HLT_xe70_mht||    HLT_xe90_mht_L1XE50||    HLT_xe110_mht_L1XE50||    HLT_xe110_pufit_L1XE50||   HLT_j25||    HLT_j35||    HLT_j45||    HLT_j60||    HLT_j85||    HLT_j110||    HLT_j145||    HLT_j175||    HLT_j260||    HLT_j340||    HLT_j360||    HLT_j380||    HLT_j400||    HLT_j420' 


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT13StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT13StringSkimmingTool", expression = expression)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT13TrigDashSkimmingTool = DerivationFramework__TriggerSkimmingTool(name          = "EXOT13TrigDashSkimmingTool",
                                                                      TriggerListOR = ["HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH","HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH","HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO","HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO" ] )

ToolSvc += EXOT13TrigDashSkimmingTool
ToolSvc += EXOT13StringSkimmingTool
print EXOT13TrigDashSkimmingTool
print EXOT13StringSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EXOT13triggerORTool=DerivationFramework__FilterCombinationOR(name="EXOT13triggerORTool", FilterList=[EXOT13TrigDashSkimmingTool,EXOT13StringSkimmingTool] )
ToolSvc += EXOT13triggerORTool
print EXOT13triggerORTool

#=======================================
# THINNING
#=======================================

thinningTools=[]

# Thin all inner detector tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT13TPThinningTool = DerivationFramework__TrackParticleThinning( name                    =  "EXOT13TPThinningTool",
                                                                    ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                    SelectionString         =  "InDetTrackParticles.pt > 1.0*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5",
                                                                    InDetTrackParticlesKey  =  "InDetTrackParticles")
ToolSvc += EXOT13TPThinningTool
thinningTools.append(EXOT13TPThinningTool)

#Thin all electrons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13ElectronsThinningTool = DerivationFramework__GenericObjectThinning( name = "EXOT13ElectronsThinningTool",
                                        ThinningService = "EXOT13ThinningSvc",
                                        ContainerName = "Electrons",
                                        ApplyAnd = False,
                                        SelectionString = "Electrons.pt > 25*GeV && Electrons.eta>-2.5 && Electrons.eta<2.5")
ToolSvc += EXOT13ElectronsThinningTool
thinningTools.append(EXOT13ElectronsThinningTool)

#Thin all Muons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13MuonsThinningTool = DerivationFramework__GenericObjectThinning( name = "EXOT13MuonsThinningTool",
                                        ThinningService = "EXOT13ThinningSvc",
                                        ContainerName = "Muons",
                                        ApplyAnd = False,
                                        SelectionString = "Muons.pt > 6*GeV && Muons.eta>-2.5 && Muons.eta<2.5")
ToolSvc += EXOT13ElectronsThinningTool
thinningTools.append(EXOT13ElectronsThinningTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot13Seq
exot13Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT13Kernel_skim", SkimmingTools = [EXOT13triggerORTool])
exot13Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT13Kernel", ThinningTools = thinningTools)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT13
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT13ContentList import *
EXOT13SlimmingHelper = SlimmingHelper("EXOT13SlimmingHelper")
EXOT13SlimmingHelper.SmartCollections = EXOT13SmartContent 
EXOT13SlimmingHelper.AllVariables = EXOT13AllVariablesContent 
EXOT13SlimmingHelper.StaticContent = EXOT13UnslimmedContent 
EXOT13SlimmingHelper.ExtraVariables = EXOT13ExtraVariables
EXOT13SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT13SlimmingHelper.IncludeMuonTriggerContent = True
EXOT13SlimmingHelper.IncludeJetTriggerContent = True
EXOT13SlimmingHelper.AppendContentToStream(EXOT13Stream)
