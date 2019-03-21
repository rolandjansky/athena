#********************************************************************
# NANO.py 
# reductionConf flag NANO in Reco_tf.py   
#********************************************************************

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('NANO')
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  from DerivationFrameworkMCTruth.HFHadronsCommon import *

from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_NANOStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_NANOStream )
NANOStream = MSMgr.NewPoolRootStream( streamName, fileName )
NANOStream.AcceptAlgs(["NANOKernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
NANOThinningHelper = ThinningHelper( "NANOThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqNANO = CfgMgr.AthSequencer("SeqNANO")
DerivationFrameworkJob += SeqNANO


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY7TriggerList import * 
NANOThinningHelper.TriggerChains = '|'.join(SUSY7ThinTriggers) #SingleLepton + DiLepton + Photon + Tau
NANOThinningHelper.AppendToStream( NANOStream )


#==============================================================================
# HEAVY FLAVOR DECORATION
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TopHeavyFlavorFilterAugmentation.cxx
# these are supposed to mimic the TTbarPlusBFilter, TTbarPlusBBFilter, and TTbarPlusCFilter Filters in https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/common/Filters
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool

  NANOttbarBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("NANOTTbarPlusBFilterTool")
  NANOttbarBfiltertool.SelectB = True
  NANOttbarBfiltertool.BpTMinCut = 5000
  NANOttbarBfiltertool.BMultiplicityCut = 1 # >=
  ToolSvc += NANOttbarBfiltertool

  NANOttbarBBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("NANOTTbarPlusBBFilterTool")
  NANOttbarBBfiltertool.SelectB = True
  NANOttbarBBfiltertool.BpTMinCut = 15000
  NANOttbarBBfiltertool.BMultiplicityCut = 2 # >=
  ToolSvc += NANOttbarBBfiltertool

  NANOttbarCfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("NANOTTbarPlusCFilterTool")
  NANOttbarCfiltertool.SelectC = True
  NANOttbarCfiltertool.CpTMinCut = 15000
  NANOttbarCfiltertool.CMultiplicityCut = 1 # >=
  # these two are the default values
  # B-hadrons have precedence; if one B is found, it won't pass the CFilter
  NANOttbarCfiltertool.BpTMinCut = 5000
  NANOttbarCfiltertool.BMultiplicityCut = 1 # >=
  ToolSvc += NANOttbarCfiltertool

  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
  NANOTopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "NANOTopHFFilterAugmentation")
  NANOTopHFFilterAugmentation.BFilterTool = NANOttbarBfiltertool
  NANOTopHFFilterAugmentation.BBFilterTool = NANOttbarBBfiltertool
  NANOTopHFFilterAugmentation.CFilterTool = NANOttbarCfiltertool
  ToolSvc += NANOTopHFFilterAugmentation
  AugmentationTools.append(NANOTopHFFilterAugmentation)
  nanolog.info("NANOTopHFFilterAugmentationTool: {!s}".format(NANOTopHFFilterAugmentation))


#====================================================================
# THINNING TOOL 
#====================================================================

# Thin all unless kept by something else
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__NANOThinningTool
NANOThinningTool = DerivationFramework__NANOThinningTool(name			 = "NANOThinningTool",
                                                         ThinningService	 = NANOThinningHelper.ThinningSvc(),
                                                         ContainersToThin  = ["InDetTrackParticles",
                                                                              "CaloCalTopoClusters",
                                                                              "egammaClusters",
                                                                              "GSFTrackParticles"])
ToolSvc += NANOThinningTool
thinningTools.append(NANOThinningTool)


# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
NANOMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "NANOMuonTPThinningTool",
									 ThinningService	 = NANOThinningHelper.ThinningSvc(),
									 MuonKey		 = "AnalysisMuons_NOSYS",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += NANOMuonTPThinningTool
thinningTools.append(NANOMuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
NANOElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "NANOElectronTPThinningTool",
										 ThinningService	 = NANOThinningHelper.ThinningSvc(),
                                                                                 SGKey			 = "AnalysisElectrons_NOSYS",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += NANOElectronTPThinningTool
thinningTools.append(NANOElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
NANOPhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "NANOPhotonTPThinningTool",
										 ThinningService	 = NANOThinningHelper.ThinningSvc(),
										 SGKey			 = "AnalysisPhotons_NOSYS",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += NANOPhotonTPThinningTool
thinningTools.append(NANOPhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
NANOTauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "NANOTauTPThinningTool",
									ThinningService = NANOThinningHelper.ThinningSvc(),
									TauKey  	= "AnalysisTauJets_NOSYS",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += NANOTauTPThinningTool
thinningTools.append(NANOTauTPThinningTool)

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
NANOPhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "NANOPhotonCCThinningTool",
                                                                                     ThinningService         = NANOThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "AnalysisPhotons_NOSYS",
                                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                #     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
                                                                                     ConeSize                = 0.4)
ToolSvc += NANOPhotonCCThinningTool
thinningTools.append(NANOPhotonCCThinningTool)


# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
NANOPhotonEGammaCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "NANOPhotonEGammaCThinningTool",
                                                                                     ThinningService         = NANOThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "AnalysisPhotons_NOSYS",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     ConeSize                = 0.4)
ToolSvc += NANOPhotonEGammaCThinningTool
thinningTools.append(NANOPhotonEGammaCThinningTool)



#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Calo Clusters associated with Electrons
NANOElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "NANOElectronCCThinningTool",
                                                                                     ThinningService         = NANOThinningHelper.ThinningSvc(),
      SGKey                   = "AnalysisElectrons_NOSYS",
                                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                   #  SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     ConeSize                = 0.2)
ToolSvc += NANOElectronCCThinningTool
thinningTools.append(NANOElectronCCThinningTool)

# Calo Clusters associated with Electrons
NANOElectronEGammaCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "NANOElectronEGammaCThinningTool",
                                                                                     ThinningService         = NANOThinningHelper.ThinningSvc(),
      SGKey                   = "AnalysisElectrons_NOSYS",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     ConeSize                = 0.2)
ToolSvc += NANOElectronEGammaCThinningTool
thinningTools.append(NANOElectronEGammaCThinningTool)


# Calo Clusters associated with Muons
NANOMuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "NANOMuonCCThinningTool",
                                                                                     ThinningService         = NANOThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Muons",
                                                                                     #CaloClCollectionSGKey   = "MuonClusterCollection",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Muons.pt > 6*GeV",
                                                                                     ConeSize                = 0.2)
ToolSvc += NANOMuonCCThinningTool
thinningTools.append(NANOMuonCCThinningTool)

#add AND with Trigger skimming criteria
trigExpr = '('+' || '.join(SUSY7AllTriggers)+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
NANOtrigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "NANOtrigSkimmingTool",
                                                                  TriggerListOR = SUSY7AllTriggers)
   
