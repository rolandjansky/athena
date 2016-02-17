#====================================================================
# JETM6.py 
# reductionConf flag JETM6 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

electronTriggers = [
    # EnhancedBias
    'HLT_noalg_L1EM18VH', 'HLT_noalg_L1EM15',  'HLT_noalg_L1EM12',
    'HLT_noalg_L12EM7',   'HLT_noalg_L12EM15', 'HLT_noalg_L1EM15VH_3EM7',
    # Week 1
    'HLT_e17_lhloose_L1EM15',
    # LLH
    'HLT_e24_lhtight_iloose', 'HLT_e26_lhtight_iloose',
    'HLT_e60_lhmedium',
    'HLT_e24_lhmedium_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM18VH',
    # Cut-based
    'HLT_e24_tight_iloose',   'HLT_e26_tight_iloose',
    'HLT_e60_medium',
    'HLT_e24_medium_iloose_L1EM18VH',
    'HLT_e24_medium_iloose_L1EM20VH',
    ]

muonTriggers = [
    # EnhancedBias
    'HLT_noalg_L1MU4',     'HLT_noalg_L1MU6',     'HLT_noalg_L1MU10',
    'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU4_3J15',
    'HLT_noalg_L1MU6_J20', 'HLT_noalg_L12MU4',
    #Week 1
    'HLT_mu10',
    # All-year
    'HLT_mu20_iloose_L1MU15',  'HLT_mu24_imedium',
    'HLT_mu24_iloose_L1MU15',  'HLT_mu26_imedium',
    'HLT_mu50', 'HLT_mu60_0eta105_msonly'
    ]

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium)) == 1',
                            'count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsMedium)) < 1',
                            'count(CamKt12LCTopoJets.pt > 75*GeV) >=1'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsMedium)) == 1',
                            'count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium)) < 1',
                            'count(CamKt12LCTopoJets.pt > 75*GeV) >=1'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
# MET filter wanted? : MET_Reference_AntiKt4LCTopo > 20*GeV # should use a different container

expression = '( ' + electronSelection + ' || ' + muonSelection + ' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM6SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM6SkimmingTool1",
                                                                 expression = expression)
ToolSvc += JETM6SkimmingTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM6Stream )
JETM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM6Stream.AcceptAlgs(["JETM6Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM6ThinningHelper = ThinningHelper( "JETM6ThinningHelper" )
JETM6ThinningHelper.TriggerChains = orstr.join(electronTriggers+muonTriggers)
JETM6ThinningHelper.AppendToStream( JETM6Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM6MuonTPThinningTool",
                                                                         ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6MuonTPThinningTool
thinningTools.append(JETM6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6ElectronTPThinningTool",
                                                                               ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6ElectronTPThinningTool
thinningTools.append(JETM6ElectronTPThinningTool)

# # TrackParticles associated with taus
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
# JETM6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM6TauTPThinningTool",
#                                                                         ThinningService = JETM6ThinningHelper.ThinningSvc(),
#                                                                         TauKey          = "TauJets",
#                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
# ToolSvc += JETM6TauTPThinningTool
# thinningTools.append(JETM6TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM6TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM6TruthThinningTool",
                                                                        ThinningService        = JETM6ThinningHelper.ThinningSvc(),
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM6TruthThinningTool
    thinningTools.append(JETM6TruthThinningTool)    

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

jetm6Seq = CfgMgr.AthSequencer("JETM6Sequence")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM6Kernel", 
                                                                SkimmingTools = [JETM6SkimmingTool],
                                                                ThinningTools = thinningTools)

#=======================================
# ATTACH TO THE MAIN SEQUENCE
#=======================================

DerivationFrameworkJob += jetm6Seq

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm6Seq,"JETM6")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM6SlimmingHelper = SlimmingHelper("JETM6SlimmingHelper")
JETM6SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices"]
JETM6SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo",
                                    "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments"
                                    ]
#JETM6SlimmingHelper.ExtraVariables = []
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM6SlimmingHelper.IncludeMuonTriggerContent = True
JETM6SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM6SlimmingHelper,["SmallR","LargeR","JETM6"])
# Add the MET containers to the stream
addMETOutputs(JETM6SlimmingHelper,["Diagnostic","AntiKt4LCTopo","AntiKt4EMPFlow","Track"])

JETM6SlimmingHelper.AppendContentToStream(JETM6Stream)
