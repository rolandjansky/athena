#====================================================================
# JETM6.py
# reductionConf flag JETM6 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkFlavourTag.HbbCommon import *

from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkInDet.InDetCommon import *

#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
electronTriggers = TriggerLists.single_el_Trig()
muonTriggers = TriggerLists.single_mu_Trig()
photonTriggers = TriggerLists.single_photon_Trig()
jetTriggers = TriggerLists.jetTrig()

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 400.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10UFOCSSKJets.pt > 400.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >= 1)'
if DerivationFrameworkHasTruth:
  jetSelection = '(count( AntiKt10LCTopoJets.pt > 180.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10UFOCSSKJets.pt > 180.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >= 1)'

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHLoose)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1)'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1 )'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
gammatrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(photonTriggers)
gammaofflinesel = '(count(Photons.pt > 150*GeV) >= 1 && (count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1 ))'
photonSelection = ' ( (' + gammatrigsel + ') && (' + gammaofflinesel + ') ) '

lepSelection = '( ' + electronSelection + ' || ' + muonSelection + ' || ' + photonSelection + ' )'


expression = jetSelection + ' || '+ lepSelection


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM6TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM6TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers+electronTriggers+muonTriggers+photonTriggers )
                                                                    #TriggerListOR  = triggers )
ToolSvc += JETM6TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM6OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM6OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += JETM6OfflineSkimmingTool

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM6Stream )
JETM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM6Stream.AcceptAlgs(["JETM6MainKernel"])

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

jetm6Seq = CfgMgr.AthSequencer("JETM6Sequence")
DerivationFrameworkJob += jetm6Seq

#====================================================================
# Trigger matching decorations
#====================================================================
photonTriggers_matching = ['HLT_g60_loose', 'HLT_g140_loose', 'HLT_g160_loose']

from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM6",
                                   ElectronTriggers=electronTriggers,
                                   MuonTriggers=muonTriggers,
                                   PhotonTriggers=photonTriggers_matching)

#====================================================================
# TRIGGER THINNING TOOL
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM6ThinningHelper = ThinningHelper( "JETM6ThinningHelper" )
JETM6ThinningHelper.TriggerChains = ''

