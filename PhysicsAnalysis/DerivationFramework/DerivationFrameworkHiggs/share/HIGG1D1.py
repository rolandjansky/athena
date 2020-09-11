#==================================================================== 
# HIGG1D1.py
# This requires the reductionConf flag HIGG1D1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

#====================================================================
# PV refitting after removing Z->ee tracks, for vertex studies
#====================================================================

# Creates a vertex container (ZeeRefittedPrimaryVertices) where the type=1 vertex is refitted
# after removing tracks that are associated with Z->ee decay candidates
# Tool runs only for data and Zee MC samples (must be defined in the MCSamples list)
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__ZeeVertexRefittingTool
HIGG1D1_ZeeVertexRefitterTool = DerivationFramework__ZeeVertexRefittingTool( name = "HIGG1D1_ZeeVertexRefitterTool",
                                    ObjectRequirements="(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.*GeV)",
                                    LowMassCut=50*GeV,
                                    RefittedPVContainerName="ZeeRefittedPrimaryVertices",                                    
                                    ElectronContainerName="Electrons",
                                    PVContainerName="PrimaryVertices",
                                    MCSamples = [361106] )
ToolSvc += HIGG1D1_ZeeVertexRefitterTool
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("ZeeVertexRefitKernel", AugmentationTools = [HIGG1D1_ZeeVertexRefitterTool])

#====================================================================
# Diphoton vertex decoration tool
#====================================================================

# Creates a shallow copy of PrimaryVertices (HggPrimaryVertices) for diphoton events
# Must be created before the jetalg in the sequence as it is input to the modified PFlow jets
from RecExConfig.RecFlags  import rec
from egammaRec.Factories import ToolFactory, AlgFactory
import PhotonVertexSelection.PhotonVertexSelectionConf as PVS 

PhotonPointingTool = ToolFactory(PVS.CP__PhotonPointingTool, name = "yyVtxPhotonPointingTool", isSimulation = rec.doTruth() )
PhotonVertexSelectionTool = ToolFactory(PVS.CP__PhotonVertexSelectionTool, PhotonPointingTool = PhotonPointingTool)

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__DiphotonVertexDecorator
DiphotonVertexDecorator = ToolFactory(DerivationFramework__DiphotonVertexDecorator, PhotonVertexSelectionTool = PhotonVertexSelectionTool)()
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DiphotonVertexKernel", AugmentationTools = [DiphotonVertexDecorator])

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkCore.LHE3WeightMetadata import *

if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    addStandardTruthContents()
    addPVCollection()
    print "HIGG1D1.py Applying MCTruthCommon"


#====================================================================
# SKIMMING TOOLS 
#====================================================================

