#====================================================================
# JETM1.py
# reductionConf flag JETM1 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkInDet.InDetCommon import *

# Include TRUTH3 containers
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()

#====================================================================
# SKIMMING TOOL
#====================================================================
from DerivationFrameworkJetEtMiss import TriggerLists
triggers = TriggerLists.jetTrig()

# NOTE: need to be able to OR isSimulated as an OR with the trigger
expression = ' (EventInfo.eventTypeBitmask==1) || HLT_xe120_pufit_L1XE50'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM1TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "JETM1TrigSkimmingTool1",
                                                                TriggerListOR          = triggers )
ToolSvc += JETM1TrigSkimmingTool


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM1OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JETM1OfflineSkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM1OfflineSkimmingTool

# OR of the above two selections
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
JETM1ORTool = DerivationFramework__FilterCombinationOR(name="JETM1ORTool", FilterList=[JETM1TrigSkimmingTool,JETM1OfflineSkimmingTool] )
ToolSvc+=JETM1ORTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm1Seq = CfgMgr.AthSequencer("JETM1Sequence")
DerivationFrameworkJob += jetm1Seq

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM1Stream )
JETM1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM1Stream.AcceptAlgs(["JETM1Kernel"])


#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM1ThinningHelper = ThinningHelper( "JETM1ThinningHelper" )
JETM1ThinningHelper.AppendToStream( JETM1Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM1MuonTPThinningTool",
                                                                    ThinningService         = JETM1ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM1MuonTPThinningTool
thinningTools.append(JETM1MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM1ElectronTPThinningTool",
                                                                               ThinningService         = JETM1ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM1ElectronTPThinningTool
thinningTools.append(JETM1ElectronTPThinningTool)

# TrackParticles associated with small-R jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM1Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM1Akt4JetTPThinningTool",
                                                                            ThinningService         = JETM1ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            SelectionString         = "AntiKt4EMTopoJets.pt > 18*GeV",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = False)
ToolSvc += JETM1Akt4JetTPThinningTool
thinningTools.append(JETM1Akt4JetTPThinningTool)

JETM1Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM1Akt4PFlowJetTPThinningTool",
                                                                                 ThinningService         = JETM1ThinningHelper.ThinningSvc(),
                                                                                 JetKey                  = "AntiKt4EMPFlowJets",
                                                                                 SelectionString         = "AntiKt4EMPFlowJets.pt > 18*GeV",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 ApplyAnd                = False)
ToolSvc += JETM1Akt4PFlowJetTPThinningTool
thinningTools.append(JETM1Akt4PFlowJetTPThinningTool)

thinning_expression = "InDetTrackParticles.JETM1DFLoose && ( abs(InDetTrackParticles.d0) < 3.0 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 4.0 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM1TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM1TPThinningTool",
                                                                  ThinningService         = JETM1ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                  ApplyAnd        = True)
ToolSvc += JETM1TPThinningTool
thinningTools.append(JETM1TPThinningTool)

#=======================================
# Augmentation tools
#=======================================

augmentationTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
JETM1TrackSelectionTool = DerivationFramework__InDetTrackSelectionToolWrapper(name = "JETM1TrackSelectionTool",
                                                                              ContainerName = "InDetTrackParticles",
                                                                              DecorationName = "JETM1DFLoose" )

JETM1TrackSelectionTool.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += JETM1TrackSelectionTool
augmentationTools.append(JETM1TrackSelectionTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm1Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM1Kernel" ,
                                                         AugmentationTools = augmentationTools,
                                                         SkimmingTools = [JETM1ORTool],
                                                         ThinningTools = thinningTools)


# Augment AntiKt4 jets with QG tagging variables
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=jetm1Seq,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=jetm1Seq,algname="QGTaggerToolPFAlg")

# Add alternative rho definitions
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
addCHSPFlowObjects()
from DerivationFrameworkJetEtMiss.JetCommon import defineEDAlg
jetm1Seq += defineEDAlg(R=0.4, inputtype="EMPFlowPUSB")
jetm1Seq += defineEDAlg(R=0.4, inputtype="EMPFlowNeut")

#SCHEDULE BTAGGING FOR PFLOW JETS
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'], Sequencer=jetm1Seq)

OutputJets["JETM1"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,jetm1Seq,"JETM1")


#############################################################################################################################################
# Ungroomed UFO jets
addDefaultUFOJets(jetm1Seq,"JETM1",doCHS=True)

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm1Seq,"JETM1")
addTrimmedJets("AntiKt", 1.0, "EMPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="pflow_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCHS", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCSSK", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")

if DerivationFrameworkHasTruth:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=True)
  addRecursiveSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, N=-1,  mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=True)
  addBottomUpSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=True)

