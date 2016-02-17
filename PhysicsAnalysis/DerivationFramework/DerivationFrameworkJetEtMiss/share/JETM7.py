#====================================================================
# JETM7.py 
# reductionConf flag JETM7 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
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
elofflinesel = andstr.join(['count((Electrons.pt > 25*GeV) && (Electrons.DFCommonElectronsLHMedium)) >= 1',
                            'count((Electrons.pt > 10*GeV) && (Electrons.DFCommonElectronsLHMedium)) <= 1',
                            'count(AntiKt4EMTopoJets.pt > 20*GeV) >=1',
                            'count(BTagging_AntiKt4EMTopo.MV2c20_discriminant > 0.0314) >=1'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

print "JETM7 EL:", electronSelection

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 25*GeV) && (Muons.DFCommonMuonsMedium)) >= 1',
                            'count((Muons.pt > 10*GeV) && (Muons.DFCommonMuonsMedium)) <= 1',
                            'count(AntiKt4EMTopoJets.pt > 20*GeV) >=1',
                            'count(BTagging_AntiKt4EMTopo.MV2c20_discriminant > 0.0314) >=1'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') )'

print "JETM7 MU:", muonSelection

expression = '( ' + electronSelection + ' || ' + muonSelection + ' )'
print "JETM7 EXP:", expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM7SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM7SkimmingTool1",
                                                                 expression = expression)
ToolSvc += JETM7SkimmingTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM7Stream )
JETM7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM7Stream.AcceptAlgs(["JETM7Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM7ThinningHelper = ThinningHelper( "JETM7ThinningHelper" )
JETM7ThinningHelper.TriggerChains = orstr.join(electronTriggers+muonTriggers)
JETM7ThinningHelper.AppendToStream( JETM7Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM7MuonTPThinningTool",
                                                                    ThinningService         = JETM7ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM7MuonTPThinningTool
thinningTools.append(JETM7MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM7ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM7ElectronTPThinningTool",
                                                                               ThinningService         = JETM7ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM7ElectronTPThinningTool
thinningTools.append(JETM7ElectronTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM7TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM7TauTPThinningTool",
                                                                        ThinningService = JETM7ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM7TauTPThinningTool
thinningTools.append(JETM7TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM7TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM7TruthThinningTool",
                                                                        ThinningService        = JETM7ThinningHelper.ThinningSvc(),
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM7TruthThinningTool
    thinningTools.append(JETM7TruthThinningTool)    

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm7Seq = CfgMgr.AthSequencer("JETM7Sequence")
DerivationFrameworkJob += jetm7Seq

#==============================================================================
# SUSY background generator filters
#==============================================================================
augmentationTools = []
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(jetm7Seq)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "JETM7GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  augmentationTools.append(ToolSvc.JETM7GenFilt)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm7Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM7Kernel", 
                                                          SkimmingTools = [JETM7SkimmingTool],
                                                          ThinningTools = thinningTools,
                                                          AugmentationTools = augmentationTools)

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

if globalflags.DataSource()=='geant4':
    addMETTruthMap('AntiKt4EMTopo')
    addMETTruthMap('AntiKt4LCTopo')
    addMETTruthMap('AntiKt4EMPFlow')
    scheduleMETAssocAlg(jetm7Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM7SlimmingHelper = SlimmingHelper("JETM7SlimmingHelper")
JETM7SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices"]
JETM7SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo", "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments"
                                    ]
JETM7SlimmingHelper.ExtraVariables = ["Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus"]
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM7SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM7SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM7SlimmingHelper.IncludeMuonTriggerContent = True
JETM7SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM7SlimmingHelper,["SmallR"])
# Add the MET containers to the stream
addMETOutputs(JETM7SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track"])

JETM7SlimmingHelper.AppendContentToStream(JETM7Stream)