ToolSvc += NANOtrigSkimmingTool
   
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
NANOSkimmingTool = DerivationFramework__FilterCombinationAND(name = "NANOSkimmingTool",
                                                              FilterList = [NANOtrigSkimmingTool])
ToolSvc += NANOSkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
   
   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqNANO += CfgMgr.DerivationFramework__DerivationKernel("NANOKernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("NANO")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# Jet building
#==============================================================================
OutputJets["NANO"] = []
reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets" ]

# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically (AntiKt4PV0TrackJets is not supported in R21)
replaceAODReducedJets(reducedJetList, SeqNANO, "NANO")

# re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqNANO)

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(SeqNANO, "NANO")

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
#sysLoader.sigmaRecommended = 1
sysLoader.systematicsList= ['']
SeqNANO += sysLoader

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
SeqNANO += pileupSequence

# Include, and then set up the electron analysis sequence:
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
    makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( dataType, 'LooseLHElectron.GradientLoose', deepCopyOutput = True )
electronSequence.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons' )
print( electronSequence ) # For debugging

# Add the electron sequence to the job:
SeqNANO += electronSequence

# Include, and then set up the photon analysis sequence:                                       
from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
    makePhotonAnalysisSequence
photonSequence = makePhotonAnalysisSequence( dataType, 'Tight.FixedCutTight', deepCopyOutput = True, recomputeIsEM=True )
photonSequence.configure( inputName = 'Photons',
                          outputName = 'AnalysisPhotons' )
print( photonSequence ) # For debugging

SeqNANO += photonSequence

# Include, and then set up the muon analysis algorithm sequence:
 
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType, deepCopyOutput = True, workingPoint = 'Medium.Iso' )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons_%SYS%' )
print( muonSequence ) # For debugging

# Add the sequence to the job:
 
SeqNANO += muonSequence

# Include, and then set up the tau analysis algorithm sequence:                                                    
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType, 'Tight', deepCopyOutput = True )
tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets' )
print( tauSequence ) # For debugging                                                                               
# Add the sequence to the job:                                                                                     
SeqNANO += tauSequence

jetContainer = "AntiKt4EMTopoJets"

# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer, deepCopyOutput = True )
jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets' )
print( jetSequence ) # For debugging

SeqNANO += jetSequence


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqNANO += CfgMgr.DerivationFramework__DerivationKernel(
   "NANOKernelSkim",
   SkimmingTools = [NANOSkimmingTool]
   )


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
SeqNANO += CfgMgr.DerivationFramework__DerivationKernel(
   "NANOKernelAug",
   AugmentationTools = AugmentationTools,
   ThinningTools = thinningTools,
   )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
NANOSlimmingHelper = SlimmingHelper("NANOSlimmingHelper")

NANOSlimmingHelper.IncludeJetTriggerContent = True
NANOSlimmingHelper.IncludeMuonTriggerContent = True
NANOSlimmingHelper.IncludeEGammaTriggerContent = True
NANOSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
NANOSlimmingHelper.IncludeTauTriggerContent = True
NANOSlimmingHelper.IncludeEtMissTriggerContent = True
NANOSlimmingHelper.IncludeBJetTriggerContent = True
NANOSlimmingHelper.IncludeBPhysTriggerContent = True

