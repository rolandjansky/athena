# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#********************************************************************
# PHYSLITE.py 
# reductionConf flag PHYSLITE in Reco_tf.py   
#********************************************************************

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('PHYSLITE')

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkMuons import MuonsCommon
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool, getPFlowfJVT
from DerivationFrameworkJetEtMiss.METCommon import scheduleMETAssocAlg 
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
import re

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_PHYSLITEStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSLITEStream )
PHYSLITEStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSLITEStream.AcceptAlgs(["PHYSLITEKernel"])

thinningTools       = []
AugmentationTools   = []
# Special sequence 
SeqPHYSLITE = CfgMgr.AthSequencer("SeqPHYSLITE")

#====================================================================
# TRUTH CONTENT
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   PHYSLITETruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "PHYSLITETruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += PHYSLITETruthCharmTool
   #from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqPHYSLITE += CfgMgr.DerivationFramework__CommonAugmentation("PHYSLITETruthCharmKernel",AugmentationTools=[PHYSLITETruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqPHYSLITE)
   # Add standard truth
   addStandardTruthContents(SeqPHYSLITE,prefix='PHYSLITE_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   PHYSLITETruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="PHYSLITETruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += PHYSLITETruthNavigationDecorator
   SeqPHYSLITE.PHYSLITE_MCTruthNavigationDecoratorKernel.AugmentationTools = [PHYSLITETruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqPHYSLITE)
   addPVCollection(SeqPHYSLITE)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # Add sumOfWeights metadata for LHE3 multiweights =======
   from DerivationFrameworkCore.LHE3WeightMetadata import *


'''
  from DerivationFrameworkMCTruth.HFHadronsCommon import *
  # Extra classifiers for the Higgs group
  import DerivationFrameworkHiggs.TruthCategories

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
'''

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
PHYSLITE_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYSLITETrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYSLITETrackParticleThinningTool",
                                                                           StreamName              = PHYSLITEStream.Name, 
                                                                           SelectionString         = PHYSLITE_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += PHYSLITETrackParticleThinningTool
thinningTools.append(PHYSLITETrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYSLITEMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYSLITEMuonTPThinningTool",
                                                                        StreamName              = PHYSLITEStream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += PHYSLITEMuonTPThinningTool
thinningTools.append(PHYSLITEMuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSLITETauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSLITETauJetsThinningTool",
                                                                     StreamName      = PHYSLITEStream.Name,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += PHYSLITETauJetsThinningTool
thinningTools.append(PHYSLITETauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
PHYSLITETauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYSLITETauTPThinningTool",
                                                                      StreamName             = PHYSLITEStream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += PHYSLITETauTPThinningTool
thinningTools.append(PHYSLITETauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
PHYSLITEDiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSLITEDiTauTPThinningTool",
                                                                          StreamName              = PHYSLITEStream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += PHYSLITEDiTauTPThinningTool
thinningTools.append(PHYSLITEDiTauTPThinningTool)

#====================================================================
# JET/MET   
#====================================================================

# TODO: UFO jets to be added in the future
largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" #, "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]

OutputJets["PHYSLITE"] = largeRJetCollections
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets", "AntiKtVR30Rmax4Rmin02PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYSLITE"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqPHYSLITE,"PHYSLITE")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqPHYSLITE,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(SeqPHYSLITE,"PHYSLITE",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqPHYSLITE,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqPHYSLITE,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqPHYSLITE,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqPHYSLITE, algname='PHYSLITEJetForwardPFlowJvtToolAlg')

#====================================================================
# EGAMMA
#====================================================================

if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ['EDTruthCentralAlg', 'EDTruthForwardAlg']:
      if hasattr(topSequence, alg):
         edtalg = getattr(topSequence, alg)
         delattr(topSequence, alg)
         SeqPHYSLITE += edtalg

#====================================================================
# Tau   
#====================================================================

# Add low-pt di-tau reconstruction
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=SeqPHYSLITE)

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSLITEDiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSLITEDiTauLowPtThinningTool",
                                                                        StreamName      = PHYSLITEStream.Name,
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSLITEDiTauLowPtThinningTool
thinningTools.append(PHYSLITEDiTauLowPtThinningTool)

# ID tracks associated with low-pt ditau
PHYSLITEDiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSLITEDiTauLowPtTPThinningTool",
                                                                               StreamName              = PHYSLITEStream.Name,
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSLITEDiTauLowPtTPThinningTool
thinningTools.append(PHYSLITEDiTauLowPtTPThinningTool)

#====================================================================
# FLAVOUR TAGGING   
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollection

FtagJetCollection('AntiKt4EMPFlowJets',SeqPHYSLITE)






#==============================================================================
# Analysis-level variables 
#==============================================================================

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
#sysLoader.sigmaRecommended = 1
sysLoader.systematicsList= ['']
SeqPHYSLITE += sysLoader


dataType = "data"
#
if DerivationFrameworkIsMonteCarlo:
  dataType = "mc"

# Create a pile-up analysis sequence
from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( dataType )
pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo_%SYS%' )
SeqPHYSLITE += pileupSequence

# Include, and then set up the electron analysis sequence:
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import  makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( dataType, 'LooseLHElectron.NonIso', shallowViewOutput = False, deepCopyOutput = True )
electronSequence.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons' )
print( electronSequence ) # For debugging
# Add the electron sequence to the job:
SeqPHYSLITE += electronSequence

# Include, and then set up the photon analysis sequence:                                       
from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import makePhotonAnalysisSequence
photonSequence = makePhotonAnalysisSequence( dataType, 'Loose.Undefined', deepCopyOutput = True, recomputeIsEM=True )
photonSequence.configure( inputName = 'Photons',
                          outputName = 'AnalysisPhotons' )
print( photonSequence ) # For debugging
SeqPHYSLITE += photonSequence

# Include, and then set up the muon analysis algorithm sequence:
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType, shallowViewOutput = False, deepCopyOutput = True, workingPoint = 'Loose.NonIso' )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons' )
print( muonSequence ) # For debugging
# Add the sequence to the job:
 
SeqPHYSLITE += muonSequence

# Include, and then set up the tau analysis algorithm sequence:                                                    
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType, 'Baseline', shallowViewOutput = False, deepCopyOutput = True )
tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets' )
print( tauSequence ) # For debugging                                                                               
# Add the sequence to the job:                                                                                     
SeqPHYSLITE += tauSequence


# Include, and then set up the jet analysis algorithm sequence:
jetContainer = 'AntiKt4EMPFlowJets'
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer, deepCopyOutput = True, shallowViewOutput = False, runFJvtUpdate = False, runFJvtSelection = False, runJvtSelection = False)
jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets' )
print( jetSequence ) # For debugging

