#********************************************************************
# SUSY16.py 
# reductionConf flag SUSY16 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY16Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY16Stream )
SUSY16Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY16Stream.AcceptAlgs(["SUSY16KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY16ThinningHelper = ThinningHelper( "SUSY16ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY16 = CfgMgr.AthSequencer("SeqSUSY16")
DerivationFrameworkJob += SeqSUSY16


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY16TriggerList import triggersNavThin
SUSY16ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY16ThinningHelper.AppendToStream( SUSY16Stream )
     
#====================================================================
# THINNING TOOLS 
#====================================================================

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY16MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY16MetTPThinningTool",
                                                                      ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                      SelectionString         = thinning_expression,
                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                      ApplyAnd                = True)
ToolSvc += SUSY16MetTPThinningTool
thinningTools.append(SUSY16MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY16JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "SUSY16JetTPThinningTool",
                                                                         ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                         JetKey                  = "AntiKt4EMTopoJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         ApplyAnd                = True)
ToolSvc += SUSY16JetTPThinningTool
thinningTools.append(SUSY16JetTPThinningTool)

# TrackParticles directly
SUSY16TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY16TPThinningTool",
                                                                  ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16TPThinningTool
thinningTools.append(SUSY16TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY16MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY16MuonTPThinningTool",
                                                                          ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16MuonTPThinningTool
thinningTools.append(SUSY16MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY16ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY16ElectronTPThinningTool",
                                                                                ThinningService        = SUSY16ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY16ElectronTPThinningTool
thinningTools.append(SUSY16ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY16PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY16PhotonTPThinningTool",
                                                                              ThinningService	 = SUSY16ThinningHelper.ThinningSvc(),
                                                                              SGKey			 = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16PhotonTPThinningTool
thinningTools.append(SUSY16PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY16TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY16TauTPThinningTool",
                                                                         ThinningService = SUSY16ThinningHelper.ThinningSvc(),
                                                                         TauKey          = "TauJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY16TauTPThinningTool
thinningTools.append(SUSY16TauTPThinningTool)

# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Keep clusters associated to electrons
SUSY16ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY16ElectronCCThinningTool',
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = 'Electrons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.4)
ToolSvc += SUSY16ElectronCCThinningTool
thinningTools.append(SUSY16ElectronCCThinningTool)

# Keep clusters associated to photons
SUSY16PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY16PhotonCCThinningTool',
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.4)
ToolSvc += SUSY16PhotonCCThinningTool
thinningTools.append(SUSY16PhotonCCThinningTool)


# Calo Clusters associated with Muons
SUSY16MuonCCThinningTool = DerivationFramework__CaloClusterThinning( 
    name = "SUSYxMuonCCThinningTool",
    ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
    SGKey                   = "Muons",
    TopoClCollectionSGKey   = "CaloCalTopoClusters",
    SelectionString         = "Muons.pt > 3*GeV",
    ConeSize                = 0.4)
ToolSvc += SUSY16MuonCCThinningTool
thinningTools.append(SUSY16MuonCCThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY16TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY16TruthThinningTool",
                                                                     ThinningService              = SUSY16ThinningHelper.ThinningSvc(),
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

   
    # Decorate Electron with bkg electron type/origin 
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BkgElectronClassificationTool
    AugmentationTools.append(BkgElectronClassificationTool)
    
    ToolSvc += SUSY16TruthThinningTool
    thinningTools.append(SUSY16TruthThinningTool)

	
#====================================================================
# SKIMMING TOOL 
#====================================================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY16)

# jet_prefab = 'count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > XPT*GeV && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < XETA))'
# count_jet_hard = jet_prefab.replace("XPT", "150.").replace("XETA", "2.8")
# count_jet_soft = jet_prefab.replace("XPT", "80.").replace("XETA", "2.8")
# jetSelection= '('+count_jet_soft+'>=1)'

# ------------------------------------------------------------
# Lepton selection
muonsRequirements = '(Muons.pt > 3*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 3*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
leptonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 2)'
expression='('+leptonSelection+')'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY16LeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY16LeptonSkimmingTool",
                                                                        expression = expression)
ToolSvc += SUSY16LeptonSkimmingTool
# ------------------------------------------------------------

# ------------------------------------------------------------
# Trigger selection
from DerivationFrameworkSUSY.SUSY16TriggerList import triggersMET,triggersSoftMuon,triggersJetPlusMet,triggersSoftMuonEmulation
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND


# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
trigReq=triggersMET+triggersSoftMuon+triggersJetPlusMet
SUSY16InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = trigReq)
ToolSvc += SUSY16InclusiveTriggerSkimmingTool

# full trigger selection, do it slightly differently for data and MC
SUSY16TriggerSkimmingTool=None
if DerivationFrameworkIsMonteCarlo:
    # one muon + jet + met trigger
    SUSY16SoftOneMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16OneMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_mu4','HLT_xe50_mht','HLT_j110'])
    ToolSvc += SUSY16SoftOneMuonTriggerSkimmingTool
    
    # dimuon + jet + met trigger
    SUSY16SoftTwoMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16TwoMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_2mu4','HLT_j85'])
    ToolSvc += SUSY16SoftTwoMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY16TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY16TriggerSkimmingTool", 
                                                                         FilterList = [SUSY16InclusiveTriggerSkimmingTool,
                                                                                       SUSY16SoftOneMuonTriggerSkimmingTool,
                                                                                       SUSY16SoftTwoMuonTriggerSkimmingTool])
    ToolSvc += SUSY16TriggerSkimmingTool
else:
    # for data we can keep all of the prescaled triggers, may be nice to have them.
    SUSY16SoftMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY16SoftMuonTriggerSkimmingTool",
                                                                                     TriggerListOR = triggersSoftMuonEmulation)
    ToolSvc += SUSY16SoftMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY16TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY16TriggerSkimmingTool", 
                                                                         FilterList = [SUSY16InclusiveTriggerSkimmingTool,
                                                                                       SUSY16SoftMuonTriggerSkimmingTool])
    ToolSvc += SUSY16TriggerSkimmingTool
    
