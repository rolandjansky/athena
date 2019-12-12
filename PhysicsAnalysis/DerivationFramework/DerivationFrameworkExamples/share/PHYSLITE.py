#********************************************************************
# PHYSLITE.py 
# reductionConf flag PHYSLITE in Reco_tf.py   
#********************************************************************

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('PHYSLITE')

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper

# Truth
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addPVCollection
  addStandardTruthContents()
  addPVCollection()
  from DerivationFrameworkMCTruth.HFHadronsCommon import *
  # Extra classifiers for the Higgs group
  import DerivationFrameworkHiggs.TruthCategories

### Set up stream
streamName = derivationFlags.WriteDAOD_PHYSLITEStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSLITEStream )
PHYSLITEStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSLITEStream.AcceptAlgs(["PHYSLITEKernel"])

### Thinning and augmentation tools lists
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
PHYSLITEThinningHelper = ThinningHelper( "PHYSLITEThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# Special sequence 
SeqPHYSLITE = CfgMgr.AthSequencer("SeqPHYSLITE")
DerivationFrameworkJob += SeqPHYSLITE

#====================================================================
# TRIGGER CONTENT   
#====================================================================
# See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
# Get single and multi mu, e, photon triggers
# Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
# Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
# Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs

# Merge and remove duplicates
trigger_names = list(set(trig_el+trig_mu+trig_g+trig_tau+trig_em+trig_et+trig_mt))

# Create trigger matching decorations
PHYSLITE_trigmatching_helper = TriggerMatchingHelper(matching_tool = "PHYSLITETriggerMatchingTool",
                                                 trigger_list = trigger_names)
AugmentationTools.append( PHYSLITE_trigmatching_helper.matching_tool )

#==============================================================================
# HEAVY FLAVOR DECORATION
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TopHeavyFlavorFilterAugmentation.cxx
# these are supposed to mimic the TTbarPlusBFilter, TTbarPlusBBFilter, and TTbarPlusCFilter Filters in https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/common/Filters
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool

  PHYSLITEttbarBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("PHYSLITETTbarPlusBFilterTool")
  PHYSLITEttbarBfiltertool.SelectB = True
  PHYSLITEttbarBfiltertool.BpTMinCut = 5000
  PHYSLITEttbarBfiltertool.BMultiplicityCut = 1 # >=
  ToolSvc += PHYSLITEttbarBfiltertool

  PHYSLITEttbarBBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("PHYSLITETTbarPlusBBFilterTool")
  PHYSLITEttbarBBfiltertool.SelectB = True
  PHYSLITEttbarBBfiltertool.BpTMinCut = 15000
  PHYSLITEttbarBBfiltertool.BMultiplicityCut = 2 # >=
  ToolSvc += PHYSLITEttbarBBfiltertool

  PHYSLITEttbarCfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("PHYSLITETTbarPlusCFilterTool")
  PHYSLITEttbarCfiltertool.SelectC = True
  PHYSLITEttbarCfiltertool.CpTMinCut = 15000
  PHYSLITEttbarCfiltertool.CMultiplicityCut = 1 # >=
  # these two are the default values
  # B-hadrons have precedence; if one B is found, it won't pass the CFilter
  PHYSLITEttbarCfiltertool.BpTMinCut = 5000
  PHYSLITEttbarCfiltertool.BMultiplicityCut = 1 # >=
  ToolSvc += PHYSLITEttbarCfiltertool

  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
  PHYSLITETopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "PHYSLITETopHFFilterAugmentation")
  PHYSLITETopHFFilterAugmentation.BFilterTool = PHYSLITEttbarBfiltertool
  PHYSLITETopHFFilterAugmentation.BBFilterTool = PHYSLITEttbarBBfiltertool
  PHYSLITETopHFFilterAugmentation.CFilterTool = PHYSLITEttbarCfiltertool
  ToolSvc += PHYSLITETopHFFilterAugmentation
  AugmentationTools.append(PHYSLITETopHFFilterAugmentation)
  nanolog.info("PHYSLITETopHFFilterAugmentationTool: {!s}".format(PHYSLITETopHFFilterAugmentation))


#====================================================================
# THINNING 
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
PHYSLITEThinningHelper = ThinningHelper( "PHYSLITEThinningHelper" )
PHYSLITEThinningHelper.AppendToStream( PHYSLITEStream )