JETM6ThinningHelper.TriggerChains += "|".join(electronTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(muonTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(photonTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(jetTriggers)

JETM6ThinningHelper.AppendToStream( JETM6Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

#########################################
# Tracks associated with akt2 jets
#########################################
# It is necessary to apply the akt2-based track thinning before other track
# thinning tools due to the faulty logic of the thinning tools
from ThinningUtils.ThinningUtilsConf import DeltaRThinningTool
# Applying only DeltaR thinning and not EleLink thinning is fine as long as ConeSize
# is sufficiently large compared to the jet size.  If it is reduced to something close
# to the size of the jet, there is no guarantee that all ghost-associated tracks will
# be picked up

JETM6BaselineTrack = "(InDetTrackParticles.JETM6DFLoose) && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm) && (InDetTrackParticles.d0 < 2.0*mm)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM6TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name            = "JETM6TrackParticleThinningTool",
                                                                            ThinningService = JETM6ThinningHelper.ThinningSvc(),
                                                                            SelectionString = JETM6BaselineTrack,
                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
                                                                            ApplyAnd        = True)

ToolSvc += JETM6TrackParticleThinningTool
thinningTools.append(JETM6TrackParticleThinningTool)

JETM6ak2DeltaRTrackThinningTool = DeltaRThinningTool(name            = "JETM6ak2DeltaRTrackThinningTool",
                                                     ThinningService = JETM6ThinningHelper.ThinningSvc(),
                                                     SGKey           = "InDetTrackParticles",
                                                     ConeSize        = 0.33,
                                                     ApplyAnd        = True)

ToolSvc += JETM6ak2DeltaRTrackThinningTool

from ThinningUtils.ThinningUtilsConf import ThinAssociatedObjectsTool
JETM6ak2TrackThinningTool = ThinAssociatedObjectsTool(name               = "JETM6ak2TrackThinningTool",
                                                      ThinningService    = JETM6ThinningHelper.ThinningSvc(),
                                                      SGKey              = "AntiKt2LCTopoJets",
                                                      ChildThinningTools = [JETM6ak2DeltaRTrackThinningTool])

ToolSvc += JETM6ak2TrackThinningTool
thinningTools.append(JETM6ak2TrackThinningTool)

#########################################
# Tracks associated with other jets
#########################################

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4JetTPThinningTool",
                                                                            ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = False)
ToolSvc += JETM6Akt4JetTPThinningTool
thinningTools.append(JETM6Akt4JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4PFlowJetTPThinningTool",
                                                                                 ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                                 JetKey                  = "AntiKt4EMPFlowJets",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 ApplyAnd                = False)
ToolSvc += JETM6Akt4PFlowJetTPThinningTool
thinningTools.append(JETM6Akt4PFlowJetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetTPThinningTool",
                                                                             ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                             JetKey                  = "AntiKt10LCTopoJets",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = False)
ToolSvc += JETM6Akt10JetTPThinningTool
thinningTools.append(JETM6Akt10JetTPThinningTool)

JETM6Akt10JetCSSKUFOThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetCSSKUFOThinningTool",
                                                                                  ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                                  JetKey                  = "AntiKt10UFOCSSKJets",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                  ApplyAnd                = False)
ToolSvc += JETM6Akt10JetCSSKUFOThinningTool
thinningTools.append(JETM6Akt10JetCSSKUFOThinningTool)


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

# TrackParticles associated with photons
JETM6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6PhotonTPThinningTool",
                                                                             ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6PhotonTPThinningTool
thinningTools.append(JETM6PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM6TauTPThinningTool",
                                                                        ThinningService = JETM6ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6TauTPThinningTool
thinningTools.append(JETM6TauTPThinningTool)


# Tracks and CaloClusters associated with UFOs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UFOTrackParticleThinning
JETM6EMUFOCSSKTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                   = "JETM6UFOCSSKTPThinningTool",
                                                                             ThinningService        = "JETM6ThinningSvc",
                                                                             JetKey                 = "AntiKt10UFOCSSKJets",
                                                                             UFOKey                 = "UFOCSSK",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                                             PFOCollectionSGKey     = "JetETMiss",
                                                                             AdditionalPFOKey       = ["CSSK"])

ToolSvc += JETM6EMUFOCSSKTPThinningTool
thinningTools.append(JETM6EMUFOCSSKTPThinningTool)

#====================================================================
# Thin TruthParticles for truth jet constituents
#====================================================================

if DerivationFrameworkHasTruth:
  from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__ViewContainerThinning
  JETM6TruthJetInputThin = DerivationFramework__ViewContainerThinning( name = "JETM6ViewContThinning",
                                                                       ThinningService        = "JETM6ThinningSvc",
                                                                       SourceContainer = "TruthParticles",
                                                                       ViewContainer = "JetInputTruthParticles",
                                                                       ParticleType = 201, # truthParticles
                                                                       ApplyAnd = False)

  ToolSvc += JETM6TruthJetInputThin
  thinningTools.append(JETM6TruthJetInputThin)


#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
augmentationTools.append(TrigMatchAug)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
JETM6TrackSelectionTool = DerivationFramework__InDetTrackSelectionToolWrapper(name = "JETM6TrackSelectionTool",
                                                                              ContainerName = "InDetTrackParticles",
                                                                              DecorationName = "JETM6DFLoose" )

JETM6TrackSelectionTool.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += JETM6TrackSelectionTool
augmentationTools.append(JETM6TrackSelectionTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel(name = "JETM6TrigSkimKernel",
                                                         AugmentationTools = [] ,
                                                         SkimmingTools = [JETM6TrigSkimmingTool],
                                                         ThinningTools = [])


#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt2TruthJets",
                  "AntiKt2LCTopoJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets"]

replaceAODReducedJets(reducedJetList,jetm6Seq,"JETM6")

#build ungroomed large-R UFO jets
addDefaultUFOJets(jetm6Seq,"JETM6",doCHS=True)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM6MainKernel",
                                                          AugmentationTools = augmentationTools,
                                                          SkimmingTools = [JETM6OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# GROOMED LARGE-R JETS
#====================================================================

OutputJets["JETM6"] = []

addDefaultTrimmedJets(jetm6Seq,"JETM6")

if DerivationFrameworkHasTruth:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
  addRecursiveSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, N=-1,  mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
  addBottomUpSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)

addTrimmedJets("AntiKt", 1.0, "UFOCHS", rclus=0.2, ptfrac=0.05, algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False, mods="tcc_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCSSK", rclus=0.2, ptfrac=0.05, algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False, mods="tcc_groomed")
addSoftDropJets("AntiKt", 1.0, "UFOCSSK", beta=1.0, zcut=0.1, algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False, mods="tcc_groomed")
addRecursiveSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, N=-1,  mods="tcc_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
addBottomUpSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, mods="tcc_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)

#====================================================================
# BTAGGING INFO FOR PFLOW JETS
#====================================================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'],Sequencer = jetm6Seq)

#====================================================================
# VR track-jets (b-tagging)
#====================================================================

largeRJetAlgs = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCHSTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10",
    "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5",
    "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5Ninf",
    ]

largeRJetCollections = []
for alg in largeRJetAlgs:
  largeRJetCollections.append(alg+"Jets")

