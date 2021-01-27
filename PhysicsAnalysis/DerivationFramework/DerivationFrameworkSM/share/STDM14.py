# *****************************************************
# STDM14.py 
# reductionConf flag STDM14 in Reco_tf.py   
# *****************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()



# =============================================
# Set up stream
# =============================================
streamName = derivationFlags.WriteDAOD_STDM14Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM14Stream )
STDM14Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM14Stream.AcceptAlgs(["STDM14Kernel"])

# =============================================
# Private sequence here
# =============================================


# =============================================
# Thinning tool
# =============================================

triggerList=['HLT_g35_medium_tau25_singlepion_tracktwo_L1TAU12',
            'HLT_g35_medium_tau25_singlepiontight_tracktwo_L1TAU12',  
            'HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000',
            'HLT_g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000',
            'HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwoEF_50mVis10000',
            'HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwoMVA_50mVis10000',
            'HLT_2g22_tight',
            'HLT_2g20_tight',
            'HLT_g35_loose_g25_loose',
            'HLT_2g25_tight_L12EM20VH',
            'HLT_2g20_tight_icalovloose_L12EM15VHI',
            'HLT_2g22_tight_L12EM15VHI',
            'HLT_g35_medium_g25_medium_L12EM20VH'
            ]

thinning_expression = "(InDetTrackParticles.pt > 10*GeV)"


from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM14ThinningHelper                              = ThinningHelper( "STDM14ThinningHelper" )

STDM14ThinningHelper.TriggerChains                = " | ".join(triggerList)
STDM14ThinningHelper.AppendToStream( STDM14Stream )

thinningTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
STDM14TPThinningTool = DerivationFramework__TrackParticleThinning(
  name                   = "STDM14TPThinningTool",
  ThinningService        = STDM14ThinningHelper.ThinningSvc(),
  SelectionString        = thinning_expression,
  InDetTrackParticlesKey = "InDetTrackParticles")

ToolSvc += STDM14TPThinningTool
thinningTools.append(STDM14TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM14JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name                   = "STDM14JetTPThinningTool",
  ThinningService        = STDM14ThinningHelper.ThinningSvc(),
  JetKey                 = "AntiKt4EMTopoJets",
  InDetTrackParticlesKey = "InDetTrackParticles")

ToolSvc += STDM14JetTPThinningTool
thinningTools.append(STDM14JetTPThinningTool)

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM14ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name                      = "STDM14ElectronTPThinningTool",
  ThinningService           = STDM14ThinningHelper.ThinningSvc(),
  SGKey                     = "Electrons",
  InDetTrackParticlesKey    = "InDetTrackParticles",
  BestMatchOnly             = False)

ToolSvc += STDM14ElectronTPThinningTool
thinningTools.append(STDM14ElectronTPThinningTool)

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM14MuonTPThinningTool   = DerivationFramework__MuonTrackParticleThinning(
  name                      = "STDM14MuonTPThinningTool",
  ThinningService           = STDM14ThinningHelper.ThinningSvc(),
  MuonKey                   = "Muons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc += STDM14MuonTPThinningTool
thinningTools.append(STDM14MuonTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      
  name                    = "STDM14PhotonTPThinningTool",
  ThinningService         = STDM14ThinningHelper.ThinningSvc(),
  SGKey                   = "Photons",
  InDetTrackParticlesKey  = "InDetTrackParticles",
  BestMatchOnly           = True)

ToolSvc += STDM14PhotonTPThinningTool
thinningTools.append(STDM14PhotonTPThinningTool)


# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM14TauTPThinningTool  = DerivationFramework__TauTrackParticleThinning(
  name                    = "STDM14TauTPThinningTool",
  ThinningService         = STDM14ThinningHelper.ThinningSvc(),
  TauKey                  = "TauJets",
  InDetTrackParticlesKey  = "InDetTrackParticles",
  ConeSize                = 0.6)

ToolSvc += STDM14TauTPThinningTool
thinningTools.append(STDM14TauTPThinningTool)

from DerivationFrameworkSM.DerivationFrameworkSMConf import DerivationFramework__ParticlePtThinningTool
STDM14ParticlePtPhotonThinningTool = DerivationFramework__ParticlePtThinningTool( 
                    name                    = "STDM14ParticlePtPhotonThinningTool",
                    ThinningService	        = STDM14ThinningHelper.ThinningSvc(),
                    ContainerName           = "Photons",
                    PtCut                   = 20000.0)

STDM14ParticlePtTauThinningTool = DerivationFramework__ParticlePtThinningTool( 
                    name                    = "STDM14ParticlePtTauThinningTool",
							      ThinningService	        = STDM14ThinningHelper.ThinningSvc(),
                    ContainerName           = "TauJets",
                    PtCut                   = 20000.0)

