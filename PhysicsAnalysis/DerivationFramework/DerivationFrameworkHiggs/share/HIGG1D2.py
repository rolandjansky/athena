#====================================================================
# HIGG1D2.py
# This requires the reductionConf flag HIGG1D2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkCore.LHE3WeightMetadata import *

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    addStandardTruthContents()
    addPVCollection()
    print "HIGG1D1.py Applying MCTruthCommon"

#====================================================================
# SKIMMING TOOLS
#====================================================================


#====================================================================
# eegamma selection, di-electron triggers
# two opposite-sign medium el, pT>10 GeV, |eta|<2.5, mee>40 GeV
# gamma: reco, ET>10 GeV< |eta|<2.5
#====================================================================

if RecomputeElectronSelectors :
    requirementEl = '(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose) && abs(Electrons.eta)<2.5 && (Electrons.pt > 9.5*GeV)'
else :
    requirementEl = '(Electrons.Loose || Electrons.DFCommonElectronsLHLoose) && abs(Electrons.eta)<2.5 && (Electrons.pt > 9.5*GeV)'


requirementMu = 'Muons.pt>9.5*GeV && abs(Muons.eta)<2.7 && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
HIGG1D2_EEMassTool = DerivationFramework__EGInvariantMassTool( name = "HIGG1D2_EEMassTool",
                                                             Object1Requirements = requirementEl,
                                                             Object2Requirements = requirementEl,
                                                             StoreGateEntryName = "HIGG1D2_DiElectronMass",
                                                             Mass1Hypothesis = 0.511*MeV,
                                                             Mass2Hypothesis = 0.511*MeV,
                                                             Container1Name = "Electrons",
                                                             Container2Name = "Electrons",
                                                             Pt1BranchName = "DFCommonElectrons_pt",
                                                             Eta1BranchName = "DFCommonElectrons_eta",
                                                             Phi1BranchName = "DFCommonElectrons_phi",
                                                             Pt2BranchName = "DFCommonElectrons_pt",
                                                             Eta2BranchName = "DFCommonElectrons_eta",
                                                             Phi2BranchName = "DFCommonElectrons_phi",
                                                             CheckCharge = True,
                                                             DoTransverseMass = False,
                                                             MinDeltaR = 0.0)
ToolSvc += HIGG1D2_EEMassTool
print HIGG1D2_EEMassTool

HIGG1D2_MuMuMassTool = DerivationFramework__EGInvariantMassTool( name = "HIGG1D2_MuMuMassTool",
                                                               Object1Requirements = requirementMu,
                                                               Object2Requirements = requirementMu,
                                                               StoreGateEntryName = "HIGG1D2_DiMuonMass",
                                                               Mass1Hypothesis = 105*MeV,
                                                               Mass2Hypothesis = 105*MeV,
                                                               Container1Name = "Muons",
                                                               Container2Name = "Muons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += HIGG1D2_MuMuMassTool
print HIGG1D2_MuMuMassTool


#====================================================================
# SKIMMING TOOL
#====================================================================
expression = '(count(abs(DFCommonPhotons_eta)<2.5 && DFCommonPhotons_et>9.5*GeV)>=1 && (count(HIGG1D2_DiElectronMass > 40.0*GeV)>=1 || count(HIGG1D2_DiMuonMass > 40.0*GeV)>=1)) || (count(DFCommonPhotons_et>500*GeV && abs(DFCommonPhotons_eta)<2.4 && Photons.DFCommonPhotonsIsEMLoose)>=1)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG1D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG1D2SkimmingTool",
                                                               expression = expression)
ToolSvc += HIGG1D2SkimmingTool
print "HIGG1D2 skimming tool:", HIGG1D2SkimmingTool



