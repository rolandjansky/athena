#
##  Modified on top of proposed version by Vangelis @ https://its.cern.ch/jira/browse/ATLSUSYDPD-417
#
#********************************************************************
# SUSY10.py 
# reductionConf flag SUSY10 in Reco_tf.py (Gtt/Gbb multi-b analysis)
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
if DerivationFrameworkIsMonteCarlo:
   from DerivationFrameworkMCTruth.HFHadronsCommon import *
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
   addStandardTruthContents()

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY10Stream )
SUSY10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY10Stream.AcceptAlgs(["SUSY10KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY10ThinningHelper = ThinningHelper( "SUSY10ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY10 = CfgMgr.AthSequencer("SeqSUSY10")
DerivationFrameworkJob += SeqSUSY10

#====================================================================
# Trigger navigation thinning
#====================================================================
#triggerRegEx = ['HLT_xe.*','HLT_e.*','HLT_mu.*']
from DerivationFrameworkSUSY.SUSY10TriggerList import * 

SUSY10ThinningHelper.TriggerChains = '|'.join( SUSY10_leptonTrig+SUSY10_jetxeTrig+SUSY10_photonTrig+SUSY10_dileptonTrig )
SUSY10ThinningHelper.AppendToStream( SUSY10Stream )


#====================================================================
# THINNING TOOL 
#====================================================================

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#SUSY10TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY10TPThinningTool",
#								 ThinningService	 = SUSY10ThinningHelper.ThinningSvc(),
#								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
#								 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY10TPThinningTool
#thinningTools.append(SUSY10TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY10MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY10MuonTPThinningTool",
									 ThinningService	 = SUSY10ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY10MuonTPThinningTool
thinningTools.append(SUSY10MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY10ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY10ElectronTPThinningTool",
										 ThinningService	 = SUSY10ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY10ElectronTPThinningTool
thinningTools.append(SUSY10ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY10PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY10PhotonTPThinningTool",
										 ThinningService	 = SUSY10ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY10PhotonTPThinningTool
thinningTools.append(SUSY10PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY10TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY10TauTPThinningTool",
									ThinningService = SUSY10ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY10TauTPThinningTool
thinningTools.append(SUSY10TauTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
# Calo Clusters associated with Electrons
SUSY10ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY10ElectronCCThinningTool",
                                                                       ThinningService         = SUSY10ThinningHelper.ThinningSvc(),
                                                                       SGKey                   = "Electrons",
                                                                       #CaloClCollectionSGKey   = "egammaClusters",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "Electrons.pt > 7*GeV",
                                                                       ConeSize                = 0.2)
ToolSvc += SUSY10ElectronCCThinningTool
thinningTools.append(SUSY10ElectronCCThinningTool)

# Calo Clusters associated with Muons
SUSY10MuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY10MuonCCThinningTool",
                                                                   ThinningService         = SUSY10ThinningHelper.ThinningSvc(),
                                                                   SGKey                   = "Muons",
                                                                   #CaloClCollectionSGKey   = "MuonClusterCollection",
                                                                   TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                   SelectionString         = "Muons.pt > 6*GeV",
                                                                   ConeSize                = 0.2)
ToolSvc += SUSY10MuonCCThinningTool
thinningTools.append(SUSY10MuonCCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY10TruthThinningTool = DerivationFramework__MenuTruthThinning(
    name              = "SUSY10TruthThinningTool",
    ThinningService              = SUSY10ThinningHelper.ThinningSvc(),
    WritePartons                 = False,
    WriteHadrons                 = False,
    WriteBHadrons                = False,
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
    WriteFirstN                  = 15,
    PreserveAncestors            = True,
    PreserveGeneratorDescendants = False,
    SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset,
    WritettHFHadrons             = True,
  )
  ToolSvc += SUSY10TruthThinningTool
  thinningTools.append(SUSY10TruthThinningTool)



#=======================================
# JET CALIBRATION AND DEFINITION
#=======================================
# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY10)
jetsDefinition = ' (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8) '

#====================================================================
# SKIMMING SELECTION
#====================================================================

# TRIGGER SELECTION
expression = '(' + ' || '.join(SUSY10_jetxeTrig + SUSY10_leptonTrig  + SUSY10_photonTrig + SUSY10_dileptonTrig) + ')' 


# OBJECT SELECTION
expression += ' && ( count('+jetsDefinition+')>=4 ) '

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY10SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "SUSY10SkimmingTool", expression = expression)
ToolSvc += SUSY10SkimmingTool

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
   SeqSUSY10 += CfgMgr.DerivationFramework__DerivationKernel("SUSY10KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY10")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY10 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY10KernelSkim",
  SkimmingTools = [SUSY10SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY10"] = []
reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically (AntiKt4PV0TrackJets is not supported in R21)
replaceAODReducedJets(reducedJetList, SeqSUSY10, "SUSY10")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY10, "SUSY10")


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
SeqSUSY10 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY10KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================

#Variables
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY10SlimmingHelper = SlimmingHelper("SUSY10SlimmingHelper")
SUSY10SlimmingHelper.SmartCollections = ["Electrons","Photons","MET_Reference_AntiKt4EMTopo","Muons","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices", 
                                         "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
# BTagging_AntiKt4Track no longer supported in R21, replaced with BTagging_AntiKt2Track for now
SUSY10SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt2PV0TrackJets", "BTagging_AntiKt2Track", "MET_Track"]
SUSY10SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                       "Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                       "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.HadronConeExclTruthLabelID.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                                       "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                                       "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                       "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
                                       "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
                                       "MuonClusterCollection.eta_sampl.phi_sampl",
                                       "Muons.quality.etcone20.ptconecoreTrackPtrCorrection",
                                       "TruthParticles.TopHadronOriginFlag",
                                       "Electrons.quality.etcone20.ptconecoreTrackPtrCorrection"]

#Triggers kept
SUSY10SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY10SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY10SlimmingHelper.IncludeEtMissTriggerContent = True
#Triggers removed
SUSY10SlimmingHelper.IncludeBPhysTriggerContent = False
SUSY10SlimmingHelper.IncludeJetTriggerContent   = False
SUSY10SlimmingHelper.IncludeTauTriggerContent   = False
SUSY10SlimmingHelper.IncludeBJetTriggerContent  = False

if DerivationFrameworkIsMonteCarlo:

  # Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
  SUSY10SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  # All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
  SUSY10SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]


#Removes useless fatjet collections - addJetOutputs no longer very appropriate, as we rebuild many jet collections
#AntiKt10TruthTrimmedPtFrac5SmallR20Jets and AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets added to 'AllVariables'
#addJetOutputs(SUSY10SlimmingHelper, ["LargeR","SUSY10"],[],["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets",
#                                                            "AntiKt4TruthWZJets","AntiKt4TruthJets","AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets"])

SUSY10SlimmingHelper.AppendContentToStream(SUSY10Stream)

