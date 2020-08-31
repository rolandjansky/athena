##********************************************************************
# EXOT13.py 
# reductionConf flag EXOT13 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
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
#Single Lepton Triggers 2015-2016-2017-2018
trigger_electron = [
    # Single e
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e140_lhloose_nod0',
    'HLT_e300_etcut',
    'HLT_e24_lhmedium_L1EM20VH',
    'HLT_e60_lhmedium',
    'HLT_e120_lhloose',
    'HLT_e26_lhtight_nod0',
    'HLT_e24_lhmedium_nod0_L1EM20VH',
    'HLT_e24_lhtight_nod0_ivarloose',
    'HLT_e60_medium',
]

trigger_muon = [
    # Sinlge muon
    'HLT_mu26_ivarmedium',
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu50',
    'HLT_mu60_0eta105_msonly',
    'HLT_mu24_ivarmedium',
    'HLT_mu24_imedium',
    'HLT_mu24_iloose',
    'HLT_mu24_iloose_L1MU15', 
    'HLT_mu24_ivarloose',
    'HLT_mu24_ivarloose_L1MU15', 
    'HLT_mu40',
]

triggerVBF = [
    # old trigger based on MET
    'HLT_2j40_0eta490_invm250_L1XE55',
    #VBF inclusive 
    'HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF',
    # VBF + MET 
    'HLT_j70_j50_0eta490_invm1000j50_dphi24_xe90_pufit_xe50_L1MJJ-500-NFF',
    # VBF + muon for potential VBF + prompt signatures
    'HLT_2mu6_2j50_0eta490_invm900j50_L1MJJ-500-NFF',
    'HLT_mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF',
    #'L1_MJJ-500-NFF',
]


trigger_ggf = [
		   #narrow scan
                   'HLT_mu20_msonly_mu6noL1_msonly_nscan05',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb',
                   'HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb',
                   'HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40',
                   'HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30',
                   'HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly',
                   'HLT_mu11_2mu4noL1_nscan03_L1MU10_2MU6',
                   'HLT_mu20_mu6noL1_nscan03',
		   #CalRatio
                   'HLT_j30_jes_PS_llp_L1TAU60',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU60',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU100',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100',
                   'HLT_j30_jes_PS_llp_noiso_L1TAU60',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH',
                   'HLT_j30_jes_PS_llp_L1TAU30', 
		   'HLT_j30_jes_PS_llp_L1TAU40',
		   'HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU60', 
		   'HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU60', 
		   'HLT_j30_jes_cleanLLP_PS_llp_pufix_L1TAU100', 
		   'HLT_j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU100',
 		   #tri-muon
                   'HLT_3mu8_msonly',
                   'HLT_3mu6_msonly',
	      ]

trigger_late = [
                   #narrow scan
                   'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY',
                   'HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY',
                   'HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO',
                   'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO',
                   'HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU6_EMPTY',
                   'HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO',
                   'HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO',
                   'HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO',
		   #CalRatio
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY',
                   'HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY',
                   'HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO',
	  	   #tri-muon
                   'HLT_3mu8_msonly_L1MU4_EMPTY',
                   'HLT_3mu8_msonly_L1MU6_EMPTY',
                   'HLT_3mu8_msonly_L1MU4_UNPAIRED_ISO',
                   'HLT_3mu6_msonly_L1MU4_EMPTY',
                   'HLT_3mu6_msonly_L1MU6_EMPTY',
                   'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO',

]


triggerJet = [
                   'HLT_j25',
                   'HLT_j35',
                   'HLT_j45',
                   'HLT_j60',
                   'HLT_j85',
                   'HLT_j110',
                   'HLT_j145',
                   'HLT_j175',
                   'HLT_j260',
                   'HLT_j340',
                   'HLT_j360',
                   'HLT_j380',
                   'HLT_j400',
                   'HLT_j420',
]
triggerMet = [
                   'HLT_xe110_pufit_xe70_L1XE50',
		   'HLT_xe110_pufit_L1XE55',
		   'HLT_xe110_mht_L1XE50',
		   'HLT_xe90_mht_L1XE50',
		   'HLT_xe70',
                   'HLT_noalg_L1J400',
]

triggersNavThin=list(set(trigger_electron + trigger_muon + triggerVBF + trigger_ggf + trigger_late + triggerJet + triggerMet ))
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT13ThinningHelper = ThinningHelper("EXOT13ThinningHelper")
EXOT13ThinningHelper.TriggerChains = ' | '.join(triggersNavThin)
EXOT13ThinningHelper.AppendToStream(EXOT13Stream)