SeqPHYSLITE += jetSequence

# Build MET from our analysis objects
from DerivationFrameworkJetEtMiss import METCommon
from METReconstruction.METAssocConfig import METAssocConfig,AssocConfig
associators = [AssocConfig('PFlowJet', 'AnalysisJets'),
               AssocConfig('Muon', 'AnalysisMuons'),
               AssocConfig('Ele', 'AnalysisElectrons'),
               AssocConfig('Gamma', 'AnalysisPhotons'),
               AssocConfig('Tau', 'AnalysisTauJets'),
               AssocConfig('Soft', '')]
PHYSLITE_cfg = METAssocConfig('AnalysisMET',
                              associators,
                              doPFlow=True)
METCommon.customMETConfigs.setdefault('AnalysisMET',{})[PHYSLITE_cfg.suffix] = PHYSLITE_cfg
scheduleMETAssocAlg(sequence=SeqPHYSLITE,configlist="AnalysisMET")

#====================================================================
# TRIGGER CONTENT
#====================================================================
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)
#
## Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))
#
## Now reduce the list...
trigger_names_notau = []
trigger_names_tau = []
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.EDMVersion == 3:
   r_tau = re.compile("HLT_.*tau.*")
   r_notau = re.compile("HLT_[1-9]*(e|mu|g).*") 
   r_veto = re.compile("HLT_.*(LRT).*")   
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      result_tau = r_tau.match(chain_name)
      result_notau = r_notau.match(chain_name)
      result_veto = r_veto.match(chain_name)
      if result_tau is not None and result_veto is None: trigger_names_tau.append(chain_name)
      if result_notau is not None and result_veto is None: trigger_names_notau.append(chain_name)
   trigger_names_notau = set(trigger_names_notau) - set(trigger_names_tau)
   trigger_names_notau = list(trigger_names_notau)
else:
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
      if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name) 
# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='PHYSLITETriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='PHYSLITETriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)

# Create trigger matching decorations
#trigmatching_helper_notau = TriggerMatchingHelper(name='PHSYLITETriggerMatchingToolNoTau',
#        OutputContainerPrefix = "Analysis",
#        trigger_list = trigger_names_notau, add_to_df_job=False,
#        InputElectrons="AnalysisElectrons",InputPhotons="AnalysisPhotons",
#        InputMuons="AnalysisMuons",InputTaus="AnalysisTauJets")
#trigmatching_helper_tau = TriggerMatchingHelper(name='PHSYLITETriggerMatchingToolTau',
#        OutputContainerPrefix = "Analysis",
#        trigger_list = trigger_names_tau, add_to_df_job=False, DRThreshold=0.2,
#        InputElectrons="AnalysisElectrons",InputPhotons="AnalysisPhotons",
#        InputMuons="AnalysisMuons",InputTaus="AnalysisTauJets")
#SeqPHYSLITE += trigmatching_helper_notau.alg
#SeqPHYSLITE += trigmatching_helper_tau.alg

