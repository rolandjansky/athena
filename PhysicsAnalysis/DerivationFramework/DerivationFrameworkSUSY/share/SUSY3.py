#********************************************************************
# SUSY3.py once called SUSY_tau
# reductionConf flag SUSY3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY3Stream )
SUSY3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY3Stream.AcceptAlgs(["SUSY3KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY3ThinningHelper = ThinningHelper( "SUSY3ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY3 = CfgMgr.AthSequencer("SeqSUSY3")
DerivationFrameworkJob += SeqSUSY3


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY3TriggerList import triggerRegEx, triggerRegExThinning
SUSY3ThinningHelper.TriggerChains = '|'.join(triggerRegExThinning)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( 
  name          = "SUSY3TriggerSkimmingTool",
	TriggerListOR = triggerRegEx )
ToolSvc += SUSY3TriggerSkimmingTool

SUSY3ThinningHelper.AppendToStream( SUSY3Stream ) # needs to go after SUSY3ThinningHelper.TriggerChains = ... but before accesses to SUSY3ThinningHelper.ThinningSvc()


#====================================================================
# THINNING TOOLS 
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
SUSY3TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY3TPThinningTool",
                                                                 ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3TPThinningTool
thinningTools.append(SUSY3TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY3MuonTPThinningTool",
                                                                         ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3MuonTPThinningTool
thinningTools.append(SUSY3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(	name                    = "SUSY3ElectronTPThinningTool",
                                                                                ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3ElectronTPThinningTool
thinningTools.append(SUSY3ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY3PhotonTPThinningTool",
										 ThinningService	 = SUSY3ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3PhotonTPThinningTool
thinningTools.append(SUSY3PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY3TauTPThinningTool",
                                                                        ThinningService = SUSY3ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3TauTPThinningTool
thinningTools.append(SUSY3TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY3TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY3TruthThinningTool",
                                                       ThinningService              = SUSY3ThinningHelper.ThinningSvc(),
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
                                                       WriteStatus3                 = True,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)

  #from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
  #SUSY3TRUTHTauTool = DerivationFramework__TruthCollectionMakerTau(
    #name              = "SUSY3TRUTHTauTool",
    #NewCollectionName = "SUSY3TRUTHTaus"
  #)

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  ToolSvc += SUSY3TruthThinningTool
  thinningTools.append(SUSY3TruthThinningTool)
  #ToolSvc += SUSY3TRUTHTauTool
  #AugmentationTools.append(SUSY3TRUTHTauTool)


#====================================================================
# SKIMMING TOOL 
# TODO: check whether TauJets pt requirement can be raised
#====================================================================
TauRequirements = '(TauJets.pt >= 12.*GeV) && (abs(TauJets.eta) < 2.6) && (TauJets.nTracks < 4) && (TauJets.DFCommonTausLoose)' 
objectSelection = 'count('+TauRequirements+') >= 1'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY3SkimmingTool",
                                                                expression = objectSelection)

ToolSvc += SUSY3SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY3KernelSkim",
  SkimmingTools = [SUSY3SkimmingTool, SUSY3TriggerSkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY3")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY3)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY3GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY3GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
# JRC: now done in TauTruthCommon.py
#if DerivationFrameworkIsMonteCarlo:
#  AugmentationTools.append(DFCommonTauTruthMatchingWrapper)

SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY3KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY3SlimmingHelper = SlimmingHelper("SUSY3SlimmingHelper")
SUSY3SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","MET_Reference_AntiKt4EMTopo","TauJets","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY3SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_LocHadTopo"]
SUSY3SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
					"Electrons.author.charge.ptcone20.truthOrigin.truthType.bkgMotherPdgId.bkgTruthOrigin",
					"Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
          "Photons.author.Loose.Tight",
          "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
          "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix.truthOrigin.truthType",
          "InDetTrackParticles.truthOrigin.truthType",
          "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"TauJets.IsTruthMatched.TruthPtVis.truthParticleLink.truthOrigin.truthType.truthJetLink.DFCommonTausLoose",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID"]
SUSY3SlimmingHelper.IncludeMuonTriggerContent = True
SUSY3SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY3SlimmingHelper.IncludeBPhysTriggerContent = True 
#SUSY3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeJetTriggerContent = True
SUSY3SlimmingHelper.IncludeTauTriggerContent = True
SUSY3SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeBJetTriggerContent = False

# Add the MET containers to the stream
# JRC: TEMPORARILY COMMENTED
#addMETOutputs(SUSY3Stream,["SUSY"])

if globalflags.DataSource()=='geant4':

	#SUSY3Stream.AddItem( "xAOD::TruthParticleContainer#SUSY3TRUTHTaus")
	#SUSY3Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSY3TRUTHTausAux.")
	#SUSY3Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus")
	#SUSY3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux.")

  SUSY3SlimmingHelper.StaticContent += [ 
    "xAOD::TruthParticleContainer#TruthElectrons",
    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
    "xAOD::TruthParticleContainer#TruthMuons",
    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
  ]
    
SUSY3SlimmingHelper.AppendContentToStream(SUSY3Stream)