#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D2MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D2MetTPThinningTool",
                                                                       ThinningService         = "HIGG1D2ThinningSvc",
                                                                       SelectionString         = thinning_expression,
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       ApplyAnd                = True)
ToolSvc += HIGG1D2MetTPThinningTool
print HIGG1D2MetTPThinningTool
#thinningTools.append(HIGG1D2MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D2JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D2JetLCTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D2JetLCTPThinningTool
print HIGG1D2JetLCTPThinningTool
#thinningTools.append(HIGG1D2JetLCTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D2JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D2JetEMTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D2JetEMTPThinningTool
print HIGG1D2JetEMTPThinningTool
#thinningTools.append(HIGG1D2JetEMTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "HIGG1D2MuonTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2MuonTPThinningTool
print HIGG1D2MuonTPThinningTool
#thinningTools.append(HIGG1D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2ElectronTPThinningTool",
                                                                                  ThinningService         = "HIGG1D2ThinningSvc",
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2ElectronTPThinningTool
print HIGG1D2ElectronTPThinningTool
#thinningTools.append(HIGG1D2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2PhotonTPThinningTool",
                                                                                  ThinningService         = "HIGG1D2ThinningSvc",
                                                                                  SGKey                   = "Photons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2PhotonTPThinningTool
print HIGG1D2PhotonTPThinningTool
#thinningTools.append(HIGG1D2PhotonTPThinningTool)

# Tracks themselves
HIGG1D2TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D2TPThinningTool",
                                                                    ThinningService         = "HIGG1D2ThinningSvc",
                                                                    SelectionString             = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2TPThinningTool
print HIGG1D2TPThinningTool
#thinningTools.append(HIGG1D2TPThinningTool)



# Truth thinning
truth_cond_1    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3  = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D2TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D2TruthThinningTool", 
                                                                      ThinningService         = "HIGG1D2ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG1D2.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D2TruthThinningTool
    thinningTools.append(HIGG1D2TruthThinningTool)
print "HIGG1D2.py thinningTools", thinningTools

# Create private sequence
# The name of the kernel  must be unique to this derivation
HIGG1D2Seq = CfgMgr.AthSequencer("HIGG1D2Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D2Kernel",
                                                                       AugmentationTools = [HIGG1D2_EEMassTool,HIGG1D2_MuMuMassTool],
                                                                       SkimmingTools = [HIGG1D2SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                      )


# Before any custom jet reconstruction, it's good to set up the output list
OutputJets["HIGG1D2Jets"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = [
                  "AntiKt4TruthJets",
                  "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList,HIGG1D2Seq,"HIGG1D2Jets")

DerivationFrameworkJob += HIGG1D2Seq

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D2Stream )
HIGG1D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D2Stream.AcceptAlgs(["HIGG1D2Kernel"])


# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG1D2ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D2SlimmingHelper = SlimmingHelper("HIGG1D2SlimmingHelper")
HIGG1D2Stream.AddItem("xAOD::EventShape#*")
HIGG1D2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

HIGG1D2SlimmingHelper.AppendToDictionary = {'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPrimaryVertices': 'xAOD::VertexContainer','TruthPrimaryVerticesAux': 'xAOD::VertexAuxContainer'}

HIGG1D2SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D2SlimmingHelper.AllVariables = ["Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthElectrons","TruthPhotons","TruthMuons","TruthBoson","TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","PrimaryVertices","MET_Truth", "MET_Track", "egammaTruthParticles","CaloCalTopoClusters"]

HIGG1D2SlimmingHelper.ExtraVariables = ["Muons.quality.EnergyLoss.energyLossType.etcone20.ptconecoreTrackPtrCorrection",
                                        "MuonClusterCollection.eta_sampl.phi_sampl",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.PartonTruthLabelID.Jvt.JVFCorr.JvtRpt.ConstituentScale",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "BTagging_AntiKt4EMTopo.MV1_discriminant",                                        
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "MuonTruthParticles.truthOrigin.truthType"]

# Add AODCellContainer (have to find how to keep only cells belonging to e/gamma objects)
# Usage of AddItem is forbidden
# HIGG1D2Stream.AddItem("CaloCellContainer#AODCellContainer")

HIGG1D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG1D2SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D2SlimmingHelper.AppendContentToStream(HIGG1D2Stream)