addSoftDropJets("AntiKt", 1.0, "UFOCHS", beta=1.0, zcut=0.1, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")
addSoftDropJets("AntiKt", 1.0, "UFOCSSK", beta=1.0, zcut=0.1, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")
addRecursiveSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, N=-1,  mods="tcc_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False)
addBottomUpSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, mods="tcc_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False)


#############################################################################################################################################

# Add jets with constituent-level pileup suppression
addConstModJets("AntiKt",0.4,"EMTopo",["CS","SK"],jetm1Seq,"JETM1",
                ptmin=2000,ptminFilter=2000)
addConstModJets("AntiKt",0.4,"EMPFlow",["CS","SK"],jetm1Seq,"JETM1",
                ptmin=2000,ptminFilter=2000)

# Add the BCID info
addDistanceInTrain(jetm1Seq)

#add pFlow fJVT and MVfJVT for EMTopo
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=jetm1Seq, algname='JetForwardJvtToolBDTAlg')
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=jetm1Seq, algname='JetForwardPFlowJvtToolAlg')

#=======================================
# SCHEDULE SMALL-R JETS WITH LOW PT CUT
#=======================================

if DerivationFrameworkHasTruth:
    addAntiKt4NoPtCutJets(jetm1Seq,"JETM1")
    ## Add GhostTruthAssociation information ##
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4LCTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMTopoNoPtCut",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgNoPtCut")
    #Commenting for now as if disabled the building of this jet container in addAntiKt4NoPtCutJets, working on the reoptimisation of LCW at the moment
    #addJetPtAssociation(jetalg="AntiKt4LCTopoNoPtCut",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgNoPtCut")
    addJetPtAssociation(jetalg="AntiKt4EMPFlowNoPtCut", truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgNoPtCut")
    addJetPtAssociation(jetalg="AntiKt4EMTopoCSSK",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgCSSK")
    addJetPtAssociation(jetalg="AntiKt4EMPFlowCSSK", truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgCSSK")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM1SlimmingHelper = SlimmingHelper("JETM1SlimmingHelper")
JETM1SlimmingHelper.AppendToDictionary = {
    "Kt4EMPFlowPUSBEventShape": "xAOD::EventShape"    ,
    "Kt4EMPFlowPUSBEventShapeAux": "xAOD::AuxInfoBase"    ,
    "Kt4EMPFlowNeutEventShape": "xAOD::EventShape"    ,
    "Kt4EMPFlowNeutEventShapeAux": "xAOD::AuxInfoBase"    ,

}

JETM1SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "PrimaryVertices",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt10UFOCSSKJets",
                                        "AntiKt10UFOCHSJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCHSSoftDropBeta100Zcut10Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                        "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        ]

# Add QG tagger variables
JETM1SlimmingHelper.ExtraVariables  += ["AntiKt4EMTopoJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1",
                                        "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1"]
JETM1SlimmingHelper.ExtraVariables  += ["InDetTrackParticles.truthMatchProbability"]

JETM1SlimmingHelper.ExtraVariables += ["AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.zg.rg.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.EnergyPerSampling.GhostTrack",
                                       "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets.zg.rg.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.EnergyPerSampling.GhostTrack",
                                       "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10UFOCHSSoftDropBeta100Zcut10Jets.zg.rg",
                                       "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.zg.rg.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.EnergyPerSampling.GhostTrack",
                                       "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets.zg.rg.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.EnergyPerSampling.GhostTrack",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10UFOCSSKJets.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegmentCount.EnergyPerSampling.GhostTrack"]

JETM1SlimmingHelper.AllVariables = [ "MuonSegments", "TruthVertices", "TruthEvents"
                                     "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape","Kt4EMPFlowPUSBEventShape","Kt4EMPFlowNeutEventShape"]

# Trigger content
JETM1SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM1SlimmingHelper,["SmallR","JETM1"],["AntiKt10UFOCSSKJets",
                                                      "AntiKt10UFOCHSJets",
                                                      "AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCHSSoftDropBeta100Zcut10Jets",
                                                      "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                                      "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                                      "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                                     ], # smart list
              [
               "AntiKt4TruthWZJets", "AntiKt10EMPFlowJets", "AntiKt10EMPFlowCSSKJets"
               ]# veto list,
              )

if DerivationFrameworkHasTruth:
    JETM1SlimmingHelper.AllVariables += ["TruthMuons", "TruthElectrons", "TruthPhotons", "TruthEvents"]
    for truthc in ["TruthTop", "TruthBoson"]:
        JETM1SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
        JETM1SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")


JETM1SlimmingHelper.AppendContentToStream(JETM1Stream)
JETM1Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM1Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
