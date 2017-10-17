#********************************************************************
# EXOT21.py 
# reductionConf flag EXOT21 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT21Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT21Stream )
EXOT21Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT21Stream.AcceptAlgs(["EXOT21Kernel"])

exot21Seq = CfgMgr.AthSequencer("EXOT21Sequence")

triggerNavChains  = ""
triggerNavChains += "HLT_3mu6.*|"
triggerNavChains += "HLT_mu26_imedium.*|HLT_mu24_imedium.*|HLT_mu50|HLT_mu40|HLT_mu26|HLT_mu24.*|HLT_2mu14.*|HLT_2mu10.*|"
triggerNavChains += "HLT_e17_lhloose.*|HLT_e60_lhmedium.*|HLT_e24_lhmedium.*|HLT_e24_lhtight_iloose.*|HLT_e26_lhtight_iloose.*|"
triggerNavChains += "HLT_j80_xe80.*|HLT_g15_loose_2mu10_msonly.*|HLT_g35_medium_g25_medium.*|"
triggerNavChains += "HLT_g35_loose_L1EM15_g25_loose_L1EM15|HLT_2e12_lhloose_L12EM10VH|HLT_e17_lhloose_2e9_lhloose|"
triggerNavChains += "HLT_j150 | HLT_j175 | HLT_j380 | HLT_j340 | HLT_3j175 | HLT_3j200"

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT21ThinningHelper = ThinningHelper("EXOT21ThinningHelper")
EXOT21ThinningHelper.TriggerChains = triggerNavChains
EXOT21ThinningHelper.AppendToStream(EXOT21Stream)

##=======================================
# SKIMMING   
#=======================================
# main triggers
expression = 'HLT_3mu6_msonly'
# muon triggers
expression += ' || HLT_mu26_imedium || HLT_mu24_imedium || HLT_mu50 || HLT_mu40 || HLT_mu26 || HLT_mu24 || HLT_2mu14 || HLT_2mu10'
# associatedHiggs and iDM triggers
expression += ' || HLT_e17_lhloose || HLT_e60_lhmedium || HLT_e24_lhmedium_iloose || HLT_e24_lhtight_iloose || HLT_e26_lhtight_iloose || HLT_j80_xe80'
# other triggers
expression += ' || HLT_g15_loose_2mu10_msonly || HLT_g35_medium_g25_medium'
expression += ' || HLT_g35_loose_L1EM15_g25_loose_L1EM15 || HLT_2e12_lhloose_L12EM10VH || HLT_e17_lhloose_2e9_lhloose'
# jet triggers
expression += ' || HLT_j150 || HLT_j175 || HLT_j380 || HLT_j340 || HLT_3j175 || HLT_3j200'
# MC triggers, which don't exist in data
expression += '|| HLT_e24_lhmedium_nodeta_L1EM18VH || HLT_e24_lhmedium_nodphires_L1EM18VH || HLT_e24_lhmedium_cutd0dphideta_L1EM18VH || HLT_e24_lhmedium_cutd0dphideta_L1EM20VH || HLT_e24_lhmedium_ivarloose_L1EM18VHL1_EM18VH || HLT_e24_lhmedium_nod0_ivarloose_L1EM18VH || HLT_e24_lhmedium_nod0_ringer_L1EM20VH || HLT_e24_lhmedium_nod0_ringer_iloose || HLT_e24_lhmedium_nod0_ringer_ivarloose || HLT_mu24_imediumcalo || HLT_2mu14_noMuCombOvlpRm || HLT_2mu14_l2msonly || HLT_e17_lhloose_L1EM15VHJ1523ETA49 ||HLT_e17_lhloose_L1EM15VHJJ1523ETA49 || HLT_e60_lhmedium_cutd0dphideta || HLT_e24_lhmedium_iloose_L1EM18VH || HLT_j80_xe80_dphi1'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT21StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT21StringSkimmingTool", expression = expression)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT21TrigDashSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT21TrigDashSkimmingTool",
                                                                      TriggerListOR = [
                                                                          "HLT_j30_jes_PS_llp_L1LLP-NOMATCH",
                                                                          "HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH",
                                                                          "HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50",
                                                                          "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50",
                                                                          "HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50",
                                                                          "HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50"])
ToolSvc += EXOT21TrigDashSkimmingTool
ToolSvc += EXOT21StringSkimmingTool
print EXOT21TrigDashSkimmingTool
print EXOT21StringSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EXOT21triggerORTool=DerivationFramework__FilterCombinationOR(name="EXOT21triggerORTool", FilterList=[EXOT21TrigDashSkimmingTool,EXOT21StringSkimmingTool])
ToolSvc += EXOT21triggerORTool
print EXOT21triggerORTool


#=======================================
# THINNING
#=======================================

thinningTools=[]

# Thin all inner detector tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT21TPThinningTool = DerivationFramework__TrackParticleThinning(name                    =  "EXOT21TPThinningTool",
                                                                  ThinningService = EXOT21ThinningHelper.ThinningSvc(),
                                                                  SelectionString         =  "InDetTrackParticles.pt > 0.5*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5",
                                                                  InDetTrackParticlesKey  =  "InDetTrackParticles")
ToolSvc += EXOT21TPThinningTool
thinningTools.append(EXOT21TPThinningTool)


#Truth thinning
truth_expression = "(abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 3000005 || abs(TruthParticles.pdgId) == 3000005 || abs(TruthParticles.pdgId) == 700021 || abs(TruthParticles.pdgId) == 700022)"


from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT21MCGenThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EXOT21MCGenThinningTool",
                                                                   ThinningService         = EXOT21ThinningHelper.ThinningSvc(),
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = True,
                                                                   #PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)
if globalflags.DataSource()=='geant4':
	ToolSvc += EXOT21MCGenThinningTool
	thinningTools.append(EXOT21MCGenThinningTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot21Seq
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel_skim", SkimmingTools = [EXOT21triggerORTool])
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel", ThinningTools = thinningTools)

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT21ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT21
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT21ContentList import *
EXOT21SlimmingHelper = SlimmingHelper("EXOT21SlimmingHelper")
EXOT21SlimmingHelper.SmartCollections = EXOT21SmartContent 
EXOT21SlimmingHelper.AllVariables = EXOT21AllVariablesContent 
EXOT21SlimmingHelper.StaticContent = EXOT21UnslimmedContent 
EXOT21SlimmingHelper.ExtraVariables = EXOT21ExtraVariables
if globalflags.DataSource()=='geant4':
    EXOT21SlimmingHelper.ExtraVariables += EXOT21ExtraTruth
EXOT21SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT21SlimmingHelper.IncludeMuonTriggerContent = True
EXOT21SlimmingHelper.IncludeJetTriggerContent = True
EXOT21SlimmingHelper.AppendContentToStream(EXOT21Stream)