#====================================================================
# MAIN KERNEL
#====================================================================
# Put our sequence into the top sequence
DerivationFrameworkJob += SeqPHYSLITE
# Add thinning tools kernel
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
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
PHYSLITESlimmingHelper.IncludeTauTriggerContent = False 
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
                                         'AnalysisElectrons':'xAOD::ElectronContainer', 'AnalysisElectronsAux':'xAOD::ElectronAuxContainer',
                                         'AnalysisMuons':'xAOD::MuonContainer', 'AnalysisMuonsAux':'xAOD::MuonAuxContainer',
                                         'AnalysisJets':'xAOD::JetContainer','AnalysisJetsAux':'xAOD::AuxContainerBase',
                                         'AnalysisPhotons':'xAOD::PhotonContainer', 'AnalysisPhotonsAux':'xAOD::PhotonAuxContainer',
                                         'AnalysisTauJets':'xAOD::TauJetContainer', 'AnalysisTauJetsAux':'xAOD::TauJetAuxContainer',
                                         'MET_Core_AnalysisMET':'xAOD::MissingETContainer', 'MET_Core_AnalysisMETAux':'xAOD::MissingETAuxContainer',
                                         'METAssoc_AnalysisMET':'xAOD::MissingETAssociationMap', 'METAssoc_AnalysisMETAux':'xAOD::MissingETAuxAssociationMap',
                                         'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                         }

# Leaving these as smart collections

PHYSLITESlimmingHelper.SmartCollections = [
  "InDetTrackParticles",
  "PrimaryVertices",
  ]

# Variables to save, per object type

PHYSLITESlimmingHelper.ExtraVariables = [ 
  "AnalysisElectrons.trackParticleLinks.pt.eta.phi.m.charge.author.DFCommonElectronsLHVeryLoose.DFCommonElectronsLHLoose.DFCommonElectronsLHLooseBL.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsLHVeryLooseIsEMValue.DFCommonElectronsLHLooseIsEMValue.DFCommonElectronsLHLooseBLIsEMValue.DFCommonElectronsLHMediumIsEMValue.DFCommonElectronsLHTightIsEMValue.DFCommonElectronsECIDS.DFCommonElectronsECIDSResult.ptvarcone20.ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.caloClusterLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType.truthPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.firstEgMotherPdgId.ambiguityType.OQ",
  "AnalysisPhotons.pt.eta.phi.m.author.OQ.DFCommonPhotonsIsEMLoose.DFCommonPhotonsIsEMTight.DFCommonPhotonsIsEMTightIsEMValue.DFCommonPhotonsIsEMTightPtIncl.DFCommonPhotonsIsEMTightPtInclIsEMValue.DFCommonPhotonsCleaning.DFCommonPhotonsCleaningNoTime.ptcone20.topoetcone20.topoetcone40.topoetcone20ptCorrection.topoetcone40ptCorrection.caloClusterLinks.vertexLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType",
  "GSFTrackParticles.chiSquared.phi.d0.theta.qOverP.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.z0.vz.charge.vertexLink",
  "egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.ETACALOFRAME.PHICALOFRAME.ETA2CALOFRAME.PHI2CALOFRAME.constituentClusterLinks",
  "AnalysisMuons.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink.allAuthors.ptcone20_TightTTVA_pt1000.ptcone20_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.numberOfPrecisionLayers.combinedTrackOutBoundsPrecisionHits.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.numberOfGoodPrecisionLayers.innerSmallHits.innerLargeHits.middleSmallHits.middleLargeHits.outerSmallHits.outerLargeHits.extendedSmallHits.extendedLargeHits.extendedSmallHoles.isSmallGoodSectors.cscUnspoiledEtaHits.EnergyLoss.energyLossType.momentumBalanceSignificance.scatteringCurvatureSignificance.scatteringNeighbourSignificance",
  "CombinedMuonTrackParticles.qOverP.d0.z0.vz.phi.theta.truthOrigin.truthType.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers.chiSquared.numberDoF",
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.truthOrigin.truthType.qOverP.theta.phi",
  "MuonSpectrometerTrackParticles.phi.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.vertexLink.theta.qOverP.truthParticleLink",
  "AnalysisTauJets.pt.eta.phi.m.tauTrackLinks.jetLink.charge.isTauFlags.ptFinalCalib.etaFinalCalib.phiFinalCalib.mFinalCalib.ptCombined.etaCombined.phiCombined.mCombined.PanTau_DecayMode.RNNJetScore.RNNJetScoreSigTrans.RNNEleScore.RNNEleScoreSigTrans.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
  "AnalysisJets.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.NumTrkPt500.SumPtTrkPt500.DetectorEta.Jvt.JVFCorr.JvtRpt.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.PartonTruthLabelID.ConeTruthLabelID.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.TrueFlavor.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.btagging.btaggingLink.GhostTrack.DFCommonJets_fJvt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PSFrac",
  "BTagging_AntiKt4EMPFlow_201903.DL1r_pu.DL1rmu_pu.DL1r_pb.DL1rmu_pb.DL1r_pc.DL1rmu_pc",
  "TruthPrimaryVertices.t.x.y.z",
  "MET_Core_AnalysisMET.name.mpx.mpy.sumet.source",
  "METAssoc_AnalysisMET.",
  "InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights",
  "EventInfo.hardScatterVertexLink"
  ]

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
    addTruth3ContentToSlimmerTool(PHYSLITESlimmingHelper)

# Add trigger matching
trigmatching_helper_notau.add_to_slimming(PHYSLITESlimmingHelper)
trigmatching_helper_tau.add_to_slimming(PHYSLITESlimmingHelper)

PHYSLITESlimmingHelper.AppendContentToStream(PHYSLITEStream)
