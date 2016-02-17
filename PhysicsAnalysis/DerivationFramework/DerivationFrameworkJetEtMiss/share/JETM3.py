#====================================================================
# JETM3.py 
# reductionConf flag JETM3 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *

#
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
    # Dielectron
    'HLT_2e17_lhloose',
    'HLT_2e15_lhloose_L12EM13VH',
    'HLT_2e12_lhloose_L12EM10VH',
    # Cut-based
    'HLT_e24_tight_iloose',   'HLT_e26_tight_iloose',
    'HLT_e60_medium',
    'HLT_e24_medium_iloose_L1EM18VH',
    'HLT_e24_medium_iloose_L1EM20VH',
    # Dielectron
    'HLT_2e17_loose',
    'HLT_2e15_loose_L12EM13VH',
    'HLT_2e12_loose_L12EM10VH'
    ]

muonTriggers = [
    # EnhancedBias
    'HLT_noalg_L1MU4',     'HLT_noalg_L1MU6',     'HLT_noalg_L1MU10',
    'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU4_3J15',
    'HLT_noalg_L1MU6_J20', 'HLT_noalg_L12MU4',
    # Week 1
    'HLT_mu10',
    # All-year
    'HLT_mu20_iloose_L1MU15',  'HLT_mu24_imedium',
    'HLT_mu24_iloose_L1MU15',  'HLT_mu26_imedium',
    'HLT_mu50',   'HLT_mu60_0eta105_msonly'
    # Dimuon
    'HLT_2mu10',  'HLT_2mu14',
    'HLT_mu18_mu8noL1',  'HLT_mu20_mu8noL1',
    'HLT_mu22_mu8noL1',  'HLT_mu24_mu8noL1',
    ]

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.charge > 0.) && (Electrons.DFCommonElectronsLHMedium)) >= 1',
                            'count((Electrons.pt > 20*GeV) && (Electrons.charge < 0.) && (Electrons.DFCommonElectronsLHMedium)) >= 1'
                           ])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.charge > 0.) && (Muons.DFCommonMuonsMedium)) >=1',
                            'count((Muons.pt > 20*GeV) && (Muons.charge < 0.) && (Muons.DFCommonMuonsMedium)) >=1'
                            ])
muonSelection = '( (' + mutrigsel + ') && (' + muofflinesel + ') )'
# NOTE: muon and electron charges requested...
expression = '( ' + electronSelection + ' || ' + muonSelection + ' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM3SkimmingTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM3Stream )
JETM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM3Stream.AcceptAlgs(["JETM3Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM3ThinningHelper = ThinningHelper( "JETM3ThinningHelper" )
JETM3ThinningHelper.TriggerChains = orstr.join(electronTriggers+muonTriggers)
JETM3ThinningHelper.AppendToStream( JETM3Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM3MuonTPThinningTool",
                                                                    ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3MuonTPThinningTool
thinningTools.append(JETM3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3ElectronTPThinningTool",
                                                                               ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3ElectronTPThinningTool
thinningTools.append(JETM3ElectronTPThinningTool)

# TrackParticles associated with electrons
JETM3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3PhotonTPThinningTool",
                                                                             ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3PhotonTPThinningTool
thinningTools.append(JETM3PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM3TauTPThinningTool",
                                                                        ThinningService = JETM3ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3TauTPThinningTool
thinningTools.append(JETM3TauTPThinningTool)

thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM3TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM3TPThinningTool",
                                                                  ThinningService         = "JETM3ThinningSvc",
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3TPThinningTool
thinningTools.append(JETM3TPThinningTool)


# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5 && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM3TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM3TruthThinningTool",
                                                                        ThinningService        = JETM3ThinningHelper.ThinningSvc(),
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM3TruthThinningTool
    thinningTools.append(JETM3TruthThinningTool)    

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm3Seq = CfgMgr.AthSequencer("JETM3Sequence")
DerivationFrameworkJob += jetm3Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm3Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM3Kernel",
                                                                SkimmingTools = [JETM3SkimmingTool],
                                                                ThinningTools = thinningTools)

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

if globalflags.DataSource()=='geant4':
    addMETTruthMap('AntiKt4EMTopo')
    addMETTruthMap('AntiKt4LCTopo')
    addMETTruthMap('AntiKt4EMPFlow')
    scheduleMETAssocAlg(jetm3Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM3SlimmingHelper = SlimmingHelper("JETM3SlimmingHelper")
JETM3SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices"]
JETM3SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo", "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments"
                                    ]
JETM3SlimmingHelper.ExtraVariables = ["Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus"]
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM3SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM3SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM3SlimmingHelper.IncludeMuonTriggerContent = True
JETM3SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM3SlimmingHelper,["SmallR"])
# Add the MET containers to the stream
addMETOutputs(JETM3SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track"])

JETM3SlimmingHelper.AppendContentToStream(JETM3Stream)
