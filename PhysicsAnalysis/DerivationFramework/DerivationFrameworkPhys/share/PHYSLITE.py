# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#********************************************************************
# PHYSLITE.py 
# reductionConf flag PHYSLITE in Reco_tf.py   
#********************************************************************

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('PHYSLITE')

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
# Is input AOD or DAOD?
inputIsDAODPHYS = False
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary['stream_names'][0] == 'StreamDAOD_PHYS': inputIsDAODPHYS = True
if (inputIsDAODPHYS==False):
  # Only schedule these is input is AOD 
  from DerivationFrameworkPhys import PhysCommon
  from DerivationFrameworkCore import LHE3WeightMetadata
from DerivationFrameworkPhys import PhysCommonTrigger
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkJetEtMiss import METCommon
from DerivationFrameworkJetEtMiss.METCommon import scheduleMETAssocAlg
import re

#====================================================================
# SET UP STREAM   
#====================================================================
if(inputIsDAODPHYS):
  streamName = derivationFlags.WriteD2AOD_PHYSLITEStream.StreamName
  fileName   = buildFileName( derivationFlags.WriteD2AOD_PHYSLITEStream )
else:
  streamName = derivationFlags.WriteDAOD_PHYSLITEStream.StreamName
  fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSLITEStream )
  
PHYSLITEStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSLITEStream.AcceptAlgs(["PHYSLITEKernel"])

thinningTools       = []

# Special sequence 
SeqPHYSLITE = CfgMgr.AthSequencer("SeqPHYSLITE")

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
# THINNING
#====================================================================
# ID tracks: See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
if (inputIsDAODPHYS == False):
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
  
  '''
  # TauJets thinning
  # Disabled for 1st production in 2021
  tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
  PHYSLITETauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSLITETauJetsThinningTool",
                                                                       StreamName      = PHYSLITEStream.Name,
                                                                       ContainerName   = "TauJets",
                                                                       SelectionString = tau_thinning_expression)
  ToolSvc += PHYSLITETauJetsThinningTool
  thinningTools.append(PHYSLITETauJetsThinningTool)
  '''

  # Only keep tau tracks (and associated ID tracks) classified as charged tracks
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
  PHYSLITETauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYSLITETauTPThinningTool",
                                                                        StreamName             = PHYSLITEStream.Name,
                                                                        TauKey                 = "TauJets",
                                                                        InDetTrackParticlesKey = "InDetTrackParticles",
#                                                                        SelectionString        = tau_thinning_expression,
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

#==============================================================================
# Analysis-level variables 
#==============================================================================

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SystematicsSvc( 'SystematicsSvc' )
#sysLoader.sigmaRecommended = 1
sysLoader.systematicsList= ['']
ServiceMgr += sysLoader


dataType = "data"
#
if DerivationFrameworkIsMonteCarlo:
  dataType = "mc"

# Create a pile-up analysis sequence
from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( dataType )
pileupSequence.configure( inputName = {}, outputName = {} )
print( pileupSequence ) # For debugging
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
photonSequence = makePhotonAnalysisSequence( dataType, 'Loose.Undefined', deepCopyOutput = True, shallowViewOutput = False, recomputeIsEM=False )
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

# Create trigger matching decorations
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
PHYSLITEtrigmatching_helper_notau = TriggerMatchingHelper(name='PHYSLITETriggerMatchingToolNoTau',
        OutputContainerPrefix = "AnalysisTrigMatch_",
        trigger_list = PhysCommonTrigger.trigger_names_notau, add_to_df_job=False,
        InputElectrons="AnalysisElectrons",InputPhotons="AnalysisPhotons",
        InputMuons="AnalysisMuons",InputTaus="AnalysisTauJets")
PHYSLITEtrigmatching_helper_tau = TriggerMatchingHelper(name='PHYSLITETriggerMatchingToolTau',
        OutputContainerPrefix = "AnalysisTrigMatch_",
        trigger_list = PhysCommonTrigger.trigger_names_tau, add_to_df_job=False, DRThreshold=0.2,
        InputElectrons="AnalysisElectrons",InputPhotons="AnalysisPhotons",
        InputMuons="AnalysisMuons",InputTaus="AnalysisTauJets")
SeqPHYSLITE += PHYSLITEtrigmatching_helper_notau.alg
SeqPHYSLITE += PHYSLITEtrigmatching_helper_tau.alg

#====================================================================
# MAIN KERNEL
#====================================================================
# Put our sequence into the top sequence
DerivationFrameworkJob += SeqPHYSLITE
# Add thinning tools kernel
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqPHYSLITE += CfgMgr.DerivationFramework__DerivationKernel(
   "PHYSLITEKernel",
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
  "EventInfo",
  "InDetTrackParticles",
  "PrimaryVertices",
  ]

