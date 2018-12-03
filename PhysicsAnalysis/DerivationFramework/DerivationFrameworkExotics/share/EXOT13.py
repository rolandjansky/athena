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
import DerivationFrameworkJetEtMiss.ExtendedJetCommon

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
triggerNavChains = "HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb|HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30|HLT_mu20_msonly_mu6noL1_msonly_nscan05|HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40|HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30|HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY|HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY|HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU6_EMPTY|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO|HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO|HLT_j30_jes_PS_llp_L1TAU30|HLT_j30_jes_PS_llp_L1TAU40|HLT_j30_jes_PS_llp_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60|HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO|HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH|HLT_j30_jes_cleanLLP_PS_llp_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO|HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY|HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO|HLT_3mu8_msonly|HLT_3mu8_msonly_L1MU4_EMPTY|HLT_3mu8_msonly_L1MU6_EMPTY|HLT_3mu8_msonly_L1MU4_UNPAIRED_ISO|HLT_3mu6_msonly|HLT_3mu6_msonly_L1MU4_EMPTY|HLT_3mu6_msonly_L1MU6_EMPTY|HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO|HLT_j25|HLT_j35|HLT_j45|HLT_j60|HLT_j85|HLT_j110|HLT_j145|HLT_j175|HLT_j260|HLT_j340|HLT_j360|HLT_j380|HLT_j400|HLT_j420|HLT_xe110_pufit_xe70_L1XE50|HLT_xe120_pufit_L1XE50|HLT_xe110_pufit_xe65_L1XE50|HLT_xe110_pufit_L1XE55|HLT_xe100_pufit_L1XE55|HLT_xe90_pufit_L1XE50|HLT_xe110_pufit_L1XE50|HLT_xe100_pufit_L1XE50|L1_XE50|L1_XE70|HLT_xe70|HLT_xe70_pueta|HLT_xe80|HLT_xe80_pueta|HLT_xe100|HLT_xe100_pueta|HLT_xe100_mht_L1XE50|HLT_xe110_mht_L1XE50|HLT_xe130_mht_L1XE50|HLT_xe70_tc_em|HLT_xe70_tc_lcw|HLT_xe70_mht|HLT_xe70_pufit|HLT_xe100_tc_em|HLT_xe100_tc_lcw|HLT_xe100_mht|HLT_xe100_pufit|HLT_xe80_tc_lcw_L1XE50|HLT_xe90_mht_L1XE50|HLT_xe90_tc_lcw_L1XE50|HLT_xe100_tc_lcw_L1XE50|HLT_xe100_tc_lcw_L1XE60"

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT13ThinningHelper = ThinningHelper("EXOT13ThinningHelper")
EXOT13ThinningHelper.TriggerChains = triggerNavChains
EXOT13ThinningHelper.AppendToStream(EXOT13Stream)

#=======================================
# SKIMMING   
#=======================================
expression = 'HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30||    HLT_mu20_msonly_mu6noL1_msonly_nscan05||    HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40||    HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY||    HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU6_EMPTY||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO||    HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO||    HLT_j30_jes_PS_llp_L1TAU30||    HLT_j30_jes_PS_llp_L1TAU40||    HLT_j30_jes_PS_llp_L1TAU60||   HLT_j30_jes_cleanLLP_PS_llp_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY||    HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO||    HLT_3mu8_msonly||   HLT_3mu8_msonly_L1MU4_EMPTY||    HLT_3mu8_msonly_L1MU6_EMPTY||    HLT_3mu8_msonly_L1MU4_UNPAIRED_ISO||   HLT_3mu6_msonly||    HLT_3mu6_msonly_L1MU4_EMPTY||    HLT_3mu6_msonly_L1MU6_EMPTY||    HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO||    HLT_j25||    HLT_j35||    HLT_j45||    HLT_j60||    HLT_j85||    HLT_j110||    HLT_j145||    HLT_j175||    HLT_j260||    HLT_j340||    HLT_j360||    HLT_j380||    HLT_j400||    HLT_j420||  HLT_xe110_pufit_xe70_L1XE50||   HLT_xe120_pufit_L1XE50||   HLT_xe110_pufit_xe65_L1XE50||   HLT_xe110_pufit_L1XE55||   HLT_xe100_pufit_L1XE55||   HLT_xe90_pufit_L1XE50||   HLT_xe110_pufit_L1XE50||   HLT_xe100_pufit_L1XE50||   L1_XE50||   L1_XE70||   HLT_xe70||   HLT_xe70_pueta||   HLT_xe80||   HLT_xe80_pueta||   HLT_xe100||   HLT_xe100_pueta||   HLT_xe100_mht_L1XE50||   HLT_xe110_mht_L1XE50||   HLT_xe130_mht_L1XE50||   HLT_xe70_tc_em||   HLT_xe70_tc_lcw||   HLT_xe70_mht||   HLT_xe70_pufit||   HLT_xe100_tc_em||   HLT_xe100_tc_lcw||   HLT_xe100_mht||   HLT_xe100_pufit||   HLT_xe80_tc_lcw_L1XE50||   HLT_xe90_mht_L1XE50||   HLT_xe90_tc_lcw_L1XE50||   HLT_xe100_tc_lcw_L1XE50||   HLT_xe100_tc_lcw_L1XE60' 


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT13StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT13StringSkimmingTool", expression = expression)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT13TrigDashSkimmingTool = DerivationFramework__TriggerSkimmingTool(name          = "EXOT13TrigDashSkimmingTool",
                                                                      TriggerListOR = ["HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH","HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH","HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO","HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO"] )

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
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT13TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "EXOT13TPThinningTool",
                                                                  ThinningService         = EXOT13ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm && InDetTrackParticles.pt > 0.5*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT13TPThinningTool
