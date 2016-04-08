#********************************************************************
# SUSY2.py 
# reductionConf flag SUSY2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY2Stream )
SUSY2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY2Stream.AcceptAlgs(["SUSY2KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY2ThinningHelper = ThinningHelper( "SUSY2ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY2 = CfgMgr.AthSequencer("SeqSUSY2")
DerivationFrameworkJob += SeqSUSY2


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY2TriggerList import triggersNavThin
SUSY2ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY2ThinningHelper.AppendToStream( SUSY2Stream )


#====================================================================
# THINNING TOOLS 
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
SUSY2TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY2TPThinningTool",
                                                                 ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2TPThinningTool
thinningTools.append(SUSY2TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY2MuonTPThinningTool",
                                                                         ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2MuonTPThinningTool
thinningTools.append(SUSY2MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY2ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY2ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 BestMatchOnly          = False,
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY2ElectronTPThinningTool
thinningTools.append(SUSY2ElectronTPThinningTool)
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY2PhotonTPThinningTool",
										 ThinningService	 = SUSY2ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2PhotonTPThinningTool
thinningTools.append(SUSY2PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY2TauTPThinningTool",
                                                                        ThinningService = SUSY2ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2TauTPThinningTool
thinningTools.append(SUSY2TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY2TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY2TruthThinningTool",
                                                       ThinningService              = SUSY2ThinningHelper.ThinningSvc(),
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
  ToolSvc += SUSY2TruthThinningTool
  thinningTools.append(SUSY2TruthThinningTool)

from RecExConfig.RecFlags  import rec
if rec.doTruth():
  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
#SUSY2TRUTHTauTool = DerivationFramework__TruthCollectionMakerTau(
  #name              = "SUSY2TRUTHTauTool",
  #NewCollectionName = "SUSY2TRUTHTaus"
#)

#if globalflags.DataSource()=='geant4':
##if 0: # ERROR SG::ExcAuxTypeMismatch: Type mismatch for aux variable `::particleOrigin' (1174); old type is int new type is unsigned int <- occurs only when run in parallel with e.g. SUSY4
	#ToolSvc += SUSY2TRUTHTauTool
	#AugmentationTools.append(SUSY2TRUTHTauTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================
muonsRequirements = '(Muons.pt >= 9*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 9*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelection = 'count('+electronsRequirements+') + count('+muonsRequirements+') >= 2'

xeTriggers = ['HLT_xe35', # list taken from SUSY6 without mht and tc_lcw
              'HLT_xe35_tc_em',
              'HLT_xe35_pueta',
              'HLT_xe35_pufit',
              'HLT_xe35_L2FS',
              'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu',
              'HLT_xe35_wEFMu',
              'HLT_xe35_tc_em_wEFMu',
              'HLT_xe35_pueta_wEFMu',
              'HLT_xe35_pufit_wEFMu',
              'HLT_xe50',
              'HLT_xe50_tc_em',
              'HLT_xe50_pueta',
              'HLT_xe50_pufit',
              'HLT_xe60',
              'HLT_xe60_tc_em',
              'HLT_xe60_pueta',
              'HLT_xe60_pufit',
              'HLT_xe60_wEFMu',
              'HLT_xe60_tc_em_wEFMu',
              'HLT_xe60_pueta_wEFMu',
              'HLT_xe60_pufit_wEFMu',
              'HLT_xe70_wEFMu',
              'HLT_xe70_tc_em_wEFMu',
              'HLT_xe70_pueta_wEFMu',
              'HLT_xe70_pufit_wEFMu',
              'HLT_xe70',
              'HLT_xe70_tc_em',
              'HLT_xe70_pueta',
              'HLT_xe70_pufit',
              'HLT_xe80_L1XE50',
              'HLT_xe80_tc_em_L1XE50',
              'HLT_xe80_pueta_L1XE50',
              'HLT_xe80_pufit_L1XE50',
              'HLT_xe80_wEFMu_L1XE50',
              'HLT_xe80_tc_em_wEFMu_L1XE50',
              'HLT_xe80_pueta_wEFMu_L1XE50',
              'HLT_xe80_pufit_wEFMu_L1XE50',
              'HLT_xe80',
              'HLT_xe80_tc_em',
              'HLT_xe80_pueta',
              'HLT_xe80_pufit',
              'HLT_xe80_wEFMu',
              'HLT_xe80_tc_em_wEFMu',
              'HLT_xe80_pueta_wEFMu',
              'HLT_xe80_pufit_wEFMu',
              'HLT_xe80_L1XE70',
              'HLT_xe80_tc_em_L1XE70',
              'HLT_xe80_pueta_L1XE70',
              'HLT_xe80_pufit_L1XE70',
              'HLT_xe80_wEFMu_L1XE70',
              'HLT_xe80_tc_em_wEFMu_L1XE70',
              'HLT_xe80_pueta_wEFMu_L1XE70',
              'HLT_xe80_pufit_wEFMu_L1XE70',
              'HLT_xe100',
              'HLT_xe100_tc_em',
              'HLT_xe100_pueta',
              'HLT_xe100_pufit',
              'HLT_xe100_wEFMu',
              'HLT_xe100_tc_em_wEFMu',
              'HLT_xe100_pueta_wEFMu',
              'HLT_xe100_pufit_wEFMu']

if DerivationFrameworkIsMonteCarlo:
  xeTriggers = [trig for trig in xeTriggers if 'xe100' in trig]
xeTriggers = '(' + ' || '.join(xeTriggers) + ')' 


muonTrig       = '(HLT_mu26_imedium || HLT_mu24_imedium || HLT_mu24_iloose_L1MU15 || HLT_mu20_iloose_L1MU15 || HLT_mu50 || HLT_mu60_0eta105_msonly)'
dimuonTrig     = '(HLT_2mu14 || HLT_2mu10 || HLT_mu24_mu8noL1 || HLT_mu22_mu8noL1 || HLT_mu20_mu8noL1 || HLT_mu18_mu8noL1)'
trimuonTrig    = '(HLT_mu24_2mu4noL1 || HLT_mu22_2mu4noL1 || HLT_mu20_2mu4noL1 || HLT_mu18_2mu4noL1 || HLT_3mu6 || HLT_3mu6_msonly)'

electronTrigCut = '(HLT_e26_tight_iloose || HLT_e60_medium || HLT_e24_tight_iloose || HLT_e24_medium_iloose_L1EM20VH || HLT_e24_medium_iloose_L1EM18VH)'
electronTrigLH  = '(HLT_e26_lhtight_iloose || HLT_e60_lhmedium || HLT_e24_lhtight_iloose || HLT_e24_lhmedium_iloose_L1EM20VH || HLT_e24_lhmedium_iloose_L1EM18VH)'

dielectronTrigCut = '(HLT_2e17_loose || HLT_2e15_loose_L12EM13VH || HLT_2e12_loose_L12EM10VH)'
dielectronTrigLH  = '(HLT_2e17_lhloose || HLT_2e15_lhloose_L12EM13VH || HLT_2e12_lhloose_L12EM10VH)'

trielectronTrigCut   = '(HLT_e17_medium_2e9_medium)'
trielectronTrigLH    = '(HLT_e17_lhmedium_2e9_lhmedium)'

elemuonTrigCut = '(HLT_e17_loose_mu14 || HLT_e7_medium_mu24 || HLT_e26_medium_L1EM22VHI_mu8noL1 || HLT_e24_medium_L1EM20VHI_mu8noL1)'
elemuonTrigLH  = '(HLT_e17_lhloose_mu14 || HLT_e7_lhmedium_mu24 || HLT_e26_lhmedium_L1EM22VHI_mu8noL1 || HLT_e24_lhmedium_L1EM20VHI_mu8noL1)'

trielemuonTrigCut = '(HLT_e12_loose_2mu10 || HLT_2e12_loose_mu10)'
trielemuonTrigLH  = '(HLT_e12_lhloose_2mu10 || HLT_2e12_lhloose_mu10)'

alltriggers = ' || '.join([
  muonTrig, dimuonTrig, trimuonTrig, 
  electronTrigCut, electronTrigLH, dielectronTrigCut, dielectronTrigLH, trielectronTrigCut, trielectronTrigLH, 
  elemuonTrigCut, elemuonTrigLH,
  trielemuonTrigCut, trielemuonTrigLH,
  xeTriggers])

expression = '(' + alltriggers + ') && '+objectSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY2SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY2SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY2KernelSkim",
  SkimmingTools = [SUSY2SkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY2")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY2)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY2GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY2GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY2 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY2KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY2SlimmingHelper = SlimmingHelper("SUSY2SlimmingHelper")
SUSY2SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","TauJets","MET_Reference_AntiKt4EMTopo","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY2SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth"]					
SUSY2SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
					"Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
					"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
					"GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"InDetTrackParticles.truthOrigin.truthType",
					"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
					"Electrons.bkgMotherPdgId.bkgTruthOrigin"]
SUSY2SlimmingHelper.IncludeMuonTriggerContent = True
SUSY2SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY2SlimmingHelper.IncludeBPhysTriggerContent = True 
#SUSY2SlimmingHelper.IncludeJetTauEtMissTriggerContent = True

if globalflags.DataSource()=='geant4':

  SUSY2SlimmingHelper.StaticContent += [ 
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
  ]

SUSY2SlimmingHelper.AppendContentToStream(SUSY2Stream)