if DerivationFrameworkHasTruth:
  for alg in largeRJetAlgs:
    addJetTruthLabel(jetalg=alg,sequence=jetm6Seq,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")
    addJetTruthLabel(jetalg=alg,sequence=jetm6Seq,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Precision")

addVRJets(jetm6Seq, largeRColls = largeRJetCollections)
addVRJets(jetm6Seq, largeRColls = largeRJetCollections, training='201903')

#====================================================================
# add xbb taggers
#====================================================================

from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(jetm6Seq, ToolSvc)

#====================================================================
# TRUTH3
#====================================================================

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addTopQuarkAndDownstreamParticles,addHFAndDownstreamParticles,addTruthCollectionNavigationDecorations
  addStandardTruthContents()
  addTopQuarkAndDownstreamParticles()
  addHFAndDownstreamParticles(addB=True, addC=False, generations=0)
  addTruthCollectionNavigationDecorations(TruthCollections=["TruthTopQuarkWithDecayParticles","TruthBosonsWithDecayParticles"],prefix='Top')
  import DerivationFrameworkCore.WeightMetadata
  import DerivationFrameworkCore.LHE3WeightMetadata

# Add the BCID info
addDistanceInTrain(jetm6Seq)

# Alternative rho definition
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
addCHSPFlowObjects()
from DerivationFrameworkJetEtMiss.JetCommon import defineEDAlg
jetm6Seq += defineEDAlg(R=0.4, inputtype="EMPFlowPUSB")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM6SlimmingHelper = SlimmingHelper("JETM6SlimmingHelper")
JETM6SlimmingHelper.AppendToDictionary = {
    "Kt4EMPFlowPUSBEventShape": "xAOD::EventShape"    ,
    "Kt4EMPFlowPUSBEventShapeAux": "xAOD::AuxInfoBase"    ,
}

JETM6SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt2TruthJets",
                                        "AntiKt2LCTopoJets",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt4TruthJets","AntiKt4TruthWZJets","AntiKt4TruthDressedWZJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        ]
JETM6SlimmingHelper.AllVariables = [
  "TruthEvents",
  "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape","Kt4EMPFlowPUSBEventShape",
  "UFOCSSK",
  "TruthParticles",
  "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
  "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets"
  ]

JETM6SlimmingHelper.AppendToDictionary["UFOCSSK"] = "xAOD::TrackCaloClusterContainer"
JETM6SlimmingHelper.AppendToDictionary["UFOCSSKAux"] = "xAOD::TrackCaloClusterAuxContainer"

JETM6SlimmingHelper.ExtraVariables  = ['CaloCalTopoClusters.calE.calEta.calM.calPhi.CENTER_MAG']
#JETM6SlimmingHelper.ExtraVariables += ['UFOCSSK.pt.eta.phi.taste']
JETM6SlimmingHelper.ExtraVariables += ['Electrons.'+NewTrigVars["Electrons"],'Muons.'+NewTrigVars["Muons"],'Photons.'+NewTrigVars["Photons"]]
JETM6SlimmingHelper.ExtraVariables += [
    'AntiKt10TruthJets.Split12.Split23.SizeParameter',
    'AntiKt10UFOCSSKJets.SizeParameter',
    'HLT_xAOD__JetContainer_a4tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a4tcemsubjesISFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a10tclcwsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    ]

for truthc in [
  "TruthTopQuark",
  "TruthBosons",
  "TruthHF"
  ]:
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthBottom"
    ]:
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

#====================================================================
# ORIGIN CORRECTED CLUSTERS
#====================================================================

addOriginCorrectedClusters(JETM6SlimmingHelper,writeLC=True,writeEM=True)

#====================================================================
# TRIGGER CONTENT
#====================================================================
 
JETM6SlimmingHelper.IncludeJetTriggerContent = True
JETM6SlimmingHelper.IncludeMuonTriggerContent = True
JETM6SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM6SlimmingHelper,
              ["JETM6","AntiKt10LCTopoJets","AntiKt10TruthJets","AntiKt10UFOCSSKJets","AntiKt10UFOCHSJets"],
              ["AntiKt10LCTopoJets","AntiKt10TruthJets","AntiKt10UFOCSSKJets","AntiKt10UFOCHSJets"], #smart slimming
              ["AntiKt10EMPFlowCSSKJets"] #veto jets
              )

# Add the MET containers to the stream
addMETOutputs(JETM6SlimmingHelper,["Diagnostic","AntiKt4EMPFlow","Track"])

JETM6SlimmingHelper.AppendContentToStream(JETM6Stream)
#JETM6Stream.RemoveItem("xAOD::TrigNavigation#*")
#JETM6Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")



def removeVars(coll, vars):
    """Terrible hack to force removing a limited set of variables.
    Intended to remove definingParametersCovMatrix if possible.. """
    items = JETM6Stream.GetItems()
    origS = None
    for i in items:
        if coll in i and '.' in i:
            origS = i
            break
    if origS is None:
        return
    existV = origS.split('.')
    cleanedV = []
    for v in existV:
        if v in vars:
            continue
        cleanedV.append(v)
    newS = '.'.join(cleanedV)
    JETM6Stream.RemoveItem( origS )
    JETM6Stream.AddItem( newS )

#removeVars('InDetTrackParticles', ['definingParametersCovMatrix',])

