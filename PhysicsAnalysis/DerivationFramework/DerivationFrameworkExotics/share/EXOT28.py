#********************************************************************
# EXOT28.py 
# reductionConf flag EXOT28 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT28Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT28Stream )
EXOT28Stream = MSMgr.NewPoolRootStream( streamName, fileName )

#attach the output stream to kernel
EXOT28Stream.AcceptAlgs(["EXOT28Kernel"])

#Create a sequence for a specific stream
EXOT28Seq = CfgMgr.AthSequencer("EXOT28Sequence")

#=====================
# TRIGGER NAV THINNING
#=====================

SingleLepton_2018 = [
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0',
    'HLT_e60_lhmedium_nod0'
    'HLT_e140_lhloose_nod0'
    'HLT_e300_etcut',
    'HLT_mu26_ivarmedium',
    'HLT_mu50',
    'HLT_mu60_0eta105_msonly',
]
SingleLepton_2017 = [
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e140_lhloose_nod0',
    'HLT_e300_etcut',
    'HLT_mu26_ivarmedium',
    'HLT_mu50',
    'HLT_mu60_0eta105_msonly',
]
SingleLepton_2016 = [
    'HLT_e24_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_medium',
    'HLT_e140_lhloose_nod0',
    'HLT_e300_etcut',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_medium', 
    'HLT_e140_lhloose_nod0', 
    'HLT_e300_etcut',
    'HLT_mu24_iloose', 
    'HLT_mu24_iloose_L1MU15',
    'HLT_mu24_ivarloose', 
    'HLT_mu24_ivarloose_L1MU15',
    'HLT_mu40',
    'HLT_mu50',
    'HLT_mu24_ivarmedium', 
    'HLT_mu24_imedium', 
    'HLT_mu26_imedium',
    'HLT_mu26_ivarmedium', 
]
SingleLepton_2015 = [
    'HLT_e24_lhmedium_L1EM20VH',  
    'HLT_e60_lhmedium',
    'HLT_e120_lhloose',
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu40',
    'HLT_mu60_0eta105_msonly',
]

MultiMuon_2018 = [
    'HLT_2mu14' , 
    'HLT_mu22_mu8noL1', 
    'HLT_mu20_2mu4noL1', 
    'HLT_3mu6', 
]
MultiMuon_2017 = [
    'HLT_2mu14',
    'HLT_mu22_mu8noL1',
    'HLT_mu22_mu8noL1_calotag_0eta010',
    'HLT_mu20_2mu4noL1',
    'HLT_3mu6',
    'HLT_3mu6_msonly',
    'HLT_4mu4',
]
MultiMuon_2016 = [
    'HLT_2mu10', 
    'HLT_2mu10_nomucomb', 
    'HLT_mu20_mu8noL1', 
    'HLT_mu20_2mu4noL1', 
    'HLT_3mu4', 
    'HLT_mu6_2mu4', 
    'HLT_mu20_nomucomb_mu6noL1_nscan03', 
    'HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6', 
    'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb', 
    'HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6_bTau',
    'HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau', 
    'HLT_mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4', 
    'HLT_2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4',
    'HLT_2mu14', 
    'HLT_2mu14_nomucomb', 
    'HLT_3mu6',
    'HLT_mu22_mu8noL1',
    'HLT_3mu6_msonly',
]
MultiMuon_2015 = [
    'HLT_2mu10',
    'HLT_3mu6',
    'HLT_3mu6_msonly',
    'HLT_mu18_2mu4noL1',
    'HLT_mu18_mu8noL1',
]

MET_2018 = [
    'HLT_xe110_pufit_xe70_L1XE50',
    'HLT_xe120_pufit_L1XE50',
    'HLT_xe110_pufit_xe65_L1XE50',
    'HLT_xe110_pufit_xe70_L1XE50',
    'HLT_xe120_pufit_L1XE50',
    'HLT_xe110_pufit_xe65_L1XE50',
    'HLT_xe110_pufit_xe70_L1XE50',
]
MET_2017 = [
    'HLT_xe90_pufit_L1XE50', 
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe100_pufit_L1XE55',
    'HLT_xe100_pufit_L1XE50',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE50',
]
MET_2016 = [
    'HLT_xe90_mht_L1XE50', 
    'HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe90_tc_lcw_wEFMu_L1XE50',
    'HLT_xe90_mht_wEFMu_L1XE50', 
    'HLT_xe100_L1XE50', 
    'HLT_xe100_tc_em_L1XE50', 
    'HLT_xe120_pueta', 
    'HLT_xe120_pufit',
    'HLT_xe120_tc_lcw_L1XE50',
    'HLT_xe110_pueta_L1XE50',
    'HLT_xe100_tc_em_L1XE50',
    'HLT_xe110_mht_L1XE50',
]
MET_2015 = [
    'HLT_xe70_mht', 
    'HLT_xe70_tc_lcw', 
    'HLT_xe70', 
]

