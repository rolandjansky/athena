#====================================================================
# JETM4.py
# reductionConf flag JETM4 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#====================================================================
# SKIMMING TOOL
#====================================================================
# NOTE: need to add isSimulation as OR with trigger

from DerivationFrameworkJetEtMiss import TriggerLists
triggerlist = TriggerLists.single_photon_Trig()

triggers = '||'.join(triggerlist)
expression = '( (EventInfo.eventTypeBitmask==1) || ('+triggers+') )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM4SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM4SkimmingTool

#Trigger matching decorations
from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM4",
                                                 PhotonTriggers=TriggerLists.single_photon_Trig())

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM4Stream )
JETM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM4Stream.AcceptAlgs(["JETM4Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM4ThinningHelper = ThinningHelper( "JETM4ThinningHelper" )
# JETM4ThinningHelper.TriggerChains = triggers
JETM4ThinningHelper.AppendToStream( JETM4Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM4MuonTPThinningTool",
                                                                    ThinningService         = JETM4ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4MuonTPThinningTool
thinningTools.append(JETM4MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM4ElectronTPThinningTool",
                                                                               ThinningService         = JETM4ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4ElectronTPThinningTool
thinningTools.append(JETM4ElectronTPThinningTool)

# TrackParticles associated with electrons
JETM4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM4PhotonTPThinningTool",
                                                                             ThinningService         = JETM4ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4PhotonTPThinningTool
thinningTools.append(JETM4PhotonTPThinningTool)

thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM4TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM4TPThinningTool",
                                                                  ThinningService         = "JETM4ThinningSvc",
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4TPThinningTool
thinningTools.append(JETM4TPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM4TauTPThinningTool",
                                                                        ThinningService = JETM4ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4TauTPThinningTool
thinningTools.append(JETM4TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM4TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM4TruthThinningTool",
                                                                        ThinningService        = JETM4ThinningHelper.ThinningSvc(),
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)

    ToolSvc += JETM4TruthThinningTool
    thinningTools.append(JETM4TruthThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm4Seq = CfgMgr.AthSequencer("JETM4Sequence")
DerivationFrameworkJob += jetm4Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm4Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM4Kernel" ,
                                                         SkimmingTools = [JETM4SkimmingTool],
                                                         ThinningTools = thinningTools,
                                                         AugmentationTools = [TrigMatchAug])
#====================================================================
# Special jets
#====================================================================

OutputJets["JETM4"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TruthJets"]
replaceAODReducedJets(reducedJetList,jetm4Seq,"JETM4")

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm4Seq,"JETM4")
# Ungroomed UFO jets
addDefaultUFOJets(jetm4Seq,"JETM4",doCHS=True)
# UFO Trimmed jets
addTrimmedJets("AntiKt", 1.0, "UFOCSSK", rclus=0.2, ptfrac=0.05, algseq=jetm4Seq, outputGroup="JETM4", writeUngroomed=False, mods="tcc_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCHS", rclus=0.2, ptfrac=0.05, algseq=jetm4Seq, outputGroup="JETM4", writeUngroomed=False, mods="tcc_groomed")
# CSSK UFO SoftDrop jets
addSoftDropJets("AntiKt", 1.0, "UFOCSSK", beta=1.0, zcut=0.1, algseq=jetm4Seq, outputGroup="JETM4", writeUngroomed=False, mods="tcc_groomed")
addRecursiveSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, N=-1,  mods="tcc_groomed", algseq=jetm4Seq, outputGroup="JETM4", writeUngroomed=False)
addBottomUpSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, mods="tcc_groomed", algseq=jetm4Seq, outputGroup="JETM4", writeUngroomed=False)

#====================================================================
# ADD PFLOW AUG INFORMATION 
#====================================================================
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation
applyPFOAugmentation(DerivationFrameworkJob)

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

if DerivationFrameworkIsMonteCarlo:
    addMETTruthMap('AntiKt4EMTopo',"JETMX")
    addMETTruthMap('AntiKt4LCTopo',"JETMX")
    addMETTruthMap('AntiKt4EMPFlow',"JETMX")
    scheduleMETAssocAlg(jetm4Seq,"JETMX")
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm4Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4LCTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm4Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=jetm4Seq, algname="JetPtAssociationAlg")


#===================================================
#add variable-R track jets for b-tagging
#===================================================

largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
    "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
    "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
    "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
    ]

from DerivationFrameworkFlavourTag.HbbCommon import addVRJets
addVRJets(jetm4Seq,largeRColls=largeRJetCollections)
addVRJets(jetm4Seq,largeRColls=largeRJetCollections, training='201903')

from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#=======================================
# Re-tag PFlow jets so they have b-tagging info
#=======================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = jetm4Seq)

#===============================
# add xbb taggers
#===============================
from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(jetm4Seq, ToolSvc)

# Add the BCID info
addDistanceInTrain(jetm4Seq)

# Alternative rho definition
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
addCHSPFlowObjects()
from DerivationFrameworkJetEtMiss.JetCommon import defineEDAlg
jetm4Seq += defineEDAlg(R=0.4, inputtype="EMPFlowPUSB")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM4SlimmingHelper = SlimmingHelper("JETM4SlimmingHelper")
JETM4SlimmingHelper.AppendToDictionary = {
    "Kt4EMPFlowPUSBEventShape": "xAOD::EventShape"    ,
    "Kt4EMPFlowPUSBEventShapeAux": "xAOD::AuxInfoBase"    ,
}


JETM4SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt10TruthJets",
                                        "AntiKt10LCTopoJets",
                                        "AntiKt10UFOCSSKJets",
                                        "AntiKt10UFOCHSJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                        "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"
                                        ]

JETM4SlimmingHelper.AllVariables = [# "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape","Kt4EMPFlowPUSBEventShape"]

JETM4SlimmingHelper.ExtraVariables = ["CaloCalTopoClusters.calE.calEta.calPhi.calM.rawE.rawEta.rawPhi.rawM","Photons."+NewTrigVars["Photons"],"JetETMissNeutralParticleFlowObjects.m.mEM.eflowRec_TIMING.eflowRec_AVG_LAR_Q.eflowRec_CENTER_LAMBDA.pt.ptEM.phi.eta",
"JetETMissChargedParticleFlowObjects.pt.eta.phi.m.eflowRec_tracksExpectedEnergyDeposit.charge.eflowRec_isInDenseEnvironment.pfo_TrackLinks.DFCommonPFlow_z0.DFCommonPFlow_vz.DFCommonPFlow_d0.DFCommonPFlow_theta.DFCommonPFlow_envWeight",
"TauJets.truthJetLink.truthParticleLink.IsTruthMatched"]

# XbbScore variables
from DerivationFrameworkFlavourTag.HbbCommon import xbbTaggerExtraVariables
JETM4SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM4SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM4SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM4SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(
    slimhelper = JETM4SlimmingHelper,
    contentlist = [
      "SmallR",
      "JETM4"
      ],
    smartlist = JETM4SlimmingHelper.SmartCollections,
    )

# Add the MET containers to the stream
addMETOutputs(JETM4SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track",])

JETM4SlimmingHelper.AppendContentToStream(JETM4Stream)
