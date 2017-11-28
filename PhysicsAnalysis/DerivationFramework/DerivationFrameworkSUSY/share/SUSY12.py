#********************************************************************
# SUSY12.py
# reductionConf flag SUSY12 in Reco_tf.py  
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
 
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY12Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY12Stream )
SUSY12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY12Stream.AcceptAlgs(["SUSY12KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY12ThinningHelper = ThinningHelper( "SUSY12ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY12 = CfgMgr.AthSequencer("SeqSUSY12")
DerivationFrameworkJob += SeqSUSY12


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY12TriggerList import triggers_met, triggers_ele, triggers_muon

triggers = triggers_met + triggers_ele + triggers_muon

SUSY12ThinningHelper.TriggerChains = '(' + ' | '.join(triggers) + ')' #triggerRegEx
SUSY12ThinningHelper.AppendToStream( SUSY12Stream )


#====================================================================
# THINNING TOOL
#====================================================================\

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#SUSY12TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY12TPThinningTool",
#                                 ThinningService     = SUSY12ThinningHelper.ThinningSvc(),
#                                 SelectionString     = "InDetTrackParticles.pt > 10*GeV",  # TODO: Check with 5 geV
#                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY12TPThinningTool
#thinningTools.append(SUSY12TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY12MuonTPThinningTool",
                                                                ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
                                                                MuonKey                 = "Muons",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY12MuonTPThinningTool
thinningTools.append(SUSY12MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY12ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name     = "SUSY12ElectronTPThinningTool",
                                                                 ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
                                                                 SGKey                   = "Electrons",
                                                                 GSFTrackParticlesKey    = 'GSFTrackParticles',
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY12ElectronTPThinningTool
thinningTools.append(SUSY12ElectronTPThinningTool)

# TrackParticles associated with jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY12JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name             = "SUSY12JetTPThinningTool",
                                                                 ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
                                                                 JetKey                   = "AntiKt4EMTopoJets",
                                                                 InDetTrackParticlesKey  = 'InDetTrackParticles',
                                                                 SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV",
                                                                 )
ToolSvc += SUSY12JetTPThinningTool
thinningTools.append(SUSY12JetTPThinningTool)

# TrackParticles associated with photons
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
#SUSY12PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                        = "SUSY12PhotonTPThinningTool",
#                                                                                 ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
#                                                                                 SGKey                   = "Photons",
#                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY12PhotonTPThinningTool
#thinningTools.append(SUSY12PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY12TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY12TauTPThinningTool",
                                                                        ThinningService = SUSY12ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        #SelectionString         = "TauJets.pt > 10*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY12TauTPThinningTool
thinningTools.append(SUSY12TauTPThinningTool)

# Calo Clusters associated with Photons
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#SUSY12PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY12PhotonCCThinningTool",
#                                                                                     ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Photons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Photons.pt > 10*GeV",
#                                                                                    #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.6)
#ToolSvc += SUSY12PhotonCCThinningTool
#thinningTools.append(SUSY12PhotonCCThinningTool)

# Calo Clusters associated with Electrons
#SUSY12ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY12ElectronCCThinningTool",
#                                                                                     ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
#                                                                                    #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.4)
#ToolSvc += SUSY12ElectronCCThinningTool
#thinningTools.append(SUSY12ElectronCCThinningTool)

# Calo Clusters associated with jets
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
SUSY12aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY12aKt4CCThinningTool",
                                                                     ThinningService         = SUSY12ThinningHelper.ThinningSvc(),
                                                                      SGKey                   = "AntiKt4EMTopoJets",
                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                      SelectionString         = "AntiKt4EMTopoJets.pt > 0*GeV",
                                                                      AdditionalClustersKey   = ["EMOriginTopoClusters"])
ToolSvc += SUSY12aKt4CCThinningTool
thinningTools.append(SUSY12aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY12TruthThinningTool = DerivationFramework__MenuTruthThinning(
                                    name                         = "SUSY12TruthThinningTool",
                                    ThinningService              = SUSY12ThinningHelper.ThinningSvc(),
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
  ToolSvc += SUSY12TruthThinningTool
  thinningTools.append(SUSY12TruthThinningTool)

#====================================================================
# SKIMMING
#====================================================================
# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY12) # default: sequence=DerivationFrameworkJob
#updateJVT_xAODColl("AntiKt4EMTopo") # TODO: for next cache?


jet_prefab = 'count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > XPT*GeV && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < XETA))'
ele_prefab = 'count(Electrons.pt > XPT*GeV && abs(Electrons.eta) < XETA && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose)))'
muon_prefab = 'count(Muons.pt > XPT*GeV && abs(Muons.eta) < XETA && (Muons.DFCommonMuonsPreselection))'

count_jet_hard = jet_prefab.replace("XPT", "150.").replace("XETA", "2.8")
count_jet_soft = jet_prefab.replace("XPT", "80.").replace("XETA", "2.8")
count_ele_hard = ele_prefab.replace("XPT", "150.").replace("XETA", "2.6")
count_ele_soft = ele_prefab.replace("XPT", "20.").replace("XETA", "2.6")
count_muon_soft = muon_prefab.replace("XPT", "20.").replace("XETA", "2.8")
#count_ele_verysoft = ele_prefab.replace("XPT", "8.").replace("XETA", "2.6")
#count_muon_verysoft = muon_prefab.replace("XPT", "8.").replace("XETA", "2.6")

expression_trigger_met = "(" + " || ".join(triggers_met) + ")"
expression_jetele = "(" +count_jet_hard + " + " + count_ele_hard + " >= 1)" # && " + count_jet_soft + " + " + count_ele_soft + " >= 2
expression_jeteletrig = "(" + expression_trigger_met + " && " + expression_jetele + ")"

expression_trigger_lep = "(" + " || ".join(triggers_met+triggers_ele+triggers_muon) + ")"
expression_lep = "(" + count_ele_soft + " + " + count_muon_soft + " >= 2 && " + count_jet_soft + " >= 1)" # && " + count_ele_soft + " + " + count_muon_soft + " >= 2)"
expression_leptrig = "(" + expression_trigger_lep + " && " + expression_lep + ")"

expression = expression_jeteletrig + " || " + expression_leptrig

print "expression = '" + expression + "'"
print

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY12SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY12SkimmingTool", expression = expression)
ToolSvc += SUSY12SkimmingTool



#====================================================================
# ISOLATION TOOL
#====================================================================
#Track selection
#from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
#TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
#TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
#TrackIsoTool.TrackSelectionTool.minPt= 1000.
#TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
#ToolSvc += TrackIsoTool

#TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
#TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
#TrackIsoTool500.TrackSelectionTool.minPt= 500.
#TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
#ToolSvc += TrackIsoTool500

#from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
#import ROOT, PyCintex
#PyCintex.loadDictionary('xAODCoreRflxDict')
#PyCintex.loadDictionary('xAODPrimitivesDict')
#isoPar = ROOT.xAOD.Iso
#Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
#                                                                TrackIsolationTool = TrackIsoTool,
#                                                                TargetContainer = "InDetTrackParticles",
#                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
#                                                                Prefix = 'TrkIsoPt1000_'
#                                                               )
#Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
#                                                                TrackIsolationTool = TrackIsoTool500,
#                                                                TargetContainer = "InDetTrackParticles",
#                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
#                                                                Prefix = 'TrkIsoPt500_'
#                                                               )
#ToolSvc += Pt1000IsoTrackDecorator
#ToolSvc += Pt500IsoTrackDecorator
#
#AugmentationTools.append(Pt1000IsoTrackDecorator)
#AugmentationTools.append(Pt500IsoTrackDecorator)


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
   SeqSUSY12 += CfgMgr.DerivationFramework__DerivationKernel("SUSY12KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY12")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY12 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY12KernelSkim",
        SkimmingTools = [SUSY12SkimmingTool],
)


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY12"] = []
reducedJetList = [ "AntiKt4PV0TrackJets" ]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY12, "SUSY12")


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
SeqSUSY12 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY12KernelAug",
        AugmentationTools = AugmentationTools,
        ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# About SmartCollections and AllVariables:
#   If you want to use CP tools on a container, you MUST include that container as a SmartCollection.
#   You are then free to include it as AllVariables in addition, to pick up variables not needed by the CP tools but needed by your analysis.
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY12SlimmingHelper = SlimmingHelper("SUSY12SlimmingHelper")

SUSY12SlimmingHelper.SmartCollections = [
                                         "AntiKt4EMTopoJets", 
                                         #"AntiKt4EMPFlowJets",
                                         #"AntiKt4LCTopoJets",
                                         "Electrons",
                                         "Muons",
                                         "TauJets",
                                         "PrimaryVertices", 
                                         "MET_Reference_AntiKt4EMTopo", 
                                         "BTagging_AntiKt4EMTopo",
                                         "InDetTrackParticles"
                                         ] #,  "BTagging_AntiKt4Track"

SUSY12SlimmingHelper.AllVariables = [
                                     #"AntiKt4PV0TrackJets",
                                     #"AntiKt4TruthJets",
                                     #"MuonSegments",
                                     #"MET_Track",
                                     #"MET_LocHadTopo",
                                     #"TopoClusterIsoCentralEventShape",
                                     #"CaloCalTopoClusters"
                                     ] 

if DerivationFrameworkIsMonteCarlo:
    
    SUSY12SlimmingHelper.AllVariables += [
                                     "MET_Truth",
                                     "TruthEvents", 
                                     "TruthVertices"
                                     ]

SUSY12SlimmingHelper.ExtraVariables = [
                                       "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelID.Width.HadronConeExclTruthLabelID.PartonTruthLabelID.N90Constituents.Charge",
                                       "AntiKt4PV0TrackJets.eta.m.phi.pt.Width",
                                       "CaloCalTopoClusters.calEta.calPhi.calE.calM",
                                       "TruthParticles.px.py.pz.m.e.status.pdgId.charge.barcode.prodVtxLink.decayVtxLink",
                                       "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                       "BTagging_AntiKt4EMTopo.MV2cl100_discriminant",
                                       #"TruthTaus.px.py.pz.m.e.status.pdgId.charge.barcode.prodVtxLink.decayVtxLink",
                                       #"TruthVertices.id.barcode.incomingParticleLinks.outgoingParticleLinks.x.y.z.t",
                                       #"BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                       #'Electrons.author.Medium.Tight.Loose.charge',
                                       #'Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40',
                                       ##"Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                       #"Photons.author.Loose.Tight",
                                       #"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi", # TODO: .DFCommonJets_Jvt",
                                       ##"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       'ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
                                       'CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
                                       #"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       #"TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                       #"GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                                       "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType"
                                       #"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      ]


SUSY12SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY12SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY12SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY12SlimmingHelper.IncludeJetTriggerContent    = True
SUSY12SlimmingHelper.IncludeTauTriggerContent    = False
SUSY12SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY12SlimmingHelper.IncludeBJetTriggerContent   = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY12SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  SUSY12SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthTop", "TruthBSM", "TruthBoson"]   


#addJetOutputs(SUSY12SlimmingHelper, ["SmallR", "SUSY12"], ["AntiKt4EMTopoJets"], ["AntiKt4LCTopoJets", "AntiKt4TruthJets"]) #"AntiKt3PV0TrackJets", "AntiKt2PV0TrackJets", "AntiKt4EMPFlowJets", 
#addJetOutputs(SUSY12SlimmingHelper, [], ["AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets","AntiKt4PV0TrackJets","AntiKt3PV0TrackJets", "AntiKt2PV0TrackJets", "AntiKt4EMPFlowJets"]) #,

SUSY12SlimmingHelper.AppendContentToStream(SUSY12Stream)
