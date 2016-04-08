#********************************************************************
# HIGG8D1.py 
# reductionConf flag HIGG8D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkMCTruth.HFHadronsCommon import *

# testing globalflags
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG8D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG8D1Stream )
HIGG8D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG8D1Stream.AcceptAlgs(["HIGG8D1Kernel"])

# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="HIGG8D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# TAU SELECTOR TOOL 
#====================================================================
augmentationTools = []

from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
HIGG8D1TauWrapper = DerivationFramework__TauSelectionWrapper(name = "HIGG8D1TauSelectionWrapper",
                                                             IsTauFlag			= 19,
                                                             CollectionName		= "TauJets",
                                                             StoreGateEntryName	= "HIGG8D1JetBDTSigLoose")
ToolSvc += HIGG8D1TauWrapper
augmentationTools.append(HIGG8D1TauWrapper)


#=======================================
# Tau truth matching tool
#=======================================

#truth matching
if globalflags.DataSource()=='geant4':
    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
    HIGG8D1TauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="HIGG8D1TauTruthMatchingTool",
                                                                         WriteTruthTaus = True)


    ToolSvc += HIGG8D1TauTruthMatchingTool

    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
    HIGG8D1TauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "HIGG8D1TauTruthMatchingWrapper",
                                                                                TauTruthMatchingTool = HIGG8D1TauTruthMatchingTool,
                                                                                TauContainerName     = "TauJets")

    ToolSvc += HIGG8D1TauTruthMatchingWrapper
    augmentationTools.append(HIGG8D1TauTruthMatchingWrapper)


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG8D1ThinningHelper = ThinningHelper( "HIGG8D1ThinningHelper" )
# Trigger Thinning Tool
HIGG8D1ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_2e.*|HLT_2mu.*|HLT_tau.*'
HIGG8D1ThinningHelper.AppendToStream( HIGG8D1Stream )


# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG8D1MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG8D1MetTPThinningTool",
                                                                      ThinningService        = HIGG8D1ThinningHelper.ThinningSvc(), #"HIGG8D1ThinningSvc
                                                                      SelectionString        = thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG8D1MetTPThinningTool
thinningTools.append(HIGG8D1MetTPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG8D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG8D1JetTPThinningTool",
                                                                ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG8D1JetTPThinningTool
thinningTools.append(HIGG8D1JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG8D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG8D1MuonTPThinningTool",
                                                                           ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1MuonTPThinningTool
thinningTools.append(HIGG8D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG8D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG8D1ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1ElectronTPThinningTool
thinningTools.append(HIGG8D1ElectronTPThinningTool)

# Tracks associated with Taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG8D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "HIGG8D1TauTPThinningTool",
                                                                          ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                          TauKey                  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1TauTPThinningTool
thinningTools.append(HIGG8D1TauTPThinningTool)

# Electrons - calo cluster thinning
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG8D1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "HIGG8D1ElectronCCThinningTool",
                                                                          ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                          SGKey                   = "Electrons",
                                                                          CaloClCollectionSGKey   = "egammaClusters",
                                                                          SelectionString         = "Electrons.pt >= 5*GeV",
                                                                          ConeSize                = 0.4)
ToolSvc += HIGG8D1ElectronCCThinningTool
thinningTools.append(HIGG8D1ElectronCCThinningTool)

#====================================================================
# Truth Thinning
#====================================================================

#Truth tau/nutau and their ancestors and descendants
truth_cond_tau = " ((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000)) "
truth_cond_lep = " ((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000)) "
truth_photon = " ((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV)) "
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+"||"+truth_photon+")"

# PreserveGeneratorDescendants only keeps particles that came directly from the event generator
# PreserveDescendants keeps all particles including those that come from Geant processes

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG8D1TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG8D1TruthTool",
                                                             ThinningService              = HIGG8D1ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

HIGG8D1TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "HIGG8D1TruthTool2",
                                                              ThinningService              = HIGG8D1ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString      = truth_cond_comb,
                                                              PreserveDescendants          = True, # False
                                                              PreserveGeneratorDescendants = False,
                                                              PreserveAncestors            = True,
                                                              TauHandling                  = False)



from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG8D1TruthTool3 = DerivationFramework__MenuTruthThinning(name                      = "HIGG8D1TruthTool3",
                                                           ThinningService            = HIGG8D1ThinningHelper.ThinningSvc(),
                                                           #WritePartons               = True,
                                                           WritePartons               = False,
                                                           WriteHadrons               = False,
                                                           WriteBHadrons              = False,
                                                           WriteGeant                 = False,
                                                           GeantPhotonPtThresh        = -1.0,
                                                           WriteTauHad                = True,
                                                           PartonPtThresh             = -1.0,
                                                           WriteBSM                   = False,
                                                           WriteBosons                = True,
                                                           WriteBSMProducts           = False,
                                                           WriteBosonProducts         = True,
                                                           WriteTopAndDecays          = True,
                                                           WriteEverything            = False,
                                                           WriteAllLeptons            = True,
                                                           WriteStatus3               = False,
                                                           WritettHFHadrons           = True,
                                                           #PreserveGeneratorDescendants = True,
                                                           #PreserveDescendants       = True,
                                                           #PreserveAncestors         = True,
                                                           WriteFirstN                = -1)

if globalflags.DataSource()=='geant4':
    #ToolSvc += HIGG8D1TruthTool
    #thinningTools.append(HIGG8D1TruthTool)
    ToolSvc += HIGG8D1TruthTool2
    thinningTools.append(HIGG8D1TruthTool2)
    ToolSvc += HIGG8D1TruthTool3
    thinningTools.append(HIGG8D1TruthTool3)



#====================================================================
# SKIMMING TOOL 
#====================================================================

#to add: MC event info requirements

eleRequirements = '(Electrons.pt > 5*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.Loose || Electrons.DFCommonElectronsLHLoose)'
leadElectron = eleRequirements + ' && (Electrons.pt > 15*GeV)'
muRequirements = '(Muons.DFCommonGoodMuon) && (Muons.muonType == 0) && (Muons.pt > 5*GeV) && (abs(Muons.eta) < 2.6)'
leadMuon = muRequirements + ' && (Muons.pt > 15*GeV)'
tauRequirements = '(TauJets.pt > 15*GeV) && HIGG8D1JetBDTSigLoose && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3))'

eeSelection = '((count('+eleRequirements+') >= 2) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muRequirements+') >= 2) && (count('+leadMuon+') >= 1))'
emSelection = '(((count('+eleRequirements+') >= 1) && (count('+muRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'