# Thin all unless kept by something else
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__PHYSLITEThinningTool
PHYSLITEThinningTool = DerivationFramework__PHYSLITEThinningTool(name              = "PHYSLITEThinningTool",
                                                                 ThinningService	 = PHYSLITEThinningHelper.ThinningSvc(),
                                                                 ContainersToThin  = ["CaloCalTopoClusters",
                                                                                      "egammaClusters",
                                                                                      "GSFTrackParticles"
                                                                                      ] )
ToolSvc += PHYSLITEThinningTool
thinningTools.append(PHYSLITEThinningTool)


# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Caloclusters associated to electrons
PHYSLITEElectronClusterThinningTool = DerivationFramework__CaloClusterThinning( name                    = "PHYSLITEElectronClusterThinningTool",
                                                                                ThinningService         = PHYSLITEThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "AnalysisElectrons_NOSYS",
                                                                                CaloClCollectionSGKey   = "egammaClusters",
                                                                                TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                #SelectionString         = "Electrons.pt > 7*GeV",
                                                                                ConeSize                = 0.4)
ToolSvc += PHYSLITEElectronClusterThinningTool
thinningTools.append(PHYSLITEElectronClusterThinningTool)

# Caloclusters associated to photons
PHYSLITEPhotonClusterThinningTool = DerivationFramework__CaloClusterThinning( name                    = "PHYSLITEPhotonClusterThinningTool",
                                                                              ThinningService         = PHYSLITEThinningHelper.ThinningSvc(),
                                                                              SGKey                   = "AnalysisPhotons_NOSYS",
                                                                              CaloClCollectionSGKey   = "egammaClusters",
                                                                              TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                              #SelectionString         = ""Photons.pt > 10*GeV"",
                                                                              ConeSize                = 0.4)
ToolSvc += PHYSLITEPhotonClusterThinningTool
thinningTools.append(PHYSLITEPhotonClusterThinningTool)


# GSF track particles thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning

# GSF track associated to electrons
PHYSLITEElectronGsfTrackThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "PHYSLITEElectronGsfTrackThinningTool",
                                                                                        ThinningService        = PHYSLITEThinningHelper.ThinningSvc(),
                                                                                        SGKey                  = "AnalysisElectrons_NOSYS",
                                                                                        BestMatchOnly          = False,
                                                                                        GSFTrackParticlesKey = "GSFTrackParticles")
ToolSvc += PHYSLITEElectronGsfTrackThinningTool
thinningTools.append(PHYSLITEElectronGsfTrackThinningTool)

# GSF track associated to photons
PHYSLITEPhotonGsfTrackThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "PHYSLITEPhotonGsfTrackThinningTool",
                                                                                      ThinningService        = PHYSLITEThinningHelper.ThinningSvc(),
                                                                                      SGKey                  = "AnalysisPhotons_NOSYS",
                                                                                      BestMatchOnly          = False,
                                                                                      GSFTrackParticlesKey = "GSFTrackParticles")
ToolSvc += PHYSLITEPhotonGsfTrackThinningTool
thinningTools.append(PHYSLITEPhotonGsfTrackThinningTool)


# INNER DETECTOR TRACK THINNING
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations
# Inner detector group recommendations for indet tracks in analysis
PHYSLITE_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYSLITETrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYSLITETrackParticleThinningTool",
                                                                               ThinningService         = PHYSLITEThinningHelper.ThinningSvc(),
                                                                               SelectionString         = PHYSLITE_thinning_expression,
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               ApplyAnd                = False)
ToolSvc += PHYSLITETrackParticleThinningTool
thinningTools.append(PHYSLITETrackParticleThinningTool)


# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYSLITEMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYSLITEMuonTPThinningTool",
                                                                            ThinningService         = PHYSLITEThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "AnalysisMuons_NOSYS",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd = False)
ToolSvc += PHYSLITEMuonTPThinningTool

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSLITETauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSLITETauJetsThinningTool",
                                                                         ThinningService = PHYSLITEThinningHelper.ThinningSvc(),
                                                                         ContainerName   = "TauJets_NOSYS",
                                                                         SelectionString = "(TauJets_NOSYS.ptFinalCalib >= 13.*GeV) && (TauJets_NOSYS.nTracks<6)")