print "HIGG1D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement
TriggerExp = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerExp               = ["EF_g35_loose_g25_loose"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerExp               = ["HLT_2g50_loose_L12EM20VH","HLT_2g25_loose_g15_loose","HLT_g35_medium_g25_medium_L12EM20VH","HLT_2g25_tight_L12EM20VH","HLT_2g22_tight_L12EM15VHI","HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_2g50_loose","HLT_2g20_tight","HLT_2g22_tight","HLT_2g20_tight_icalovloose_L12EM15VHI","HLT_2g20_tight_icalotight_L12EM15VHI","HLT_2g22_tight_L12EM15VHI","HLT_2g22_tight_icalovloose_L12EM15VHI","HLT_2g22_tight_icalotight_L12EM15VHI","HLT_2g22_tight_icalovloose","HLT_2g25_tight_L12EM20VH","HLT_2g20_loose","HLT_2g20_loose_L12EM15","HLT_g35_medium_g25_medium","HLT_g35_medium_g25_medium_L12EM15VH","HLT_g35_loose_g25_loose","HLT_g35_loose_g25_loose_L12EM15VH", "HLT_2g20_loose_g15_loose", "HLT_3g20_loose", "HLT_3g15_loose", "HLT_2g6_tight_icalotight_L1J100", "HLT_2g6_loose_L1J100", "HLT_2g6_tight_icalotight_L1J50", "HLT_2g6_loose_L1J50","HLT_g120_loose","HLT_g140_loose"]


print "HIGG1D1.py TriggerExp", TriggerExp

KeepTrigger=not SkipTriggerRequirement
#KeepTrigger=False

print "HIGG1D1.py KeepTrigger", KeepTrigger

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
SkimmingToolHIGG1D1 = DerivationFramework__SkimmingToolHIGG1(
                                 name = "SkimmingToolHIGG1D1",
                                 RequireGRL = True,
                                 ReqireLArError = True,
                                 RequireTrigger = KeepTrigger,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False)

ToolSvc += SkimmingToolHIGG1D1
print SkimmingToolHIGG1D1

#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D1MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1MetTPThinningTool",
                                                                       ThinningService         = "HIGG1D1ThinningSvc",
                                                                       SelectionString         = thinning_expression,
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       ApplyAnd                = True)
ToolSvc += HIGG1D1MetTPThinningTool
print HIGG1D1MetTPThinningTool
#thinningTools.append(HIGG1D1MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetLCTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetLCTPThinningTool
print HIGG1D1JetLCTPThinningTool
#thinningTools.append(HIGG1D1JetLCTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetEMTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetEMTPThinningTool
print HIGG1D1JetEMTPThinningTool
#thinningTools.append(HIGG1D1JetEMTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "HIGG1D1MuonTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1MuonTPThinningTool
print HIGG1D1MuonTPThinningTool
#thinningTools.append(HIGG1D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1ElectronTPThinningTool",
                                                                                  ThinningService         = "HIGG1D1ThinningSvc",
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1ElectronTPThinningTool
print HIGG1D1ElectronTPThinningTool
#thinningTools.append(HIGG1D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1PhotonTPThinningTool",
                                                                                ThinningService         = "HIGG1D1ThinningSvc",
                                                                                SGKey                   = "Photons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1PhotonTPThinningTool
print HIGG1D1PhotonTPThinningTool
#thinningTools.append(HIGG1D1PhotonTPThinningTool)

# Tracks themselves
HIGG1D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1TPThinningTool",
                                                                    ThinningService         = "HIGG1D1ThinningSvc",
                                                                    SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1TPThinningTool
print HIGG1D1TPThinningTool
#thinningTools.append(HIGG1D1TPThinningTool)


# Truth thinning
truth_cond_1    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3  = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_5 = "(abs(TruthParticles.pdgId) >=  1000000)" # BSM
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+ ' || '+truth_cond_5+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D1TruthThinningTool", 
                                                                      ThinningService         = "HIGG1D1ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG1D1.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D1TruthThinningTool
    thinningTools.append(HIGG1D1TruthThinningTool)
print "HIGG1D1.py thinningTools", thinningTools

#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HIGG1D1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "HIGG1D1_MaxCellDecoratorTool",
                                                                      SGKey_electrons = "Electrons",
                                                                      SGKey_photons   = "Photons"
                                                                      )
ToolSvc += HIGG1D1_MaxCellDecoratorTool

#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
HIGG1D1_GainDecoratorTool = GainDecorator()

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
HIGG1D1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]

# Create private sequence
# The name of the kernel  must be unique to this derivation
HIGG1D1Seq = CfgMgr.AthSequencer("HIGG1D1Sequence")


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D1Kernel", 
                                                                       SkimmingTools = [SkimmingToolHIGG1D1],
                                                                       AugmentationTools = [HIGG1D1_MaxCellDecoratorTool,HIGG1D1_GainDecoratorTool] + HIGG1D1_ClusterEnergyPerLayerDecorators,
                                                                       ThinningTools = thinningTools
                                                                       )

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
addOriginCorrection("AntiKt4EMTopo", HIGG1D1Seq, "jetaug_EMTopoOriginCorrHggVtx","Hgg")

# Before any custom jet reconstruction, it's good to set up the output list
OutputJets["HIGG1D1Jets"] = []

#=======================================
# AUGMENT JET COLLECTIONS
#=======================================

