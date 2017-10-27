#********************************************************************
# STDM7.py 
# reductionConf flag STDM7 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.EGammaCommon import *
import AthenaCommon.SystemOfUnits as Units
from DerivationFrameworkSM import STDMTriggers

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#====================================================================
# SET UP STREAM
#====================================================================

streamName = derivationFlags.WriteDAOD_STDM7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM7Stream )
STDM7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM7Stream.AcceptAlgs(["STDM7Kernel"])


#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM7ThinningHelper = ThinningHelper( "STDM7ThinningHelper" )

#trigger navigation content
STDM7ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM7ThinningHelper.AppendToStream( STDM7Stream )


#===================== 
# TRACK  THINNING
#=====================  

# removed for full tracking info...



# Truth leptons and their ancestors and descendants + final-state hadrons
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"
# Truth hadrons for UE analysis
truth_cond_hadrons = "( (TruthParticles.status ==1) && (TruthParticles.barcode<200000) )"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning


if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *
   
    STDM7TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM7TruthLepTool",
                                                                  ThinningService              = STDM7ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)
   
    STDM7TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM7TruthBosTool",
                                                                  ThinningService              = STDM7ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)
   
    STDM7PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM7PhotonThinning",
                                                                    ThinningService         = STDM7ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "STDMTruthPhotons",
                                                                    ParticleSelectionString = STDMphotonthinningexpr)

    STDM7TruthHadTool = DerivationFramework__GenericTruthThinning(name                         = "STDM7TruthHadTool",
                                                                  ThinningService              = STDM7ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_hadrons,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = False)

    
    ToolSvc += STDM7TruthLepTool
    ToolSvc += STDM7TruthBosTool
    ToolSvc += STDM7PhotonThinning
    ToolSvc += STDM7TruthHadTool
    thinningTools.append(STDM7TruthLepTool)
    thinningTools.append(STDM7TruthBosTool)
    thinningTools.append(STDM7PhotonThinning)
    thinningTools.append(STDM7TruthHadTool)
    
#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 4*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 11*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.DFCommonElectronsIsEMLoose) || (Electrons.DFCommonElectronsLHLoose))'
muonOnlySelection = 'count('+muonsRequirements+') >=2'
electronOnlySelection = 'count('+electronsRequirements+') >= 2'
electronMuonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 2'
offlineexpression = '('+muonOnlySelection+' || '+electronOnlySelection+' || '+electronMuonSelection+')'

MuonTriggerRequirement='( HLT_2mu10 || HLT_2mu14 ||  HLT_2mu6_10invm30_pt2_z10  || HLT_mu4_iloose_mu4_11invm60_noos || HLT_mu4_iloose_mu4_11invm60_noos_novtx || HLT_mu4_iloose_mu4_7invm9_noos || HLT_mu4_iloose_mu4_7invm9_noos_novtx || HLT_mu6_iloose_mu6_11invm24_noos || HLT_mu6_iloose_mu6_11invm24_noos_novtx || HLT_mu6_iloose_mu6_24invm60_noos || HLT_mu6_iloose_mu6_24invm60_noos_novtx || HLT_mu18_mu8noL1 || HLT_mu20_iloose_L1MU15 || HLT_2mu6_bBmumu || HLT_mu6_mu4_bBmumu ||  HLT_2mu4_bUpsimumu ||  HLT_mu6_mu4_bUpsimumu || HLT_2mu6_bUpsimumu || HLT_mu4_iloose_mu4_11invm60_noos_L1_MU6_2MU4 || HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1_MU6_2MU4 || HLT_mu4_iloose_mu4_7invm9_noos_L1_MU6_2MU4 || HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1_MU6_2MU4 || HLT_mu24_imedium || HLT_mu26_ivarmedium || HLT_mu50 || HLT_mu24_iloose || HLT_mu24_iloose_L1MU15 )'
triggerRequirement="(" + STDMTriggers.list_combine_OR(STDMTriggers.single_e_triggers + STDMTriggers.multi_e_triggers + STDMTriggers.multi_mu_triggers + STDMTriggers.mixed_emu_triggers) + " || " + MuonTriggerRequirement + ")"

expression = triggerRequirement+' && '+offlineexpression
print "STDM7 skimming expression: ",expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM7SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM7SkimmingTool",
                                                                 expression = expression)
ToolSvc += STDM7SkimmingTool


#=====================================================
# CREATE AND SCHEDULE THE DERIVATION KERNEL ALGORITHM   
#=====================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE
STDM7Sequence = CfgMgr.AthSequencer("STDM7Sequence")

# ADD KERNEL
STDM7Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM7Kernel",
                                                              SkimmingTools = [STDM7SkimmingTool],
                                                              ThinningTools = thinningTools)

# ADD SEQUENCE TO JOB
DerivationFrameworkJob += STDM7Sequence

#====================================================================
# SET UP STREAM   
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM7Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM7Stream )
#STDM7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM7Stream.AcceptAlgs(["STDM7Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="STDM7ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM7SlimmingHelper = SlimmingHelper("STDM7SlimmingHelper")
STDM7SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM7SlimmingHelper.IncludeEGammaTriggerContent = True
STDM7SlimmingHelper.IncludeMuonTriggerContent = True


STDM7SlimmingHelper.ExtraVariables = ExtraContentAll
STDM7SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"]

# btagging variables
from  DerivationFrameworkFlavourTag.BTaggingContent import *
STDM7SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMTopoJets")

ExtraDictionary["BTagging_AntiKt4EMTopo"]    = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt4EMTopoAux"] = "xAOD::BTaggingAuxContainer"

STDM7SlimmingHelper.AllVariables = ExtraContainersAll
STDM7SlimmingHelper.AllVariables += ["Electrons"]

if globalflags.DataSource()=='geant4':
    STDM7SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM7SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM7SlimmingHelper.AppendToDictionary.update(ExtraDictionary)

    
STDM7SlimmingHelper.AppendContentToStream(STDM7Stream)