# ------------------------------------------------------------

# ------------------------------------------------------------
# Final skim selection, with trigger selection and lepton selection
SUSY16SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY16SkimmingTool", 
                                                               FilterList = [SUSY16LeptonSkimmingTool, 
                                                                             SUSY16TriggerSkimmingTool])
ToolSvc += SUSY16SkimmingTool
# ------------------------------------------------------------

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY16_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY16_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY16_MaxCellDecoratorTool

# Calo Clusters associated with Electrons
SUSY16ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name = "SUSY16ElectronCCThinningTool",
                                                                        ThinningService         = SUSY16ThinningHelper.ThinningSvc(),
                                                                        SGKey                   = "Electrons",
                                                                        CaloClCollectionSGKey   = "egammaClusters",
                                                                        TopoClCollectionSGKey   = "",
                                                                        #SelectionString         = "Electrons.pt > 15*GeV",
                                                                        #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                        ConeSize                = 0)
ToolSvc += SUSY16ElectronCCThinningTool


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
    SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel("SUSY16KernelSigAug",
                                                              AugmentationTools = DecorateSUSYProcess("SUSY16")
                                                              )

    from DerivationFrameworkSUSY.SUSYWeightMetadata import *

#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY16KernelSkim",
    SkimmingTools = [SUSY16SkimmingTool]
    )


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY16"] = []
reducedJetList = [ "AntiKt2PV0TrackJets" ]
if DerivationFrameworkIsMonteCarlo:
  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY16, "SUSY16")


#==============================================================================
# Tau truth building/matching
#==============================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
  addTruthTaus(AugmentationTools)
    

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY16 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY16KernelAug",
    AugmentationTools = AugmentationTools,
    ThinningTools = thinningTools,
    )


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY16SlimmingHelper = SlimmingHelper("SUSY16SlimmingHelper")
SUSY16SlimmingHelper.SmartCollections = ["Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "AntiKt4EMTopoJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "BTagging_AntiKt4EMTopo",
                                         "InDetTrackParticles",
                                         "PrimaryVertices"]
SUSY16SlimmingHelper.AllVariables = ["TruthParticles", 
                                     "TruthEvents", 
                                     "TruthVertices", 
                                     #"AntiKt4TruthJets",
                                     "MET_Track",
                                     "MET_LocHadTopo",
                                     "MuonSegments",
                                     "MET_Truth"]					
SUSY16SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.eta_sampl.phi_sampl",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink"
                                      + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.firstEgMotherPdgId.truthPdgId.bkgTruthParticleLink.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c10_discriminant",
                                      "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl"
]

SUSY16SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY16SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY16SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY16SlimmingHelper.IncludeJetTriggerContent    = True
SUSY16SlimmingHelper.IncludeTauTriggerContent    = True
SUSY16SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY16SlimmingHelper.IncludeBJetTriggerContent   = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY16SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY16SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY16SlimmingHelper.AppendContentToStream(SUSY16Stream)