# Augment AntiKt4 jets with QG tagging variables
truthjetalg='AntiKt4TruthJets'
if not DerivationFrameworkHasTruth:
    truthjetalg=None    
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=HIGG1D1Seq,algname="QGTaggerToolAlg",truthjetalg=truthjetalg)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=HIGG1D1Seq,algname="QGTaggerToolPFAlg",truthjetalg=truthjetalg) 

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=HIGG1D1Seq, algname='JetForwardJvtToolBDTAlg')


#=======================================
# ADD CUSTOM PFLOW JET COLLECTION
#=======================================

# Get AntiKt4PFlowCustomVtxHggJets with fJVT and flavour tagging
include('DerivationFrameworkHiggs/configurePFlowCustomVtxHggJets.py')

#MET associated to HggPrimaryVertices
from DerivationFrameworkJetEtMiss import METCommon
METCommon.scheduleMETCustomVertex ( "Hgg", "AntiKt4EMTopo",outputlist = "TopoHggMET" )
METCommon.scheduleMETCustomVertex ( "Hgg", "AntiKt4PFlowCustomVtx",outputlist = "PFlowHggMET" )
scheduleMETAssocAlg(HIGG1D1Seq)

DerivationFrameworkJob += HIGG1D1Seq


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D1Stream )
HIGG1D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D1Stream.AcceptAlgs(["HIGG1D1Kernel"])

# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG1D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper")

HIGG1D1SlimmingHelper.AppendToDictionary = {'HggPrimaryVertices': 'xAOD::VertexContainer','HggPrimaryVerticesAux': 'xAOD::ShallowAuxContainer',
                                            'ZeeRefittedPrimaryVertices': 'xAOD::VertexContainer', 'ZeeRefittedPrimaryVerticesAux': 'xAOD::VertexAuxContainer',
                                           'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPrimaryVertices': 'xAOD::VertexContainer','TruthPrimaryVerticesAux': 'xAOD::VertexAuxContainer',
                                           'AntiKt4PFlowCustomVtxHggJets': 'xAOD::JetContainer', 'AntiKt4PFlowCustomVtxHggJetsAux': 'xAOD::JetAuxContainer',
                                           'BTagging_AntiKt4PFlowCustomVtxHgg': 'xAOD::BTaggingContainer', 'BTagging_AntiKt4PFlowCustomVtxHggAux': 'xAOD::BTaggingAuxContainer'}

for shape in ['Kt4PFlowCustomVtxEventShape','TruthIsoCentralEventShape','TruthIsoForwardEventShape']:
    HIGG1D1SlimmingHelper.AppendToDictionary[shape]       = 'xAOD::EventShape'
    HIGG1D1SlimmingHelper.AppendToDictionary[shape+'Aux'] = 'xAOD::EventShapeAuxInfo'
    
HIGG1D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
					                                "MET_Reference_AntiKt4EMPFlow",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMPFlowJets",
                                          "AntiKt4EMTopoJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201903",
                                          "BTagging_AntiKt4EMTopo_201810",
                                          "BTagging_AntiKt4EMPFlow_201810",
                                          "BTagging_AntiKt4EMPFlow_201903",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D1SlimmingHelper.AllVariables = ["HLT_xAOD__PhotonContainer_egamma_Iso_Photons","Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","TruthElectrons","TruthPhotons","TruthMuons","TruthTaus","TruthBoson","PrimaryVertices","MET_Truth", "MET_Track","egammaTruthParticles","CaloCalTopoClusters","HggPrimaryVertices","ZeeRefittedPrimaryVertices"]

PFlowJetCommonSlimList = "JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.JVF.LArQuality.LeadingClusterCenterLambda.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterSecondR.N90Constituents.NegativeE.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex.PileupCorrected.Width.AverageLArQF.CentroidR.ConeExclTausFinal.ECPSFraction.EMFrac.GhostAntiKt2TrackJet.GhostAntiKt4TrackJet.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTausFinal.GhostTQuarksFinal.GhostTruth.GhostWBosons.GhostZBosons.HECFrac.HECQuality.HighestJVFVtx.ConstituentScale.TrackWidthPt500.DFCommonJets_Jvt"

QGTaggerList = "DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1"

