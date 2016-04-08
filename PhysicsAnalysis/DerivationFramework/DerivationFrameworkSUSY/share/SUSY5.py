#********************************************************************
# SUSY5.py 
# reductionConf flag SUSY5 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY5Stream )
SUSY5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY5Stream.AcceptAlgs(["SUSY5KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY5ThinningHelper = ThinningHelper( "SUSY5ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY5 = CfgMgr.AthSequencer("SeqSUSY5")
DerivationFrameworkJob += SeqSUSY5


#====================================================================
# Trigger navigation thinning
#====================================================================
triggerRegEx = [
  "e.*",
  "mu.*",
  "tau.*"
]
SUSY5ThinningHelper.TriggerChains = 'HLT_(' + ' | '.join(triggerRegEx) + ')'
SUSY5ThinningHelper.AppendToStream( SUSY5Stream )

     
#====================================================================
# THINNING TOOLS 
#====================================================================

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY5MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY5MetTPThinningTool",
                                                                ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY5MetTPThinningTool
thinningTools.append(SUSY5MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "SUSY5JetTPThinningTool",
                                                                ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY5JetTPThinningTool
thinningTools.append(SUSY5JetTPThinningTool)

# TrackParticles directly
SUSY5TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY5TPThinningTool",
                                                                 ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5TPThinningTool
thinningTools.append(SUSY5TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY5MuonTPThinningTool",
                                                                         ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5MuonTPThinningTool
thinningTools.append(SUSY5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY5ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY5ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY5ElectronTPThinningTool
thinningTools.append(SUSY5ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY5PhotonTPThinningTool",
										 ThinningService	 = SUSY5ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5PhotonTPThinningTool
thinningTools.append(SUSY5PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY5TauTPThinningTool",
                                                                        ThinningService = SUSY5ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5TauTPThinningTool
thinningTools.append(SUSY5TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY5TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY5TruthThinningTool",
                                                       ThinningService              = SUSY5ThinningHelper.ThinningSvc(),
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

AugmentationTools = []

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':

        ###############
        #TRUTH3 content
        ###############

	from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__Truth3CollectionMaker
	
	TRUTH3MuonTool = DerivationFramework__Truth3CollectionMaker(name                   = "TRUTH3MuonTool",
								    NewCollectionName       = "SUSYTRUTHMuons",
								    ParticleSelectionString = "abs(TruthParticles.pdgId) == 13 && TruthParticles.status==1")
	ToolSvc += TRUTH3MuonTool
	AugmentationTools.append(TRUTH3MuonTool)
	
	TRUTH3ElectronTool = DerivationFramework__Truth3CollectionMaker(name               = "TRUTH3ElectronTool",
									NewCollectionName       = "SUSYTRUTHElectrons",
									ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 && TruthParticles.status==1")
	ToolSvc += TRUTH3ElectronTool
	AugmentationTools.append(TRUTH3ElectronTool)
	
	TRUTH3PhotonTool = DerivationFramework__Truth3CollectionMaker(name                 = "TRUTH3PhotonTool",
								      NewCollectionName       = "SUSYTRUTHPhotons",
								      ParticleSelectionString = "abs(TruthParticles.pdgId) == 22 && TruthParticles.pt > 50.*GeV")
	ToolSvc += TRUTH3PhotonTool
	AugmentationTools.append(TRUTH3PhotonTool)
	
	TRUTH3TauTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3TauTool",
								   NewCollectionName       = "SUSYTRUTHTaus",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 15")
	ToolSvc += TRUTH3TauTool
	AugmentationTools.append(TRUTH3TauTool)

        ##############
        ##############
        ##############

from RecExConfig.RecFlags  import rec
if  rec.doTruth():
	
	# Decorate Electron with bkg electron type/origin 
	from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
	from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
	BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
	ToolSvc += BkgElectronClassificationTool
	AugmentationTools.append(BkgElectronClassificationTool)

if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY5TruthThinningTool
	thinningTools.append(SUSY5TruthThinningTool)
	
#====================================================================
# SKIMMING TOOL 
#====================================================================

#=======
# Triggers for skimming
#=======

xeTrig = [
  'HLT_xe100', 'HLT_xe100_tc_lcw', 'HLT_xe100_tc_em', 'HLT_xe100_mht', 'HLT_xe100_pueta', 
  'HLT_xe100_pufit', 'HLT_xe100_wEFMu', 'HLT_xe100_tc_lcw_wEFMu', 'HLT_xe100_tc_em_wEFMu', 'HLT_xe100_mht_wEFMu', 'HLT_xe100_pueta_wEFMu', 'HLT_xe100_pufit_wEFMu',
  'HLT_xe70', 'HLT_xe70_tc_lcw', 'HLT_xe70_tc_em', 'HLT_xe70_mht','HLT_xe70_pueta',
  'HLT_xe70_pufit', 'HLT_xe70_wEFMu', 'HLT_xe70_tc_lcw_wEFMu', 'HLT_xe70_tc_em_wEFMu', 'HLT_xe70_mht_wEFMu', 'HLT_xe70_pueta_wEFMu', 'HLT_xe70_pufit_wEFMu',
  'HLT_xe80', 'HLT_xe80_tc_lcw', 'HLT_xe80_tc_em', 'HLT_xe80_mht','HLT_xe80_pueta',
  'HLT_xe80_pufit', 'HLT_xe80_wEFMu', 'HLT_xe80_tc_lcw_wEFMu', 'HLT_xe80_tc_em_wEFMu', 'HLT_xe80_mht_wEFMu', 'HLT_xe80_pueta_wEFMu', 'HLT_xe80_pufit_wEFMu'
]

leptonTrig = [
  'HLT_e24_medium_iloose_L1EM18VH',
  'HLT_e24_medium_iloose_L1EM20VH',
  'HLT_e24_tight_iloose_L1EM20VH',
  'HLT_e24_tight_iloose',
  'HLT_e26_tight_iloose',
  'HLT_e60_medium',
  'HLT_e24_lhmedium_iloose_L1EM18VH',
  'HLT_e24_lhmedium_iloose_L1EM20VH',
  'HLT_e24_lhtight_iloose_L1EM20VH',
  'HLT_e24_lhtight_iloose',
  'HLT_e26_lhtight_iloose',
  'HLT_e26_vloose_L1EM20VH',
  'HLT_e26_lhvloose_L1EM20VH',
  'HLT_e60_lhmedium',
              

  'HLT_mu20_iloose_L1MU15',
  'HLT_mu20_L1MU15',
  'HLT_mu24',
  'HLT_mu24_L1MU15',
  'HLT_mu24_iloose_L1MU15',
  'HLT_mu24_imedium',
  'HLT_mu26',
  'HLT_mu26_imedium',
  'HLT_mu40',
  'HLT_mu50',
  'HLT_mu60_0eta105_msonly'
]

vbfTrig ='(HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49 || HLT_mu6_2j40_0eta490_invm400)'

muonsRequirementsSL = '(Muons.pt >= 6.*GeV) && (Muons.pt <= 25.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirementsSL = '(Electrons.pt > 7.*GeV) && (Electrons.pt <= 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionSL = '(count('+electronsRequirementsSL+') + count('+muonsRequirementsSL+') >= 1)'

muonsRequirementsHL = '(Muons.pt >= 25.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirementsHL = '(Electrons.pt > 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionHL = '(count('+electronsRequirementsHL+') + count('+muonsRequirementsHL+') >= 1)'

muonsRequirements = '(Muons.pt >= 6.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 7.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1)'

xeTrignew = '(HLT_xe35 || HLT_xe35_mht || HLT_xe35_pufit || HLT_xe35_pueta || HLT_xe35_wEFMu || HLT_xe35_mht_wEFMu || HLT_xe35_pueta_wEFMu || HLT_xe35_pufit_wEFMu)'

#For now, we are rather inclusive on the data and have a loose MET trigger for the soft leptons in the MC.
#expression = '(' + ' || '.join(xeTrig + leptonTrig) + ')' 
#expression += ' && ' + objectSelection
#expression += ' && (count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 35.*GeV) > 1) && (count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 70.*GeV) > 0)'
expression = objectSelection

if globalflags.DataSource()=='geant4':
	expression = '(('+ xeTrignew + ' || ' + vbfTrig + ') && ' + objectSelectionSL + ') || ' + objectSelectionHL 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY5SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY5)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY5KernelSkim",
  SkimmingTools = [SUSY5SkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY5")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY5)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY5GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY5GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY5KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

#=======================================
# add jet building, cf. example in JETM1
#=======================================

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(SeqSUSY5, "SUSY5")


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY5SlimmingHelper = SlimmingHelper("SUSY5SlimmingHelper")
SUSY5SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","TauJets","AntiKt4EMTopoJets","MET_Reference_AntiKt4EMTopo","BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY5SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth"]					
SUSY5SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                "Photons.author.Loose.Tight",
                "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                "TauJets.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
                "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
				        "Electrons.bkgTruthType.bkgTruthOrigin",
                "MET_Track",
                "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                "BTagging_AntiKt3Track.MV2c20_discriminant"]

SUSY5SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY5SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY5SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY5SlimmingHelper.IncludeJetTriggerContent    = True
SUSY5SlimmingHelper.IncludeTauTriggerContent    = True
SUSY5SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY5SlimmingHelper.IncludeBJetTriggerContent   = True


#Trigger navigation is way too big for now.
if globalflags.DataSource()=='geant4':
  SUSY5SlimmingHelper.IncludeMuonTriggerContent   = True
  SUSY5SlimmingHelper.IncludeEGammaTriggerContent = True
  SUSY5SlimmingHelper.IncludeBPhysTriggerContent  = False
  SUSY5SlimmingHelper.IncludeJetTriggerContent    = False
  SUSY5SlimmingHelper.IncludeTauTriggerContent    = True
  SUSY5SlimmingHelper.IncludeEtMissTriggerContent = True
  SUSY5SlimmingHelper.IncludeBJetTriggerContent   = False
  

addJetOutputs(SUSY5SlimmingHelper,["LargeR", "SUSY5"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets"])
#addJetOutputs(slimhelper,contentlist,t0list_smart=[],vetolist=[]):
# Add the MET containers to the stream
# JRC: TEMPORARILY COMMENTED
#addMETOutputs(SUSY5Stream,["SUSY"])

if globalflags.DataSource()=='geant4':

  SUSY5Stream.AddItem( "xAOD::TruthParticleContainer#SUSYTRUTHMuons*")
  SUSY5Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSYTRUTHMuons*")
  SUSY5Stream.AddItem( "xAOD::TruthParticleContainer#SUSYTRUTHElectrons*")
  SUSY5Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSYTRUTHElectrons*")
  #SUSY5Stream.AddItem( "xAOD::TruthParticleContainer#SUSYTRUTHTaus*")
  #SUSY5Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSYTRUTHTaus*")

  SUSY5SlimmingHelper.StaticContent += [ 
    "xAOD::TruthParticleContainer#TruthElectrons",
    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
    "xAOD::TruthParticleContainer#TruthMuons",
    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
  ]
  

SUSY5SlimmingHelper.AppendContentToStream(SUSY5Stream)