# Variables to save, per object type

PHYSLITESlimmingHelper.ExtraVariables = [ 
  "AnalysisElectrons.trackParticleLinks.pt.eta.phi.m.charge.author.DFCommonElectronsLHVeryLoose.DFCommonElectronsLHLoose.DFCommonElectronsLHLooseBL.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsLHVeryLooseIsEMValue.DFCommonElectronsLHLooseIsEMValue.DFCommonElectronsLHLooseBLIsEMValue.DFCommonElectronsLHMediumIsEMValue.DFCommonElectronsLHTightIsEMValue.DFCommonElectronsECIDS.DFCommonElectronsECIDSResult.ptvarcone20.ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.caloClusterLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType.truthPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.firstEgMotherPdgId.ambiguityType.OQ",
  "AnalysisPhotons.pt.eta.phi.m.author.OQ.DFCommonPhotonsIsEMLoose.DFCommonPhotonsIsEMTight.DFCommonPhotonsIsEMTightIsEMValue.DFCommonPhotonsCleaning.DFCommonPhotonsCleaningNoTime.ptcone20.topoetcone20.topoetcone40.topoetcone20ptCorrection.topoetcone40ptCorrection.caloClusterLinks.vertexLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType",
  "GSFTrackParticles.chiSquared.phi.d0.theta.qOverP.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.z0.vz.charge.vertexLink",
  "egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.ETACALOFRAME.PHICALOFRAME.ETA2CALOFRAME.PHI2CALOFRAME.constituentClusterLinks",
  "AnalysisMuons.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink.allAuthors.ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500.numberOfPrecisionLayers.combinedTrackOutBoundsPrecisionHits.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.numberOfGoodPrecisionLayers.innerSmallHits.innerLargeHits.middleSmallHits.middleLargeHits.outerSmallHits.outerLargeHits.extendedSmallHits.extendedLargeHits.extendedSmallHoles.isSmallGoodSectors.cscUnspoiledEtaHits.EnergyLoss.energyLossType.momentumBalanceSignificance.scatteringCurvatureSignificance.scatteringNeighbourSignificance",
  "CombinedMuonTrackParticles.qOverP.d0.z0.vz.phi.theta.truthOrigin.truthType.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers.chiSquared.numberDoF",
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.truthOrigin.truthType.qOverP.theta.phi",
  "MuonSpectrometerTrackParticles.phi.d0.z0.vz.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.vertexLink.theta.qOverP.truthParticleLink",
  "AnalysisTauJets.pt.eta.phi.m.ptFinalCalib.etaFinalCalib.ptTauEnergyScale.etaTauEnergyScale.charge.isTauFlags.PanTau_DecayMode.NNDecayMode.RNNJetScore.RNNJetScoreSigTrans.RNNEleScore.RNNEleScoreSigTrans.tauTrackLinks.vertexLink.truthParticleLink.truthJetLink.IsTruthMatched.truthOrigin.truthType",
  "AnalysisJets.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.NumTrkPt500.SumPtTrkPt500.DetectorEta.Jvt.JVFCorr.JvtRpt.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.PartonTruthLabelID.ConeTruthLabelID.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.TrueFlavor.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.btagging.btaggingLink.GhostTrack.DFCommonJets_fJvt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PSFrac.JetAccessorMap.EMFrac.Width.ActiveArea4vec_pt.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.EnergyPerSampling.SumPtChargedPFOPt500",
  "BTagging_AntiKt4EMPFlow.DL1r_pu.DL1rmu_pu.DL1r_pb.DL1rmu_pb.DL1r_pc.DL1rmu_pc",
  "TruthPrimaryVertices.t.x.y.z",
  "MET_Core_AnalysisMET.name.mpx.mpy.sumet.source",
  "METAssoc_AnalysisMET.",
  "InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights.numberOfTRTHits.numberOfTRTOutliers",
  "EventInfo.hardScatterVertexLink.RandomRunNumber",
  "Kt4EMPFlowEventShape.Density",
  "TauTracks.pt.eta.phi.flagSet.trackLinks",
  ]

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
    addTruth3ContentToSlimmerTool(PHYSLITESlimmingHelper)

# Add trigger matching
PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(PHYSLITESlimmingHelper)
PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(PHYSLITESlimmingHelper)
PHYSLITEtrigmatching_helper_notau.add_to_slimming(PHYSLITESlimmingHelper)
PHYSLITEtrigmatching_helper_tau.add_to_slimming(PHYSLITESlimmingHelper)

PHYSLITESlimmingHelper.AppendContentToStream(PHYSLITEStream)