ToolSvc += PHYSLITETauJetsThinningTool

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
PHYSLITETauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYSLITETauTPThinningTool",
                                                                          ThinningService        = PHYSLITEThinningHelper.ThinningSvc(),
                                                                          TauKey                 = "TauJets_NOSYS",
                                                                          InDetTrackParticlesKey = "InDetTrackParticles",
                                                                          SelectionString        = "(TauJets_NOSYS.ptFinalCalib >= 13.*GeV) && (TauJets_NOSYS.nTracks<6)",
                                                                          ApplyAnd               = False,
                                                                          DoTauTracksThinning    = True,
                                                                          TauTracksKey           = "TauTracks")
ToolSvc += PHYSLITETauTPThinningTool

# Only keep the highest sum pT2 primary vertex
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VertexThinning
PHYSLITEVertexThinningTool = DerivationFramework__VertexThinning(name = "PHYSLITEVertexThinningTool",
                                                                 ThinningService = PHYSLITEThinningHelper.ThinningSvc(),
                                                                 VertexKey = "PrimaryVertices")
ToolSvc += PHYSLITEVertexThinningTool
thinningTools.append(PHYSLITEVertexThinningTool)

#==============================================================================
# Kernel algorithm and LHE3 imports
#==============================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY signal augmentation 
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# Jet building
#==============================================================================
OutputJets["PHYSLITE"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYSLITE"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")
   reducedJetList.append("AntiKt4TruthWZJets")
   reducedJetList.append("AntiKt4TruthJets")  
replaceAODReducedJets(reducedJetList,SeqPHYSLITE,"PHYSLITE")
addDefaultTrimmedJets(SeqPHYSLITE,"PHYSLITE",dotruth=DerivationFrameworkIsMonteCarlo)

addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqPHYSLITE,algname="QGTaggerToolPFAlg")

#====================================================================
# Flavour tagging   
#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(SeqPHYSLITE)
#addVRJetsTCC(DerivationFrameworkJob, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
#             VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
#             ghostArea = 0 , ptmin = 2000, ptminFilter = 2000,
#             variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")
# add xbb taggers
from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(SeqPHYSLITE, ToolSvc)

FlavorTagInit(JetCollections  = [ 'AntiKt4EMPFlowJets'], Sequencer = SeqPHYSLITE)


#==============================================================================
# Systematics
#==============================================================================

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
#sysLoader.sigmaRecommended = 1
sysLoader.systematicsList= ['']
SeqPHYSLITE += sysLoader

dataType = "data"

if DerivationFrameworkIsMonteCarlo:
  dataType = "mc"

# Include, and then set up the pileup analysis sequence:
from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
    makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( dataType )
pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo' )
print( pileupSequence ) # For debugging

# Add the pileup sequence to the job:
SeqPHYSLITE += pileupSequence

# Include, and then set up the electron analysis sequence:
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
    makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( dataType, 'LooseLHElectron.NonIso', shallowViewOutput = False, deepCopyOutput = True )
electronSequence.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons' )
print( electronSequence ) # For debugging

# Add the electron sequence to the job:
SeqPHYSLITE += electronSequence

# Include, and then set up the photon analysis sequence:                                       
from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
    makePhotonAnalysisSequence
photonSequence = makePhotonAnalysisSequence( dataType, 'Loose.Undefined', deepCopyOutput = True, recomputeIsEM=True )
photonSequence.configure( inputName = 'Photons',
                          outputName = 'AnalysisPhotons' )
print( photonSequence ) # For debugging

SeqPHYSLITE += photonSequence

# Include, and then set up the muon analysis algorithm sequence:
 
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType, shallowViewOutput = False, deepCopyOutput = True, workingPoint = 'Loose.NonIso' )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons_%SYS%' )
print( muonSequence ) # For debugging

# Add the sequence to the job:
 
SeqPHYSLITE += muonSequence

# Include, and then set up the tau analysis algorithm sequence:                                                    
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType, 'Tight', shallowViewOutput = False, deepCopyOutput = True )
tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets' )
print( tauSequence ) # For debugging                                                                               
# Add the sequence to the job:                                                                                     
SeqPHYSLITE += tauSequence

