#********************************************************************
# SUSY9.py 
# Clone of SUSY1 with loose skimming for ~ first week of data.
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkTau.TauCommon import *
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY9Stream )
SUSY9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY9Stream.AcceptAlgs(["SUSY9KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY9ThinningHelper = ThinningHelper( "SUSY9ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY9 = CfgMgr.AthSequencer("SeqSUSY9")
DerivationFrameworkJob += SeqSUSY9


#====================================================================
# Trigger navigation thinning
#====================================================================
# To be added.
SUSY9ThinningHelper.AppendToStream( SUSY9Stream )


#====================================================================
# THINNING TOOL 
#====================================================================\

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

SUSY9TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY9TPThinningTool",
								 ThinningService	 = SUSY9ThinningHelper.ThinningSvc(),
								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
								 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9TPThinningTool
thinningTools.append(SUSY9TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY9MuonTPThinningTool",
									 ThinningService	 = SUSY9ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9MuonTPThinningTool
thinningTools.append(SUSY9MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY9ElectronTPThinningTool",
										 ThinningService	 = SUSY9ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9ElectronTPThinningTool
thinningTools.append(SUSY9ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY9PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY9PhotonTPThinningTool",
										 ThinningService	 = SUSY9ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9PhotonTPThinningTool
thinningTools.append(SUSY9PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY9TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY9TauTPThinningTool",
									ThinningService = SUSY9ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9TauTPThinningTool
thinningTools.append(SUSY9TauTPThinningTool)

# Calo Clusters associated with Photons
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#SUSY9PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY9PhotonCCThinningTool",
#                                                                                     ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Photons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.6)
#ToolSvc += SUSY9PhotonCCThinningTool
#thinningTools.append(SUSY9PhotonCCThinningTool)

# Calo Clusters associated with Electrons
#SUSY9ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY9ElectronCCThinningTool",
#                                                                                     ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.4)
#ToolSvc += SUSY9ElectronCCThinningTool
#thinningTools.append(SUSY9ElectronCCThinningTool)

# Calo Clusters associated with jets
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__JetCaloClusterThinning
#SUSY9aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY9aKt4CCThinningTool",
#                                                                     ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
#                                                                      SGKey                   = "AntiKt4LCTopoJets",
#                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV",
#                                                                      ConeSize                = 0)
#ToolSvc += SUSY9aKt4CCThinningTool
#thinningTools.append(SUSY9aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
SUSY9TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY9TruthThinningTool",
                                                     ThinningService              = SUSY9ThinningHelper.ThinningSvc(),
                                                     WritePartons                 = False,
                                                     WriteHadrons                 = False,
                                                     WriteBHadrons                = True,
                                                     WriteGeant                   = False,
                                                     GeantPhotonPtThresh          = 20000,
                                                     WriteTauHad                  = True,
                                                     PartonPtThresh               = -1.0,
                                                     WriteBSM                     = True,
                                                     WriteBosons                  = True,
                                                     WriteBosonProducts           = True,
                                                     WriteBSMProducts             = True,
                                                     WriteTopAndDecays            = True,
                                                     WriteEverything              = False,
                                                     WriteAllLeptons              = False,
                                                     WriteLeptonsNotFromHadrons   = False,
                                                     WriteStatus3                 = False,
                                                     WriteFirstN                  = -1,
                                                     PreserveAncestors            = True,
                                                     PreserveGeneratorDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
        ToolSvc += SUSY9TruthThinningTool
        thinningTools.append(SUSY9TruthThinningTool)

#====================================================================
# SKIMMING (loose, object based)
#====================================================================

# loose objects
electronRequirements = '(Electrons.pt > 10*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
muonRequirements     = '(Muons.pt > 10*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)'
tauRequirements      = '(TauJets.pt > 15.*GeV) && (abs(TauJets.eta) < 2.6) && (TauJets.nTracks < 4) && (TauJets.DFCommonTausLoose)'
photonRequirements   = '(DFCommonPhotons_et > 20.*GeV) && (abs(DFCommonPhotons_eta) < 2.6)'
jetRequirements      = '(AntiKt4EMTopoJets.JetConstitScaleMomentum_pt - AntiKt4EMTopoJets.ActiveArea4vec_pt * Kt4EMTopoEventShape.Density >= 30.*GeV) && \
                        (abs(AntiKt4EMTopoJets.JetConstitScaleMomentum_eta)<2.2)'
btagRequirements     = '(BTagging_AntiKt4EMTopo.MV2c20_discriminant > -0.5517)' # && (BTagging_AntiKt4EMTopo.pt > 20)' # FIXME
# combine objects
acceptors            = ["(count(%s) > 0)" % s for s in [electronRequirements, muonRequirements, tauRequirements, photonRequirements, jetRequirements, btagRequirements]]
# add MET requirement
#METrequirement       = '(MET_LocHadTopo.mpx > 40.*GeV)'     # segfault
#METrequirement       = '(MET_LocHadTopoAux.mpx > 40.*GeV)'  # "failed to load valid proxy loader"
#METrequirement       = '(MET_LocHadTopo.met > 40.*GeV)'     # segfault
#METrequirement       = '(MET_LocHadTopo.mpy > 40.*GeV)'     # segfault
#METrequirement = "(count((MET_RefFinal.mpx * MET_RefFinal.mpx + MET_RefFinal.mpy * MET_RefFinal.mpy) > 40*GeV * 40*GeV) > 0)" # like EXOT3, "ERROR MultipleProxyLoader: unable to find valid proxy loader for MET_RefFinal.mpx"
#METrequirement = "(count((MET_LocHadTopo.mpx * MET_LocHadTopo.mpx + MET_LocHadTopo.mpy * MET_LocHadTopo.mpy) > 40*GeV * 40*GeV) > 0)" # segfault
#METrequirement = "(count((MET_Reference_AntiKt4EMTopo.mpx * MET_Reference_AntiKt4EMTopo.mpx + MET_Reference_AntiKt4EMTopo.mpy * MET_Reference_AntiKt4EMTopo.mpy) > 40*GeV * 40*GeV) > 0)" # segfault
METrequirement       = '(%s)' % ' || '.join(["L1_XE%d" % x for x in [35, 40, 45, 50, 55, 60, 70, 80]])
acceptors.append(METrequirement) 
# combine all
looseSelection       = ' || '.join(acceptors)

print "Skimming expression:", looseSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY9SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY9SkimmingTool",
                                                                 expression = looseSelection
                                                               )

ToolSvc += SUSY9SkimmingTool


#==============================================================================
# SUSY signal augmentation
#==============================================================================
if False:
  from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__SUSYSignalTagger
  SUSYSignalTagger = DerivationFramework__SUSYSignalTagger(name = "SUSYSignalTagger")
  SUSYSignalTagger.EventInfoName = "EventInfo"
  SUSYSignalTagger.MCCollectionName = "TruthParticles"
  ToolSvc += SUSYSignalTagger
  AugmentationTools += [SUSYSignalTagger]

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqSUSY9 += CfgMgr.DerivationFramework__DerivationKernel(
                            "SUSY9KernelSkim",
                            AugmentationTools = AugmentationTools,
                            SkimmingTools = [SUSY9SkimmingTool],
                            ThinningTools = thinningTools
                          )

#=======================================
# add jet building, cf. example in JETM1
#=======================================

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(SeqSUSY9, "SUSY9")
if DerivationFrameworkIsMonteCarlo:
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY9, "SUSY9")



#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY9SlimmingHelper = SlimmingHelper("SUSY9SlimmingHelper")
SUSY9SlimmingHelper.SmartCollections = ["Electrons","Photons","MET_Reference_AntiKt4EMTopo","Muons","TauJets","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY9SlimmingHelper.AllVariables = ["AntiKt10LCTopoJets", "AntiKt10TruthJets", "TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt4PV0TrackJets", "BTagging_AntiKt4Track", "MET_Track"]
SUSY9SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
					"Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
					"Photons.author.Loose.Tight",
					"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
					"GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
					"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID"]
SUSY9SlimmingHelper.IncludeMuonTriggerContent = True
SUSY9SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY9SlimmingHelper.IncludeBPhysTriggerContent = True 
#SUSY9SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY9SlimmingHelper.IncludeJetTriggerContent = True
SUSY9SlimmingHelper.IncludeTauTriggerContent = True
SUSY9SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY9SlimmingHelper.IncludeBJetTriggerContent = True


addJetOutputs(SUSY9SlimmingHelper,["LargeR", "SUSY9"])

SUSY9SlimmingHelper.AppendContentToStream(SUSY9Stream)