thinningTools.append(EXOT13TPThinningTool)

#Thin all electrons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13ElectronsThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT13ElectronsThinningTool",
                                                                         ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "Electrons",
                                                                         ApplyAnd        = False,
                                                                         SelectionString = "Electrons.pt > 6*GeV && Electrons.eta>-2.5 && Electrons.eta<2.5")
ToolSvc += EXOT13ElectronsThinningTool
thinningTools.append(EXOT13ElectronsThinningTool)


#Thin all jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13JetsThinningTool = DerivationFramework__GenericObjectThinning(name                 = "EXOT13JetsThinningTool",
                                                                         ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "AntiKt4EMTopoJets",
                                                                         ApplyAnd        = False,
                                                                         SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 10*GeV && AntiKt4EMTopoJets.eta >-2.5 && AntiKt4EMTopoJets.eta <2.5")
ToolSvc += EXOT13JetsThinningTool
thinningTools.append(EXOT13JetsThinningTool)


#Thin all Muons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13MuonsThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT13MuonsThinningTool",
                                                                     ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "Muons",
                                                                     ApplyAnd        = False,
                                                                     SelectionString = "Muons.pt > 6*GeV && Muons.eta>-2.5 && Muons.eta<2.5")
ToolSvc += EXOT13MuonsThinningTool
thinningTools.append(EXOT13MuonsThinningTool)

# Thinning CaloCalTopoClusters associated to AntiKt4EMTopoJets
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT13JCCThinningTool = DerivationFramework__JetCaloClusterThinning( name                    = "EXOT13JCCThinningTool",
                                                                     ThinningService         = "EXOT13ThinningSvc",
                                                                     SGKey                   = "AntiKt4EMTopoJets",
                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                     SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 10*GeV && AntiKt4EMTopoJets.eta >-2.5 && AntiKt4EMTopoJets.eta <2.5",
                                                                     AdditionalClustersKey = ["EMOriginTopoClusters","LCOriginTopoClusters","CaloCalTopoClusters"] )
ToolSvc += EXOT13JCCThinningTool
thinningTools.append(EXOT13JCCThinningTool)



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
EXOT13SlimmingHelper.IncludeMuonTriggerContent = True
EXOT13SlimmingHelper.IncludeJetTriggerContent = True

# Add origin corrected clusters -- for accessing jet constituents
EXOT13SlimmingHelper.ExtraVariables.append('CaloCalTopoClusters.e_sampl.calM.calE.calEta.calPhi')
EXOT13SlimmingHelper.AppendToDictionary["LCOriginTopoClusters"]='xAOD::CaloClusterContainer'
EXOT13SlimmingHelper.AppendToDictionary["LCOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
EXOT13SlimmingHelper.ExtraVariables.append('LCOriginTopoClusters.e_sampl.calM.calE.calEta.calPhi')
EXOT13SlimmingHelper.AppendToDictionary["EMOriginTopoClusters"]='xAOD::CaloClusterContainer'
EXOT13SlimmingHelper.AppendToDictionary["EMOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
EXOT13SlimmingHelper.ExtraVariables.append('EMOriginTopoClusters.e_sampl.calM.calE.calEta.calPhi')


EXOT13SlimmingHelper.AppendContentToStream(EXOT13Stream)