#=======================================
# SKIMMING   
#=======================================

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT13TrigDashSkimmingTool = DerivationFramework__TriggerSkimmingTool(name          = "EXOT13TrigDashSkimmingTool",
                                                                      TriggerListOR = trigger_electron + trigger_muon + triggerVBF + trigger_ggf + trigger_late + triggerJet + triggerMet)


ToolSvc += EXOT13TrigDashSkimmingTool
print EXOT13TrigDashSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EXOT13triggerORTool=DerivationFramework__FilterCombinationOR(name="EXOT13triggerORTool", FilterList=[EXOT13TrigDashSkimmingTool] )
ToolSvc += EXOT13triggerORTool
print EXOT13triggerORTool


FlavorTagInit(JetCollections = ['AntiKt4EMTopoJets'], Sequencer = exot13Seq)

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
                                                                         SelectionString = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20*GeV && AntiKt4EMTopoJets.eta >-4.9 && AntiKt4EMTopoJets.eta <4.9) || (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 10*GeV && AntiKt4EMTopoJets.eta >-2.5 && AntiKt4EMTopoJets.eta <2.5 && AntiKt4EMTopoJets.EMFrac < 0.45)")
ToolSvc += EXOT13JetsThinningTool
thinningTools.append(EXOT13JetsThinningTool)


#Thin all bjets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13BJetsThinningTool = DerivationFramework__GenericObjectThinning(name                 = "EXOT13BJetsThinningTool",
                                                                         ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "AntiKt4EMTopoJets_BTagging201810",
                                                                         ApplyAnd        = False,
                                                                         SelectionString = "(AntiKt4EMTopoJets_BTagging201810.DFCommonJets_Calib_pt > 20*GeV && AntiKt4EMTopoJets_BTagging201810.eta >-4.9 && AntiKt4EMTopoJets_BTagging201810.eta <4.9) || (AntiKt4EMTopoJets_BTagging201810.DFCommonJets_Calib_pt > 10*GeV && AntiKt4EMTopoJets_BTagging201810.eta >-2.5 && AntiKt4EMTopoJets_BTagging201810.eta <2.5 && AntiKt4EMTopoJets_BTagging201810.EMFrac < 0.45)")
ToolSvc += EXOT13BJetsThinningTool
thinningTools.append(EXOT13BJetsThinningTool)


#Thin all Muons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT13MuonsThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT13MuonsThinningTool",
                                                                     ThinningService = EXOT13ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "Muons",
                                                                     ApplyAnd        = False,
                                                                     SelectionString = "Muons.pt > 2*GeV && Muons.eta>-2.5 && Muons.eta<2.5")
ToolSvc += EXOT13MuonsThinningTool
thinningTools.append(EXOT13MuonsThinningTool)

# Thinning CaloCalTopoClusters associated to AntiKt4EMTopoJets
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT13JCCThinningTool = DerivationFramework__JetCaloClusterThinning( name                    = "EXOT13JCCThinningTool",
                                                                     ThinningService         = "EXOT13ThinningSvc",
                                                                     SGKey                   = "AntiKt4EMTopoJets",
                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                     SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 10*GeV && AntiKt4EMTopoJets.eta >-2.5 && AntiKt4EMTopoJets.eta <2.5 && AntiKt4EMTopoJets.EMFrac < 0.45",
                                                                     AdditionalClustersKey = ["EMOriginTopoClusters","CaloCalTopoClusters"] )
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
#EXOT13SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT13SlimmingHelper.IncludeJetTriggerContent = True

# Add origin corrected clusters -- for accessing jet constituents
EXOT13SlimmingHelper.ExtraVariables.append('CaloCalTopoClusters.e_sampl.calM.calE.calEta.calPhi.CENTER_MAG')
EXOT13SlimmingHelper.AppendToDictionary["EMOriginTopoClusters"]='xAOD::CaloClusterContainer'
EXOT13SlimmingHelper.AppendToDictionary["EMOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
EXOT13SlimmingHelper.ExtraVariables.append('EMOriginTopoClusters.e_sampl.calM.calE.calEta.calPhi')
EXOT13SlimmingHelper.AppendToDictionary["BTagging_AntiKt4EMTopo_201810"]='xAOD::BTaggingContainer'
EXOT13SlimmingHelper.AppendToDictionary["BTagging_AntiKt4EMTopo_201810Aux"]='xAOD::BTaggingAuxContainer'
EXOT13SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent 

EXOT13SlimmingHelper.AppendContentToStream(EXOT13Stream)