#addJetOutputs(NANOSlimmingHelper,["LargeR", "NANO"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets",
#                                                            "AntiKt4TruthWZJets","AntiKt4TruthJets","AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"])

# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
NANOSlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                          'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                         'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                         'AnalysisElectrons_NOSYS':'xAOD::ElectronContainer', 
                                         'AnalysisElectrons_NOSYSAux':'xAOD::ElectronAuxContainer',
                                         'AnalysisMuons_NOSYS':'xAOD::MuonContainer', 
                                         'AnalysisMuons_NOSYSAux':'xAOD::MuonAuxContainer',
                                         'AnalysisJets_NOSYS':'xAOD::JetContainer',
                                         'AnalysisJets_NOSYSAux':'xAOD::AuxContainerBase',
                                         'AnalysisPhotons_NOSYS':'xAOD::PhotonContainer', 
                                         'AnalysisPhotons_NOSYSAux':'xAOD::PhotonAuxContainer',
                                         'AnalysisTauJets_NOSYS':'xAOD::TauJetContainer', 
                                         'AnalysisTauJets_NOSYSAux':'xAOD::TauJetAuxContainer',
                                         'AntiKt4EMTopoJets':'xAOD::JetContainer'
                                         }

# Leaving these as smart collections

NANOSlimmingHelper.SmartCollections = [
  "InDetTrackParticles",
  "PrimaryVertices"
  ]

# Variables to save, per object type

NANOSlimmingHelper.ExtraVariables = [ 
  "AnalysisElectrons_NOSYS.trackParticleLinks.pt.eta.phi.m.charge.author.DFCommonElectronsLHVeryLoose.DFCommonElectronsLHLoose.DFCommonElectronsLHLooseBL.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsLHVeryLooseIsEMValue.DFCommonElectronsLHLooseIsEMValue.DFCommonElectronsLHLooseBLIsEMValue.DFCommonElectronsLHMediumIsEMValue.DFCommonElectronsLHTightIsEMValue.DFCommonElectronsECIDS.DFCommonElectronsECIDSResult.ptvarcone20.ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.caloClusterLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType.truthPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink.firstEgMotherPdgId.ambiguityType",
  "AnalysisPhotons_NOSYS.pt.eta.phi.m.author.OQ.DFCommonPhotonsIsEMLoose.DFCommonPhotonsIsEMTight.DFCommonPhotonsIsEMTightIsEMValue.DFCommonPhotonsIsEMTightPtIncl.DFCommonPhotonsIsEMTightPtInclIsEMValue.DFCommonPhotonsCleaning.DFCommonPhotonsCleaningNoTime.ptcone20.topoetcone20.topoetcone40.topoetcone20ptCorrection.topoetcone40ptCorrection.caloClusterLinks.vertexLinks.ambiguityLink.truthParticleLink.truthOrigin.truthType",
  "GSFTrackParticles.chiSquared.phi.d0.theta.qOverP.definingParametersCovMatrix.z0.vz.charge.vertexLink",
  "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
  "egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME.ETA2CALOFRAME.PHI2CALOFRAME.constituentClusterLinks",
  "AnalysisMuons_NOSYS.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink.allAuthors.ptcone20_TightTTVA_pt1000.ptcone20_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500",
  "CombinedMuonTrackParticles.qOverP.d0.z0.vz.phi.theta.truthOrigin.truthType.definingParametersCovMatrix",  
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "MuonSpectrometerTrackParticles.phi.d0.z0.vz.definingParametersCovMatrix.vertexLink.theta.qOverP.truthParticleLink",
  "AnalysisTauJets_NOSYS.pt.eta.phi.m.tauTrackLinks.jetLink.charge.isTauFlags.BDTJetScore.BDTEleScore.ptFinalCalib.etaFinalCalib.phiFinalCalib.mFinalCalib.ele_match_lhscore.ele_olr_pass.electronLink.IsVeryLoose.EleMatchLikelihoodScore.pt_combined.eta_combined.phi_combined.m_combined.BDTJetScoreSigTrans.BDTEleScoreSigTrans.PanTau_DecayMode.RNNJetScore.RNNJetScoreSigTrans.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
  "TauTracks.pt.eta.phi.flagSet.trackLinks.bdtScores",
  "AnalysisJets_NOSYS.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.NumTrkPt500.SumPtTrkPt500.DetectorEta.Jvt.JVFCorr.JvtRpt.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.PartonTruthLabelID.ConeTruthLabelID.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.TrueFlavor.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.btagging.btaggingLink",
  "BTagging_AntiKt4EMTopo.MV2c10_discriminant",
  "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
  "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection"
  ]

NANOSlimmingHelper.AllVariables = [
  "MET_Truth", 
  "MET_Track",
  "MET_Core_AntiKt4EMTopo",
  "METAssoc_AntiKt4EMTopo"
  ]

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py) 
if DerivationFrameworkIsMonteCarlo:   
  NANOSlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson", "TruthWbosonWithDecayParticles", "TruthWbosonWithDecayVertices"]

NANOSlimmingHelper.AppendContentToStream(NANOStream)

