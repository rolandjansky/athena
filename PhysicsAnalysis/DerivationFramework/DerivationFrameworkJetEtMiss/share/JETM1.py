#====================================================================
# JETM1.py
# reductionConf flag JETM1 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addCSSKSoftDropJets)
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

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



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm1Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM1Kernel" ,
                                                         SkimmingTools = [JETM1ORTool],
                                                         ThinningTools = thinningTools)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"           # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"           # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))" # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                # Photon
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM1TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM1TruthThinningTool",
                                                                        ThinningService = "JETM1ThinningSvc",
                                                                        ParticlesKey = "TruthParticles",
                                                                        VerticesKey = "TruthVertices",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)

    ToolSvc += JETM1TruthThinningTool
    thinningTools.append(JETM1TruthThinningTool)

#====================================================================
# Special jets
#====================================================================

# Create TCC objects
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction

from TrackCaloClusterRecTools.TrackCaloClusterConfig import runUFOReconstruction
emufoAlg = runUFOReconstruction(jetm1Seq,ToolSvc, PFOPrefix="CHS")
emcsskufoAlg = runUFOReconstruction(jetm1Seq,ToolSvc, PFOPrefix="CSSK")


# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(jetm1Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles", outputTCCName="TrackCaloClustersCombinedAndNeutral")

from JetRec.JetRecConf import PseudoJetGetter

csskufopjgetter = PseudoJetGetter("csskufoPJGetter", InputContainer="CSSKUFO", OutputContainer="CSSKUFOPJ", Label="UFO", SkipNegativeEnergy=True)
jtm+=csskufopjgetter

ufopjgetter     = PseudoJetGetter("ufoPJGetter",     InputContainer="CHSUFO",     OutputContainer="CHSUFOPJ",     Label="UFO", SkipNegativeEnergy=True)
jtm+=ufopjgetter

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
addCHSPFlowObjects()
addConstModJets("AntiKt", 1.0, "EMPFlow", ["CS", "SK"], jetm1Seq, "JETM1", ptmin=40000, ptminFilter=50000)
csskufogetters = [csskufopjgetter]+list(jtm.gettersMap["tcc"])[1:]
chsufogetters = [ufopjgetter]+list(jtm.gettersMap["tcc"])[1:]
addStandardJets("AntiKt", 1.0, "UFOCSSK", ptmin=40000, ptminFilter=50000, algseq=jetm1Seq, outputGroup="JETM1", customGetters = csskufogetters, constmods=["CSSK"])
addStandardJets("AntiKt", 1.0, "UFOCHS", ptmin=40000, ptminFilter=50000, algseq=jetm1Seq, outputGroup="JETM1", customGetters = chsufogetters, constmods=["CHS"])

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm1Seq,"JETM1")
addTCCTrimmedJets(jetm1Seq,"JETM1")
addTrimmedJets("AntiKt", 1.0, "EMPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="pflow_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCHS", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")
addTrimmedJets("AntiKt", 1.0, "UFOCSSK", rclus=0.2, ptfrac=0.05, algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False, mods="tcc_groomed")

if DerivationFrameworkIsMonteCarlo:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False)
  addRecursiveSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, N=-1,  mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False)
  addBottomUpSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, mods="truth_groomed", algseq=jetm1Seq, outputGroup="JETM1", writeUngroomed=False)



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

#=======================================
# SCHEDULE SMALL-R JETS WITH LOW PT CUT
#=======================================

if DerivationFrameworkIsMonteCarlo:
    addAntiKt4NoPtCutJets(jetm1Seq,"JETM1")
    ## Add GhostTruthAssociation information ##
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4LCTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMTopoNoPtCut",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgNoPtCut")
    addJetPtAssociation(jetalg="AntiKt4LCTopoNoPtCut",  truthjetalg="AntiKt4TruthJets", sequence=jetm1Seq, algname="JetPtAssociationAlgNoPtCut")
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
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
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

JETM1SlimmingHelper.ExtraVariables += ["AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets.zg.rg",
                                       "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10UFOCHSSoftDropBeta100Zcut10Jets.zg.rg",
                                       "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.zg.rg",
                                       "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets.zg.rg",
                                       "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.zg.rg",
                                       "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets.zg.rg",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.zg.rg"]


JETM1SlimmingHelper.AllVariables = [ "MuonTruthParticles", "egammaTruthParticles",
                                     "TruthParticles", "TruthEvents", "TruthVertices",
                                     "MuonSegments",
                                     "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape","Kt4EMPFlowPUSBEventShape","Kt4EMPFlowNeutEventShape",
                                     ]

# Trigger content
JETM1SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM1SlimmingHelper,["SmallR","JETM1"],["AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                                      "AntiKt10UFOCHSSoftDropBeta100Zcut10Jets",
                                                      "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                                      "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                                      "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                                     ], # smart list
              [
               "AntiKt4TruthWZJets", "AntiKt10UFOCSSKJets", "AntiKt10UFOCHSJets", "AntiKt10EMPFlowJets", "AntiKt10EMPFlowCSSKJets"
               ]# veto list,
              )

JETM1SlimmingHelper.AppendContentToStream(JETM1Stream)
JETM1Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM1Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