jetContainer = 'AntiKt4EMPFlowJets_BTagging201903'

# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer, deepCopyOutput = True, shallowViewOutput = False )
jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets' )
print( jetSequence ) # For debugging

SeqPHYSLITE += jetSequence

# Make sure the MET knows what we've done
# First we need to rebuild charged pflow objects
from eflowRec.ScheduleCHSPFlowMods import scheduleCHSPFlowMods
scheduleCHSPFlowMods(SeqPHYSLITE)
# Now build MET from our analysis objects
from DerivationFrameworkJetEtMiss import METCommon
from METReconstruction.METAssocConfig import METAssocConfig,AssocConfig
associators = [AssocConfig('PFlowJet', 'AnalysisJets_NOSYS'),
               AssocConfig('Muon', 'AnalysisMuons_NOSYS'),
               AssocConfig('Ele', 'AnalysisElectrons_NOSYS'),
               AssocConfig('Gamma', 'AnalysisPhotons_NOSYS'),
               AssocConfig('Tau', 'AnalysisTauJets_NOSYS'),
               AssocConfig('Soft', '')]
PHYSLITE_cfg = METAssocConfig('AnalysisMET',
                              associators,
                              doPFlow=True)
METCommon.customMETConfigs.setdefault('AnalysisMET',{})[PHYSLITE_cfg.suffix] = PHYSLITE_cfg
scheduleMETAssocAlg(sequence=SeqPHYSLITE,configlist="AnalysisMET")

#====================================================================
# MAIN KERNEL
#====================================================================
SeqPHYSLITE += CfgMgr.DerivationFramework__DerivationKernel(
   "PHYSLITEKernel",
   AugmentationTools = AugmentationTools,
   ThinningTools = thinningTools,
   )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSLITESlimmingHelper = SlimmingHelper("PHYSLITESlimmingHelper")

PHYSLITESlimmingHelper.IncludeTriggerNavigation = False
PHYSLITESlimmingHelper.IncludeJetTriggerContent = False
PHYSLITESlimmingHelper.IncludeMuonTriggerContent = False
PHYSLITESlimmingHelper.IncludeEGammaTriggerContent = False
PHYSLITESlimmingHelper.IncludeJetTauEtMissTriggerContent = False
PHYSLITESlimmingHelper.IncludeTauTriggerContent = False #True
PHYSLITESlimmingHelper.IncludeEtMissTriggerContent = False
PHYSLITESlimmingHelper.IncludeBJetTriggerContent = False
PHYSLITESlimmingHelper.IncludeBPhysTriggerContent = False
PHYSLITESlimmingHelper.IncludeMinBiasTriggerContent = False

# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
PHYSLITESlimmingHelper.AppendToDictionary = {
                                         'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                         'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                         'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                         'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                         'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                         'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                         'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                         'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
                                         'AnalysisElectrons_NOSYS':'xAOD::ElectronContainer', 'AnalysisElectrons_NOSYSAux':'xAOD::ElectronAuxContainer',
                                         'AnalysisMuons_NOSYS':'xAOD::MuonContainer', 'AnalysisMuons_NOSYSAux':'xAOD::MuonAuxContainer',
                                         'AnalysisJets_NOSYS':'xAOD::JetContainer','AnalysisJets_NOSYSAux':'xAOD::AuxContainerBase',
                                         'AnalysisPhotons_NOSYS':'xAOD::PhotonContainer', 'AnalysisPhotons_NOSYSAux':'xAOD::PhotonAuxContainer',
                                         'AnalysisTauJets_NOSYS':'xAOD::TauJetContainer', 'AnalysisTauJets_NOSYSAux':'xAOD::TauJetAuxContainer',
                                         'MET_Core_AnalysisMET':'xAOD::MissingETContainer', 'MET_Core_AnalysisMETAux':'xAOD::MissingETAuxContainer',
                                         'METAssoc_AnalysisMET':'xAOD::MissingETAssociationMap', 'METAssoc_AnalysisMETAux':'xAOD::MissingETAuxAssociationMap',
                                         }

# Leaving these as smart collections

PHYSLITESlimmingHelper.SmartCollections = [
  "InDetTrackParticles",
  "PrimaryVertices",
  ]

# Variables to save, per object type