HIGG1D1SlimmingHelper.ExtraVariables = ["Muons.quality.EnergyLoss.energyLossType",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.PartonTruthLabelID.ConstituentScale.Hgg_JetOriginConstitScaleMomentum_pt.Hgg_JetOriginConstitScaleMomentum_eta.Hgg_JetOriginConstitScaleMomentum_m.Hgg_JetOriginConstitScaleMomentum_phi.Hgg_JetOriginConstitScaleMomentum_OriginVertex.TrackWidthPt500.DFCommonJets_MVfJVT",
                                        "AntiKt4EMPFlowJets."+PFlowJetCommonSlimList,
                                        "AntiKt4EMPFlowJets."+QGTaggerList,
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix", 
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "MuonTruthParticles.truthOrigin.truthType",
                                        "TauChargedParticleFlowObjects.pt.eta.phi.m.e.bdtPi0Score",
                                        "TauHadronicParticleFlowObjects.pt.eta.phi.m.e.bdtPi0Score",
                                        "TauNeutralParticleFlowObjects.pt.eta.phi.m.e.bdtPi0Score",
                                        "TauShotParticleFlowObjects.pt.eta.phi.m.e.bdtPi0Score",
                                        "Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z",
                                        "Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.neflowisol20.neflowisol20ptCorrection.neflowisol30.neflowisol30ptCorrection.neflowisol40.neflowisol40ptCorrection.neflowisolCorrBitset.neflowisolcoreConeEnergyCorrection",
                                        "Kt4PFlowCustomVtxEventShape",
                                        "Kt4PFlowCustomVtxEventShape.Density",
                                        "TruthIsoCentralEventShape",
                                        "TruthIsoCentralEventShape.Density",
                                        "TruthIsoForwardEventShape",
                                        "TruthIsoForwardEventShape.Density"
                                        ]

# Add variables from smart lists for the custom PFlow jets
from DerivationFrameworkJetEtMiss.AntiKt4EMPFlowJetsCPContent import AntiKt4EMPFlowJetsCPContent as pfjc
PFlowCustomVtxContent = []
for i in pfjc:
    if i.find('AntiKt4EMPFlow') >= 0:
        PFlowCustomVtxContent.append(i.replace('EMPFlow','PFlowCustomVtxHgg').replace('Aux',''))
HIGG1D1SlimmingHelper.ExtraVariables += PFlowCustomVtxContent
HIGG1D1SlimmingHelper.ExtraVariables += [CustomPFJetContainerName+'Jets.'+PFlowJetCommonSlimList]

from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
btv = BTaggingStandardContent(CustomPFJetContainerName+'Jets')
BTaggingCustomVtxContent = []
for i in btv:
    BTaggingCustomVtxContent.append(i.replace('Aux',''))
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingCustomVtxContent

# is this really needed given Photons are in the AllVariables list ?
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
HIGG1D1SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(HIGG1D1_GainDecoratorTool) )
for tool in HIGG1D1_ClusterEnergyPerLayerDecorators:
    HIGG1D1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )


addMETOutputs(HIGG1D1SlimmingHelper,["TopoHggMET"])
addMETOutputs(HIGG1D1SlimmingHelper,["PFlowHggMET"])
addMETOutputs(HIGG1D1SlimmingHelper,["AntiKt4EMPFlow"])
HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D1SlimmingHelper.AppendContentToStream(HIGG1D1Stream)
HIGG1D1Stream.AddItem("xAOD::VertexContainer#ZeeRefittedPrimaryVertices")
HIGG1D1Stream.AddItem("xAOD::VertexAuxContainer#ZeeRefittedPrimaryVerticesAux.-vxTrackAtVertex")

# Put the truth density computation (done in EGammaCommon) after jetalg (to have the truth jet input particles prepared)
index = topSequence.getSequence().index('JetAlgorithm/jetalg') 
for a in ['EDTruthCentralAlg','EDTruthForwardAlg']: 
    if hasattr(topSequence,a):  
        edta = getattr(topSequence,a)
        delattr(topSequence,a)
        topSequence.insert(index,edta)

        