lepSelection = eeSelection+' || '+mmSelection+' || '+emSelection
tauSelection = '((count('+leadElectron+')+count('+leadMuon+') >= 1) && (count('+tauRequirements+') >= 2))'

expression = lepSelection+ ' || ' +tauSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG8D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG8D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG8D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG8D1Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = [HIGG8D1SkimmingTool]
                                                                       )





#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG8D1SlimmingHelper = SlimmingHelper("HIGG8D1SlimmingHelper")

## Smart Slimming
HIGG8D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          #"MET_Reference_AntiKt4LCTopo",
                                          #"AntiKt4LCTopoJets",
                                          #"BTagging_AntiKt4LCTopo",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG8D1SlimmingHelper.ExtraVariables = ["Muons.clusterLink.allAuthors.charge.extrapolatedMuonSpectrometerTrackParticleLink.scatteringCurvatureSignificance.scatteringNeighbourSignificance",
                                        "Electrons.author.charge",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt",
                                        "GSFTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "PrimaryVertices.x.y",
                                        "BTagging_AntiKt4Track.SV1_pb.SV1_pc.SV1_pu.IP2D_pb.IP2D_pc.IP2D_pu.IP3D_pb.IP3D_pc.IP3D_pu.JetFitter_pb.JetFitter_pc.JetFitter_pu.JetFitterCombNN_pb.JetFitterCombNN_pc.JetFitterCombNN_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant",
                                        "AntiKt4PV0TrackJets.pt.eta.phi.e.m.rapidity.btaggingLink"
                                        ]

HIGG8D1SlimmingHelper.AllVariables = ["egammaClusters"]

if globalflags.DataSource()=='geant4':
    HIGG8D1SlimmingHelper.AllVariables += ["TruthParticles", "TruthEvents","TruthVertices","AntiKt4TruthJets"]
    HIGG8D1SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink",
                                             "MuonTruthParticles.truthOrigin.truthType.truthParticleLink",
                                             "InDetTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "CombinedMuonTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "TauJets.IsTruthMatched.TruthTau.truthParticleLink"]
    
    HIGG8D1SlimmingHelper.StaticContent = ["xAOD::TruthParticleContainer#TruthMuons",
                                           "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                           "xAOD::TruthParticleContainer#TruthElectrons",
                                           "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                           "xAOD::TruthParticleContainer#TruthNeutrinos",
                                           "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
                                           "xAOD::TruthParticleContainer#TruthTaus",
                                           "xAOD::TruthParticleAuxContainer#TruthTausAux."]
    
    
HIGG8D1SlimmingHelper.IncludeMuonTriggerContent=True
HIGG8D1SlimmingHelper.IncludeEGammaTriggerContent=True
HIGG8D1SlimmingHelper.IncludeTauTriggerContent=True
 
HIGG8D1SlimmingHelper.AppendContentToStream(HIGG8D1Stream)

# Add MET_RefFinalFix
#addMETOutputs(HIGG8D1Stream)

# Add TOP dedicated MET (LHElectrons)
#from DerivationFrameworkJetEtMiss.TopCustomMET import *
#addMETOutputs(HIGG8D1Stream,["Top"])
