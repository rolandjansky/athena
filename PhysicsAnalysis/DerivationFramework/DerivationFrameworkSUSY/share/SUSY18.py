#********************************************************************
# SUSY18.py
# reductionConf flag SUSY18 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY18Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY18Stream )
SUSY18Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY18Stream.AcceptAlgs(["SUSY18KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY18ThinningHelper = ThinningHelper( "SUSY18ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY18 = CfgMgr.AthSequencer("SeqSUSY18")
DerivationFrameworkJob += SeqSUSY18


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY18TriggerList import SUSY18Triggers
trig = SUSY18Triggers()
SUSY18ThinningHelper.TriggerChains = trig.AllTrig_BitwiseOR

# needs to go after SUSY18ThinningHelper.TriggerChains = ... but before accesses to SUSY18ThinningHelper.ThinningSvc()
SUSY18ThinningHelper.AppendToStream( SUSY18Stream ) 

#====================================================================
# THINNING TOOLS
#====================================================================

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
#SUSY18TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY18TPThinningTool",
#                                                                  ThinningService         = SUSY18ThinningHelper.ThinningSvc(),
#                                                                  SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY18TPThinningTool
#thinningTools.append(SUSY18TPThinningTool)

# TrackParticles associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY18JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "SUSY18JetTPThinningTool",
                                                                          ThinningService         = SUSY18ThinningHelper.ThinningSvc(),
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += SUSY18JetEMTPThinningTool
thinningTools.append(SUSY18JetEMTPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY18MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY18MuonTPThinningTool",
                                                                          ThinningService         = SUSY18ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY18MuonTPThinningTool
thinningTools.append(SUSY18MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY18ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "SUSY18ElectronTPThinningTool",
                                                                                ThinningService         = SUSY18ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY18ElectronTPThinningTool
thinningTools.append(SUSY18ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY18PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name		     = "SUSY18PhotonTPThinningTool",
                                                                              ThinningService        = SUSY18ThinningHelper.ThinningSvc(),
                                                                              SGKey		     = "Photons",
                                                                              InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY18PhotonTPThinningTool
thinningTools.append(SUSY18PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY18TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                   = "SUSY18TauTPThinningTool",
                                                                         ThinningService        = SUSY18ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         SelectionString        = "TauJets.ptFinalCalib >= 18.0*GeV",
                                                                         ConeSize               = 0.4,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY18TauTPThinningTool
thinningTools.append(SUSY18TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

    #thin special containers
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning

    #truth tau thinning - Truth tau/nutau and their ancestors and descendants
    truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
    truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

    #thin taus
    SUSY18TruthTool_TAU = DerivationFramework__GenericTruthThinning(name                         = "SUSY18TruthTool_TAU",
                                                                    ThinningService              = SUSY18ThinningHelper.ThinningSvc(),
                                                                    ParticleSelectionString      = truth_cond_tau,
                                                                    PreserveDescendants          = False,
                                                                    PreserveGeneratorDescendants = True,
                                                                    PreserveAncestors            = False,
                                                                    TauHandling                  = False)
    ToolSvc += SUSY18TruthTool_TAU
    thinningTools.append(SUSY18TruthTool_TAU)

    #thin leptons and taus
    SUSY18TruthTool_COMB = DerivationFramework__GenericTruthThinning(name                         = "SUSY18TruthTool_COMB",
                                                                     ThinningService              = SUSY18ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString      = truth_cond_comb,
                                                                     PreserveDescendants          = False,
                                                                     PreserveGeneratorDescendants = False,
                                                                     PreserveAncestors            = True,
                                                                     TauHandling                  = False)
    
    ToolSvc += SUSY18TruthTool_COMB
    thinningTools.append(SUSY18TruthTool_COMB)

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY18TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY18TruthThinningTool",
                                                                     ThinningService              = SUSY18ThinningHelper.ThinningSvc(),
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
                                                                     WriteStatus3                 = True,
                                                                     WriteFirstN                  = -1,
                                                                     PreserveAncestors            = True,
                                                                     PreserveGeneratorDescendants = False,
                                                                     SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)


    # Decorate Electron with bkg electron type/origin
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name                  = "BkgElectronClassificationTool",
                                                                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BkgElectronClassificationTool
    AugmentationTools.append(BkgElectronClassificationTool)

    ToolSvc += SUSY18TruthThinningTool
    thinningTools.append(SUSY18TruthThinningTool)

#====================================================================
# SKIMMING TOOL
#====================================================================

#muon quality
muonQual = "Muons.DFCommonMuonsPreselection"

#electron quality
eleQual  = "(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose)"

#muon eta
muonEta = "abs(Muons.eta) < 2.5"

#ele eta
eleEta = 'abs(Electrons.eta) < 2.7'

#leptons
e15p3  = '(count( Electrons.pt > 15.3*GeV && '+eleEta+' && '+eleQual+') >= 1)'
e21p6  = '(count( Electrons.pt > 21.6*GeV && '+eleEta+' && '+eleQual+') >= 1)'
e23p4  = '(count( Electrons.pt > 23.4*GeV && '+eleEta+' && '+eleQual+') >= 1)'
mu12p6 = '(count( Muons.pt > 12.6*GeV && '+muonEta+' && '+muonQual+' ) >= 1)'
mu18p0 = '(count( Muons.pt > 18.0*GeV && '+muonEta+' && '+muonQual+' ) >= 1)'
mu21p6 = '(count( Muons.pt > 21.6*GeV && '+muonEta+' && '+muonQual+' ) >= 1)'
mu23p4 = '(count( Muons.pt > 23.4*GeV && '+muonEta+' && '+muonQual+' ) >= 1)'

#tau prongs and tracks
tauProngs = "abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3)"

# taus 10% * pT corresponds to a 2sigma deviation from the nominal
tau18p0  = '(count( TauJets.ptFinalCalib >= 18.0*GeV && '+tauProngs+' ) >= 1)'
tau22p5  = '(count( TauJets.ptFinalCalib >= 22.5*GeV && '+tauProngs+' ) >= 1)'

#muon triggers: 
# pT offline  = pT online x 1.05
# conservative pT offline derivations = pT online x 0.9
MuTau_SMT_2015        = mu18p0+' && '+tau18p0+' && ('+trig.SMT_OR_2015+')'
MuTau_SMT_2016_A      = mu21p6+' && '+tau18p0+' && ('+trig.SMT_OR_2016_A+')'
MuTau_SMT_2016_B_D3   = mu21p6+' && '+tau18p0+' && ('+trig.SMT_OR_2016_B_D3+')'
MuTau_SMT_2016_D4_End = mu23p4+' && '+tau18p0+' && ('+trig.SMT_OR_2016_D4_End+')'
MuTau_SMT_2017	      = mu23p4+' && '+tau18p0+' && ('+trig.SMT_OR_2017+')'

#Ele triggers
# conservative pT offline derivations = pT online x 0.9
ElTau_SET_2015 = e21p6+' && '+tau18p0+' && ('+trig.SET_OR_2015+')'
ElTau_SET_2016 = e23p4+' && '+tau22p5+' && ('+trig.SET_OR_2016+')'
ElTau_SET_2017 = e23p4+' && '+tau22p5+' && ('+trig.SET_OR_2017+')'

# tau + lepton triggers
MuTau_MTT = mu12p6+' && '+tau22p5+' && ('+trig.MTT_OR+')'
ElTau_ETT = e15p3+' && '+tau22p5+' && ('+trig.ETT_OR+')'

# tau + lepton + met triggers
MuTau_MTMT = mu12p6+' && '+tau22p5+' && ('+trig.MTMT_OR+')'
ElTau_ETMT = e15p3+' && '+tau22p5+' && ('+trig.ETMT_OR+')'

#final skim: object+trigger selection
ORdelim = '||'
skim_expression = \
'('+MuTau_SMT_2015+')' + ORdelim + \
'('+MuTau_SMT_2016_A+')' + ORdelim + \
'('+MuTau_SMT_2016_B_D3+')' + ORdelim + \
'('+MuTau_SMT_2016_D4_End+')' + ORdelim + \
'('+MuTau_SMT_2017+')' + ORdelim + \
'('+ElTau_SET_2015+')' + ORdelim + \
'('+ElTau_SET_2016+')' + ORdelim + \
'('+ElTau_SET_2017+')' + ORdelim + \
'('+MuTau_MTT+')' + ORdelim + \
'('+ElTau_ETT+')' + ORdelim + \
'('+MuTau_MTMT+')' + ORdelim + \
'('+ElTau_ETMT+')'

print "SUSY18 skimming expression", skim_expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY18SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY18SkimmingTool",
                                                                  expression = skim_expression)

ToolSvc += SUSY18SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():

   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY18 += CfgMgr.DerivationFramework__DerivationKernel("SUSY18KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY18")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY18 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY18KernelSkim",
  SkimmingTools = [SUSY18SkimmingTool]#, SUSY18TriggerSkimmingTool]
  )


#==============================================================================
# Jet building
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY18"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY18, "SUSY18")


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)
  ToolSvc.DFCommonTauTruthMatchingTool.WriteInvisibleFourMomentum = True

#==============================================================================
# Augment after skim
#==============================================================================

SeqSUSY18 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY18KernelAug",
    AugmentationTools = AugmentationTools,
    ThinningTools = thinningTools,
    )


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY18SlimmingHelper = SlimmingHelper("SUSY18SlimmingHelper")

# smart variables
SUSY18SlimmingHelper.SmartCollections = ["Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "AntiKt4EMTopoJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "BTagging_AntiKt4EMTopo",
                                         "InDetTrackParticles",
                                         "PrimaryVertices",
                                         "AntiKt4TruthJets",
                                         "AntiKt4TruthWZJets"]

#all variables
SUSY18SlimmingHelper.AllVariables = ["TruthParticles",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "MET_Truth",
                                     #"AntiKt4TruthJets",
                                     #"AntiKt4TruthWZJets",
                                     "LVL1JetRoIs",
                                     "MET_Track" #needed for the forward JVT
                                     ]

#extra truth content
ExtraElectronsTruth = [
    "Electrons."
    "truthOrigin."
    "truthType."
    "truthParticleLink."
    "bkgMotherPdgId."
    "bkgTruthOrigin."
    "bkgTruthType."
    "firstEgMotherTruthType."
    "firstEgMotherTruthOrigin."
    "firstEgMotherPdgId"
    ]

ExtraMuonsTruth = [
    "MuonTruthParticles."
    "barcode."
    "decayVtxLink."
    "e."
    "m."
    "pdgId."
    "prodVtxLink."
    "px."
    "py."
    "pz."
    "recoMuonLink."
    "status."
    "truthOrigin."
    "truthType",
    #
    "CombinedMuonTrackParticles."
    "truthOrigin."
    "truthType",
    #
    "ExtrapolatedMuonTrackParticles."
    "truthOrigin."
    "truthType",
    #
    "GSFTrackParticles."
    "truthOrigin."
    "truthType"
    ]

ExtraTausTruth = [
    "TauJets."
    "IsTruthMatched."
    "truthParticleLink."
    "truthJetLink."
    "truthOrigin."
    "truthType"
    ]

ExtraJetsTruth = [
    "AntiKt4TruthJets."
    "eta."
    "m."
    "phi."
    "pt."
    "TruthLabelDeltaR_B."
    "TruthLabelDeltaR_C."
    "TruthLabelDeltaR_T."
    "TruthLabelID."
    "ConeTruthLabelID."
    "PartonTruthLabelID."
    "HadronConeExclTruthLabelID"
]

#extra reco content
ExtraVtx = ["PrimaryVertices."
                   "x."
                   "y."
                   "z."
                   "vertexType"
                 ]

ExtraBtag = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant"]

ExtraElectrons = ["Electrons.author.charge.ptcone20"]

ExtraMuons = ["Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag"]

ExtraPhotons = ["Photons.author.Loose.Tight"]

ExtraJets = [ "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.N90Constituents.Timing.Width"]

ExtraMuonTrks = [ "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
              "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix",
              "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix"]

ExtraHLT = [ "HLT_xAOD__JetContainer_SplitJet.pt.eta.phi.m",
             "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant",
             "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht.ex.ey"]

ExtraTracks= ["InDetTrackParticles.truthOrigin.truthType"]

SUSY18SlimmingHelper.ExtraVariables = ExtraBtag + ExtraElectrons + ExtraMuons + ExtraPhotons + ExtraJets + ExtraMuonTrks + ExtraHLT + ExtraTracks

if DerivationFrameworkIsMonteCarlo:
    SUSY18SlimmingHelper.ExtraVariables += ExtraElectronsTruth + ExtraMuonsTruth + ExtraTausTruth + ExtraJetsTruth


#trigger content
SUSY18SlimmingHelper.IncludeMuonTriggerContent = True
SUSY18SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY18SlimmingHelper.IncludeTauTriggerContent = True

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY18SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  SUSY18SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY18SlimmingHelper.AppendContentToStream(SUSY18Stream)
