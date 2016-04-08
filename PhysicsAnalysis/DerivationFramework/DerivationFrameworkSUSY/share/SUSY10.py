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
triggerRegEx = ['HLT_xe.*','HLT_e.*','HLT_mu.*']
SUSY10ThinningHelper.TriggerChains = '|'.join(triggerRegEx)

SUSY10ThinningHelper.AppendToStream( SUSY10Stream )


#====================================================================
# THINNING TOOL 
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

SUSY10TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY10TPThinningTool",
								 ThinningService	 = SUSY10ThinningHelper.ThinningSvc(),
								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
								 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY10TPThinningTool
thinningTools.append(SUSY10TPThinningTool)

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


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  SUSY10Classifier = MCTruthClassifier( name = "SUSY10Classifier",
                                        ParticleCaloExtensionTool = "" )
  ToolSvc += SUSY10Classifier
                
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  SUSY10ClassificationDecorator = DerivationFramework__TruthClassificationDecorator( name             = "SUSY10ClassificationDecorator",
                                                                                    ParticlesKey      = "TruthParticles",
                                                                                    MCTruthClassifier = SUSY10Classifier)
  ToolSvc += SUSY10ClassificationDecorator
  AugmentationTools.append(SUSY10ClassificationDecorator)

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
    WriteFirstN                  = -1,
    PreserveAncestors            = True,
    PreserveGeneratorDescendants = False,
    SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset,
    WritettHFHadrons             = True,
  )
  ToolSvc += SUSY10TruthThinningTool
  thinningTools.append(SUSY10TruthThinningTool)


#====================================================================
# TRIGGER AND OBJECT SKIMMING
#====================================================================
#
# TRIGGER
#
jetxeTrig = [
  'HLT_xe100', 'HLT_xe100_tc_lcw', 'HLT_xe100_tc_em', 'HLT_xe100_mht', 'HLT_xe100_pueta',
  'HLT_xe100_pufit', 'HLT_xe100_wEFMu', 'HLT_xe100_tc_lcw_wEFMu', 'HLT_xe100_tc_em_wEFMu', 'HLT_xe100_mht_wEFMu', 'HLT_xe100_pueta_wEFMu', 'HLT_xe100_pufit_wEFMu',
  'HLT_xe70', 'HLT_xe70_tc_lcw', 'HLT_xe70_tc_em', 'HLT_xe70_mht','HLT_xe70_pueta',
  'HLT_xe70_pufit', 'HLT_xe70_wEFMu', 'HLT_xe70_tc_lcw_wEFMu', 'HLT_xe70_tc_em_wEFMu', 'HLT_xe70_mht_wEFMu', 'HLT_xe70_pueta_wEFMu', 'HLT_xe70_pufit_wEFMu',
  'HLT_xe80', 'HLT_xe80_tc_lcw', 'HLT_xe80_tc_em', 'HLT_xe80_mht','HLT_xe80_pueta',
  'HLT_xe80_pufit', 'HLT_xe80_wEFMu', 'HLT_xe80_tc_lcw_wEFMu', 'HLT_xe80_tc_em_wEFMu', 'HLT_xe80_mht_wEFMu', 'HLT_xe80_pueta_wEFMu', 'HLT_xe80_pufit_wEFMu'
]

leptonTrig = [
  'HLT_e24_lhmedium_L1EM18VH','HLT_e24_lhmedium_L1EM20VH','HLT_e60_lhmedium', 'HLT_e120_lhloose', 'HLT_e24_lhvloose_L1EM18VH','HLT_e24_lhmedium_iloose_L1EM18VH','HLT_e24_lhmedium_iloose_L1EM20VH','HLT_e24_lhtight_iloose_L1EM20VH','HLT_e24_lhtight_iloose','HLT_e26_lhtight_iloose',
  'HLT_mu20_L1MU15','HLT_mu24','HLT_mu20_iloose_L1MU15','HLT_mu24_iloose_L1MU15','HLT_mu26','HLT_mu24_imedium','HLT_mu26_imedium','HLT_mu40','HLT_mu50','HLT_mu60_0eta105_msonly'
]

expression = '(' + ' || '.join(jetxeTrig + leptonTrig) + ')' 

#
# OBJECTS
#
applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY10)
jetsDefinition = ' (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8) '
expression += ' && ( count('+jetsDefinition+')>=4 ) '


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY10SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY10SkimmingTool",
                                                                expression = expression)
ToolSvc += SUSY10SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqSUSY10 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY10KernelSkim",
  SkimmingTools = [SUSY10SkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY10")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY10)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY10GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY10GenFilt)



#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY10 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY10KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

#=======================================
# add jet building, cf. example in JETM1
#=======================================
# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(SeqSUSY10, "SUSY10")


#====================================================================
# CONTENT LIST  
#====================================================================

#Variables
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY10SlimmingHelper = SlimmingHelper("SUSY10SlimmingHelper")
SUSY10SlimmingHelper.SmartCollections = ["Electrons","Photons","MET_Reference_AntiKt4EMTopo","Muons","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY10SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt4PV0TrackJets", "BTagging_AntiKt4Track", "MET_Track"]
SUSY10SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
					"Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
					"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.HadronConeExclTruthLabelID.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
					"GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
					"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]

#Triggers kept
SUSY10SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY10SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY10SlimmingHelper.IncludeEtMissTriggerContent = True
#Triggers removed
SUSY10SlimmingHelper.IncludeBPhysTriggerContent = False
SUSY10SlimmingHelper.IncludeJetTriggerContent   = False
SUSY10SlimmingHelper.IncludeTauTriggerContent   = False
SUSY10SlimmingHelper.IncludeBJetTriggerContent  = False

#Removes useless fatjet collections
addJetOutputs(SUSY10SlimmingHelper, ["LargeR","SUSY10"],[],["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets"])

SUSY10SlimmingHelper.AppendContentToStream(SUSY10Stream)
