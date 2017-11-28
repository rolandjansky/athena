#********************************************************************
# SUSY1.py 
# reductionConf flag SUSY1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY1Stream )
SUSY1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY1Stream.AcceptAlgs(["SUSY1KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY1ThinningHelper = ThinningHelper( "SUSY1ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY1 = CfgMgr.AthSequencer("SeqSUSY1")
DerivationFrameworkJob += SeqSUSY1


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY1TriggerList import SUSY1ThinTriggers
SUSY1ThinningHelper.TriggerChains = '|'.join(SUSY1ThinTriggers)
SUSY1ThinningHelper.AppendToStream( SUSY1Stream )


#====================================================================
# THINNING TOOL 
#====================================================================\

#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# B.M.: likely not used
#SUSY1TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY1TPThinningTool",
#								 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
#								 SelectionString	 = "InDetTrackParticles.pt > 5*GeV",
#								 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY1TPThinningTool
#thinningTools.append(SUSY1TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY1MuonTPThinningTool",
									 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1MuonTPThinningTool
thinningTools.append(SUSY1MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY1ElectronTPThinningTool",
										 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1ElectronTPThinningTool
thinningTools.append(SUSY1ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY1PhotonTPThinningTool",
										 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1PhotonTPThinningTool
thinningTools.append(SUSY1PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY1TauTPThinningTool",
									ThinningService = SUSY1ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1TauTPThinningTool
thinningTools.append(SUSY1TauTPThinningTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY1PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY1PhotonThinningTool",
                                                                      ThinningService  = SUSY1ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY1PhotonThinningTool
thinningTools.append(SUSY1PhotonThinningTool)

# Calo Clusters associated with Photons
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#SUSY1PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY1PhotonCCThinningTool",
#                                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Photons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.6)
#ToolSvc += SUSY1PhotonCCThinningTool
#thinningTools.append(SUSY1PhotonCCThinningTool)

# Calo Clusters associated with Electrons
#SUSY1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY1ElectronCCThinningTool",
#                                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.4)
#ToolSvc += SUSY1ElectronCCThinningTool
#thinningTools.append(SUSY1ElectronCCThinningTool)

# Calo Clusters associated with jets
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__JetCaloClusterThinning
#SUSY1aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY1aKt4CCThinningTool",
#                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                      SGKey                   = "AntiKt4LCTopoJets",
#                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV")
#ToolSvc += SUSY1aKt4CCThinningTool
#thinningTools.append(SUSY1aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY1TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY1TruthThinningTool",
                                                       ThinningService              = SUSY1ThinningHelper.ThinningSvc(),
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
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY1TruthThinningTool
  thinningTools.append(SUSY1TruthThinningTool)


#====================================================================
# SKIMMING
#====================================================================
expression_cut_HT      = 'sum(AntiKt4EMTopoJets.DFCommonJets_Calib_pt * ((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 40*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) ) > 150*GeV'

expression_cut_elecs   = 'count((Electrons.pt > 100*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)) > 0 || count((Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)) > 1'
expression_cut_muons   = 'count((Muons.pt >= 100*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)) > 0 || count((Muons.pt >= 20*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)) > 1'
expression_cut_photons = 'count((DFCommonPhotons_et > 100*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 0 || count((DFCommonPhotons_et > 50*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 1'

expression             = '(%s) || (%s) || (%s) || (%s)' % (expression_cut_HT, expression_cut_elecs, expression_cut_muons, expression_cut_photons)


##NEW SCHEME
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY1SkimmingORTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY1SkimmingORTool",
                                                                   expression = expression)
ToolSvc += SUSY1SkimmingORTool

#add AND with Trigger skimming criteria
from DerivationFrameworkSUSY.SUSY1TriggerList import SUSY1AllTriggers
trigExpr = '('+' || '.join(SUSY1AllTriggers)+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY1trigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY1trigSkimmingTool",
                                                                  TriggerListOR = SUSY1AllTriggers)

ToolSvc += SUSY1trigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
SUSY1SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY1SkimmingTool",
                                                              FilterList = [SUSY1SkimmingORTool, SUSY1trigSkimmingTool])
ToolSvc += SUSY1SkimmingTool





from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg

#====================================================================
# ISOLATION TOOL 
#====================================================================
#Track selection
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool500.TrackSelectionTool.minPt= 500.
TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool500

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt1000_'
                                                               )
Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool500,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt500_'
                                                               )
ToolSvc += Pt1000IsoTrackDecorator
ToolSvc += Pt500IsoTrackDecorator

AugmentationTools.append(Pt1000IsoTrackDecorator)
AugmentationTools.append(Pt500IsoTrackDecorator)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY1) # default: sequence=DerivationFrameworkJob
# updateJVT_xAODColl("AntiKt4EMTopo") # TODO: for next cache?

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================

from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():

  from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
  SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel("SUSY1KernelSigAug",
                                                           AugmentationTools = DecorateSUSYProcess("SUSY1")
                                                           )

  from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelSkim",
	SkimmingTools = [SUSY1SkimmingTool],
        )


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY1"] = []

#reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets"]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically (AntiKt4PV0TrackJets flavour tagging not supported in R21)
replaceAODReducedJets(reducedJetList, SeqSUSY1, "SUSY1")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY1, "SUSY1")


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# About SmartCollections and AllVariables:
#   If you want to use CP tools on a container, you MUST include that container as a SmartCollection. 
#   You are then free to include it as AllVariables in addition, to pick up variables not needed by the CP tools but needed by your analysis.
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY1SlimmingHelper = SlimmingHelper("SUSY1SlimmingHelper")
# BTagging_AntiKt4Track changed to BTagging_AntiKt2Track, as the former is no longer supported
SUSY1SlimmingHelper.SmartCollections = ["Electrons","Photons",
                                        "AntiKt4EMTopoJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "Muons",
                                        "TauJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "BTagging_AntiKt2Track",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
SUSY1SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "MET_Track"]
SUSY1SlimmingHelper.ExtraVariables = ["Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
				      "Photons.author.Loose.Tight",
				      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.Timing",
                                      # TODO: .DFCommonJets_Jvt",
				      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
				      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
				      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
				      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "InDetTrackParticles.TrkIsoPt1000_ptcone20.TrkIsoPt1000_ptcone30.TrkIsoPt1000_ptcone40.TrkIsoPt500_ptcone20.TrkIsoPt500_ptcone30.TrkIsoPt500_ptcone40"
                                    ]	      

SUSY1SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY1SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY1SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY1SlimmingHelper.IncludeJetTriggerContent    = True
SUSY1SlimmingHelper.IncludeTauTriggerContent    = True
SUSY1SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY1SlimmingHelper.IncludeBJetTriggerContent   = False

if DerivationFrameworkIsMonteCarlo:

  # Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
  SUSY1SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  # All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
  SUSY1SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]


# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets added to 'AllVariables' here, is it intended?
#addJetOutputs(SUSY1SlimmingHelper, ["SmallR", "SUSY1"], ["AntiKt4EMTopoJets"], ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4EMPFlowJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets", "AntiKt10LCTopoJets"])


SUSY1SlimmingHelper.AppendContentToStream(SUSY1Stream)