HT_2018 = [
    'HLT_ht1000_L1J100',
]
HT_2017 = [
    'HLT_ht1000_L1J100',
]
HT_2016 = [
    'HLT_ht700_L1J75',
    'HLT_ht1000',
    'HLT_ht1000_L1J100',
]
HT_2015 = [
    'HLT_ht700_L1J75',  
    'HLT_ht850_L1J100',
]

JET_2018 = [
    'HLT_j420',
]
JET_2017 = [
    'HLT_j450',
    'HLT_j420',
    'HLT_j400',
    'HLT_j380',
]
JET_2016 = [
    'HLT_j340',
    'HLT_j380',
]
JET_2015 = [
    'HLT_j360',
]

SingleLepton = list(set(SingleLepton_2018+SingleLepton_2017+SingleLepton_2016+SingleLepton_2015))
JET = list(set(JET_2018+JET_2017+JET_2016+JET_2015))
HT = list(set(HT_2018+HT_2017+HT_2016+HT_2015))
MET = list(set(MET_2018+MET_2017+MET_2016+MET_2015))
MultiMuon = list(set(MultiMuon_2018+MultiMuon_2017+MultiMuon_2016+MultiMuon_2015))

triggersNavThin = (MultiMuon + SingleLepton + HT + MET + JET)
triggersNavThin_OR = ' || '.join(triggersNavThin)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT28ThinningHelper = ThinningHelper("EXOT28ThinningHelper")
EXOT28ThinningHelper.TriggerChains = ' | '.join(triggersNavThin)
EXOT28ThinningHelper.AppendToStream(EXOT28Stream)

#=======================================
# SKIMMING   
#=======================================
expression_skimming = triggersNavThin_OR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT28StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT28StringSkimmingTool", expression = expression_skimming)
ToolSvc += EXOT28StringSkimmingTool

#=======================================
# THINNING
#=======================================
thinningTools=[]
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT28TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "EXOT28TPThinningTool",
                                                                  ThinningService         = EXOT28ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = "InDetTrackParticles.eta > -2.7 && InDetTrackParticles.eta < 2.7 && " + 
                                                                                            "InDetTrackParticles.pt > 0.4*GeV && " +
                                                                                            "abs(DFCommonInDetTrackZ0AtPV) < 10*mm", 
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT28TPThinningTool
thinningTools.append(EXOT28TPThinningTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT28ElectronsThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT28ElectronsThinningTool",
                                                                         ThinningService = EXOT28ThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "Electrons",
                                                                         ApplyAnd        = False,
                                                                         SelectionString = "Electrons.eta>-2.5 && Electrons.eta<2.5")
ToolSvc += EXOT28ElectronsThinningTool
thinningTools.append(EXOT28ElectronsThinningTool)

applyJetCalibration_xAODColl('AntiKt4EMPFlow')
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT28JetsThinningTool = DerivationFramework__GenericObjectThinning(name                 = "EXOT28JetsThinningTool",
                                                                         ThinningService = EXOT28ThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "AntiKt4EMPFlowJets",
                                                                         ApplyAnd        = False,
                                                                         SelectionString = "AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 15*GeV && " + 
                                                                                           "AntiKt4EMPFlowJets.DFCommonJets_Calib_eta >-4.5 && " +
                                                                                           "AntiKt4EMPFlowJets.DFCommonJets_Calib_eta <4.5")
ToolSvc += EXOT28JetsThinningTool
thinningTools.append(EXOT28JetsThinningTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT28MuonsThinningTool = DerivationFramework__GenericObjectThinning(name          = "EXOT28MuonsThinningTool",
                                                                     ThinningService = EXOT28ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "Muons",
                                                                     ApplyAnd        = False,
                                                                     SelectionString = "Muons.eta>-2.5 && Muons.eta<2.5")
ToolSvc += EXOT28MuonsThinningTool
thinningTools.append(EXOT28MuonsThinningTool)

#Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'],Sequencer = EXOT28Seq)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += EXOT28Seq
EXOT28Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT28Kernel_skim", SkimmingTools = [EXOT28StringSkimmingTool])
EXOT28Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT28Kernel", ThinningTools = thinningTools)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT28ContentList import *
EXOT28SlimmingHelper = SlimmingHelper("EXOT28SlimmingHelper")
EXOT28SlimmingHelper.SmartCollections = EXOT28SmartContent 
EXOT28SlimmingHelper.AllVariables = EXOT28AllVariablesContent 
EXOT28SlimmingHelper.StaticContent = EXOT28UnslimmedContent 
EXOT28SlimmingHelper.ExtraVariables = EXOT28ExtraVariables
EXOT28SlimmingHelper.IncludeMuonTriggerContent = True
EXOT28SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT28SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT28SlimmingHelper.IncludeJetTriggerContent = True # this will cover HT too? 

EXOT28SlimmingHelper.AppendContentToStream(EXOT28Stream)