STDM14ParticlePtJetThinningTool = DerivationFramework__ParticlePtThinningTool( 
                    name                    = "STDM14ParticlePtJetThinningTool",
							      ThinningService	        = STDM14ThinningHelper.ThinningSvc(),
                    ContainerName           = "AntiKt4EMPFlowJets",
                    PtCut                   = 25000.0)

ToolSvc += STDM14ParticlePtPhotonThinningTool
ToolSvc += STDM14ParticlePtTauThinningTool
ToolSvc += STDM14ParticlePtJetThinningTool

thinningTools += [
  STDM14ParticlePtPhotonThinningTool,
  STDM14ParticlePtTauThinningTool,
  STDM14ParticlePtJetThinningTool
]


# =============================================
# Skimming tool
# =============================================

trigRequirement = "(" + " || ".join(triggerList) +  ")"
tauRequirement = "(count((TauJets.pt > 20.0*GeV || TauJets.ptFinalCalib > 20.0*GeV) && (abs(TauJets.eta) < 2.6)) >= 1 )"
photonRequirements ="(count((DFCommonPhotons_et >= 20*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) >= 1)"


expression =  trigRequirement + " && " + tauRequirement + " && " + photonRequirements

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM14SkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name          = "STDM14SkimmingTool",
    expression    = expression)

ToolSvc += STDM14SkimmingTool

# =============================================
# Augmentation
# =============================================

augmentationTools=[]

if globalflags.DataSource() == "geant4":
    # Tau truth matching - using the tau tools
    from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
    scheduleTauTruthTools()


# =============================================
# Standard jets
# =============================================

FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = DerivationFrameworkJob)


# =============================================
# Create derivation Kernel
# =============================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "STDM14Kernel",
  AugmentationTools         = augmentationTools,
  SkimmingTools             = [STDM14SkimmingTool],
  ThinningTools             = thinningTools
  )


# =============================================
# Add the containers to the output stream (slimming done here)
# =============================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

STDM14SlimmingHelper       = SlimmingHelper("STDM14SlimmingHelper")
STDM14SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMPFlow",                                        
                                        "AntiKt4EMPFlowJets"
]


STDM14SlimmingHelper.IncludeMuonTriggerContent    = False
STDM14SlimmingHelper.IncludeTauTriggerContent     = True
STDM14SlimmingHelper.IncludeEGammaTriggerContent  = True
STDM14SlimmingHelper.IncludeEtMissTriggerContent  = False
STDM14SlimmingHelper.IncludeJetTriggerContent     = False
STDM14SlimmingHelper.IncludeBJetTriggerContent    = False

STDM14SlimmingHelper.AppendToDictionary = {
    'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer'
    }


#Extra content lists
ExtraContentTracks               = ["InDetTrackParticles.eProbabilityHT.numberOfInnermostPixelLayerHits.numberOfPixelHits.numberOfSCTHits"]

ExtraContentElectrons=[]
ExtraElectronsTruth=["Electrons.truthOrigin.truthType.truthParticleLink"]

ExtraContentMuons=["Muons.quality.m"]
ExtraMuonsTruth=["MuonTruthParticles.truthOrigin.truthType"]

ExtraContentPhotons       = ["Photons.Loose.Medium.Tight.author.OQ.ambiguityType.Rhad.Rhad1"]
ExtraPhotonsTruth         = ["Photons.truthOrigin.truthParticleLink.truthType"]

ExtraContentTaus  = [ "TauJets.chargedPFOLinks.neutralPFOLinks.pi0PFOLinks.jetLink.clusterLinks.ptIntermediateAxis.ptDetectorAxis.dRmax.etOverPtLeadTrk.caloIso.isolFrac.centFrac.innerTrkAvgDist.ipSigLeadTrk.SumPtTrkFrac.trFlightPathSig.EMPOverTrkSysP.mEflowApprox.ptRatioEflowApprox.ptJetSeed",
                      "TauTracks",
                      "TauNeutralParticleFlowObjects",
                      "TauChargedParticleFlowObjects",
                      "TauPi0Clusters"]
ExtraTausTruth               = []

ExtraContainers  = []

ExtraContainersTruth      = [ "TruthEvents", 
                              "TruthParticles", 
                              "TruthVertices",
                              "TruthElectrons",
                              "TruthMuons",
                              "AntiKt4TruthJets",
                              "TruthTaus"
                            ]



STDM14SlimmingHelper.ExtraVariables               = ExtraContentPhotons + ExtraContentTaus + ExtraContentMuons + ExtraContentElectrons + ExtraContentTracks
STDM14SlimmingHelper.AllVariables                 = ExtraContainers 

if globalflags.DataSource() == "geant4":
  STDM14SlimmingHelper.ExtraVariables            += ExtraPhotonsTruth +ExtraTausTruth +ExtraElectronsTruth +ExtraMuonsTruth
  STDM14SlimmingHelper.AllVariables              += ExtraContainersTruth
  STDM14Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )


STDM14SlimmingHelper.AppendContentToStream(STDM14Stream)