PHYSLITESlimmingHelper.ExtraVariables = [ 
  "AnalysisElectrons_NOSYS.trackParticleLinks.pt.eta.phi.m.charge.author.DFCommonElectronsLHVeryLoose.DFCommonElectronsLHLoose.DFCommonElectronsLHLooseBL.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsLHVeryLooseIsEMValue.DFCommonElectronsLHLooseIsEMValue.DFCommonElectronsLHLooseBLIsEMValue.DFCommonElectronsLHMediumIsEMValue.DFCommonElectronsLHTightIsEMValue.DFCommonElectronsECIDS.DFCommonElectronsECIDSResult.ptvarcone20.ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.caloClusterLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType.truthPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.firstEgMotherPdgId.ambiguityType.OQ",
  "AnalysisPhotons_NOSYS.pt.eta.phi.m.author.OQ.DFCommonPhotonsIsEMLoose.DFCommonPhotonsIsEMTight.DFCommonPhotonsIsEMTightIsEMValue.DFCommonPhotonsIsEMTightPtIncl.DFCommonPhotonsIsEMTightPtInclIsEMValue.DFCommonPhotonsCleaning.DFCommonPhotonsCleaningNoTime.ptcone20.topoetcone20.topoetcone40.topoetcone20ptCorrection.topoetcone40ptCorrection.caloClusterLinks.vertexLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType",
  "GSFTrackParticles.chiSquared.phi.d0.theta.qOverP.definingParametersCovMatrix.z0.vz.charge.vertexLink",
  "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
  "egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME.ETA2CALOFRAME.PHI2CALOFRAME.constituentClusterLinks",
  "AnalysisMuons_NOSYS.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink.allAuthors.ptcone20_TightTTVA_pt1000.ptcone20_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.numberOfPrecisionLayers.combinedTrackOutBoundsPrecisionHits.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.numberOfGoodPrecisionLayers.innerSmallHits.innerLargeHits.middleSmallHits.middleLargeHits.outerSmallHits.outerLargeHits.extendedSmallHits.extendedLargeHits.extendedSmallHoles.isSmallGoodSectors.cscUnspoiledEtaHits.EnergyLoss.energyLossType.momentumBalanceSignificance.scatteringCurvatureSignificance.scatteringNeighbourSignificance",
  "CombinedMuonTrackParticles.qOverP.d0.z0.vz.phi.theta.truthOrigin.truthType.definingParametersCovMatrix.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers.chiSquared.numberDoF",
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType.qOverP.theta.phi",
  "MuonSpectrometerTrackParticles.phi.d0.z0.vz.definingParametersCovMatrix.vertexLink.theta.qOverP.truthParticleLink",
  "AnalysisTauJets_NOSYS.pt.eta.phi.m.tauTrackLinks.jetLink.charge.isTauFlags.BDTJetScore.BDTEleScore.ptFinalCalib.etaFinalCalib.phiFinalCalib.mFinalCalib.ele_match_lhscore.ele_olr_pass.electronLink.IsVeryLoose.EleMatchLikelihoodScore.pt_combined.eta_combined.phi_combined.m_combined.BDTJetScoreSigTrans.BDTEleScoreSigTrans.PanTau_DecayMode.RNNJetScore.RNNJetScoreSigTrans.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
  "AnalysisJets_NOSYS.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.NumTrkPt500.SumPtTrkPt500.DetectorEta.Jvt.JVFCorr.JvtRpt.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.PartonTruthLabelID.ConeTruthLabelID.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.TrueFlavor.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.btagging.btaggingLink.GhostTrack",
  "BTagging_AntiKt4EMPFlow_201903.DL1r_pu.DL1rmu_pu.DL1r_pb.DL1rmu_pb.DL1r_pc.DL1rmu_pc",
  "TruthPrimaryVertices.t.x.y.z",
  "MET_Core_AnalysisMET.name.mpx.mpy.sumet.source",
  "METAssoc_AnalysisMET.",
  ]

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
    addTruth3ContentToSlimmerTool(PHYSLITESlimmingHelper)

# Extra trigger collections
PHYSLITE_trigmatching_helper.add_to_slimming(PHYSLITESlimmingHelper)

PHYSLITESlimmingHelper.AppendContentToStream(PHYSLITEStream)

