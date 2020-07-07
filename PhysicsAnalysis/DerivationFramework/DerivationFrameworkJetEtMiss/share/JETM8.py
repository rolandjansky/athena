#====================================================================
# JETM8.py 
# reductionConf flag JETM8 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
jetTriggers = TriggerLists.jetTrig()

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 150.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1)'
jetSelection += '||(count( AntiKt10TrackCaloClusterJets.pt > 150.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >=1)'
jetSelection += '||(count( AntiKt10UFOCSSKJets.pt > 150.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >=1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM8TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM8TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers )
ToolSvc += JETM8TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM8OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM8OfflineSkimmingTool",
                                                                        expression = jetSelection)
ToolSvc += JETM8OfflineSkimmingTool

#====================================================================
# THINNING TOOLS 
#====================================================================

thinningTools = []

#====================================================================
# Thin TruthParticles for truth jet constituents
#====================================================================

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__ViewContainerThinning
  JETM8TruthJetInputThin = DerivationFramework__ViewContainerThinning( name = "JETM8ViewContThinning",
                                                                       StreamName = streamName,
                                                                       TruthParticleKey = "TruthParticles",
                                                                       TruthParticleViewKey = "JetInputTruthParticles")

  ToolSvc += JETM8TruthJetInputThin
  thinningTools.append(JETM8TruthJetInputThin)

#====================================================================
# Thin jet inputs for jet constituents
#====================================================================

# Calo clusters
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
JETM8AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "JETM8AKt10CCThinningTool",
                                                                       StreamName            = streamName,
                                                                       SGKey                 = "AntiKt10LCTopoJets",
                                                                       SelectionString       = "(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8)",
                                                                       TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                       AdditionalClustersKey = ["LCOriginTopoClusters"],
                                                                       ApplyAnd              = False)
ToolSvc += JETM8AKt10CCThinningTool
thinningTools.append(JETM8AKt10CCThinningTool)

# Tracks and CaloClusters associated with TCCs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
JETM8TCCTPThinningTool = DerivationFramework__TCCTrackParticleThinning(name                         = "JETM8TCCTPThinningTool",
                                                                       StreamName                   = streamName,
                                                                       JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                       SelectionString              = "(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.8)",
                                                                       TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                       ThinOriginCorrectedClusters  = True,
                                                                       OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")

ToolSvc += JETM8TCCTPThinningTool
thinningTools.append(JETM8TCCTPThinningTool)

# Tracks and CaloClusters associated with UFOs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UFOTrackParticleThinning
JETM8EMCSSKUFOTPThinningTool = DerivationFramework__UFOTrackParticleThinning(name                   = "JETM8CSSKUFOTPThinningTool",
                                                                             StreamName             = streamName,
                                                                             JetKey                 = "AntiKt10UFOCSSKJets",
                                                                             UFOKey                 = "CSSKUFO",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                                             PFOCollectionSGKey     = "JetETMiss",
                                                                             AdditionalPFOKey       = ["CSSK"])

ToolSvc += JETM8EMCSSKUFOTPThinningTool
thinningTools.append(JETM8EMCSSKUFOTPThinningTool)

#====================================================================
# Thin tracks
#====================================================================

JETM8BaselineTrack = "(InDetTrackParticles.pt > 0.0)"

# This is necessary to keep tracks that would otherwise be removed by TCC and UFO thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM8TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name            = "JETM8TrackParticleThinningTool",
                                                                            StreamName      = streamName,
                                                                            SelectionString = JETM8BaselineTrack,
                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
                                                                            ApplyAnd        = False)

ToolSvc += JETM8TrackParticleThinningTool
thinningTools.append(JETM8TrackParticleThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm8Seq = CfgMgr.AthSequencer("JETM8Sequence")
DerivationFrameworkJob += jetm8Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM8TrigKernel", 
									SkimmingTools = [JETM8TrigSkimmingTool],
									ThinningTools = [])

#=======================================
# BUILD TCC INPUTS
#=======================================

# Add TCC constituents
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(jetm8Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles", outputTCCName="TrackCaloClustersCombinedAndNeutral")

#=======================================
# BUILD UFO INPUTS
#=======================================

## Add PFlow constituents
from JetRecTools.ConstModHelpers import getConstModSeq, xAOD
pflowCSSKSeq = getConstModSeq(["CS","SK"], "EMPFlow")

# add the pflow cssk sequence to the main jetalg if not already there :
if pflowCSSKSeq.getFullName() not in [t.getFullName() for t in DerivationFrameworkJob.jetalg.Tools]:
  DerivationFrameworkJob.jetalg.Tools += [pflowCSSKSeq]

# Add UFO constituents
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runUFOReconstruction
emcsskufoAlg = runUFOReconstruction(jetm8Seq, ToolSvc, PFOPrefix="CSSK",caloClusterName="LCOriginTopoClusters")

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt2LCTopoJets",
                  "AntiKt2TruthJets",
                  "AntiKt4TruthJets",
                  "AntiKt4TruthWZJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TrackCaloClusterJets",
                  "AntiKt10UFOCSSKJets"]
replaceAODReducedJets(reducedJetList,jetm8Seq,"JETM8")

jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM8MainKernel",
                                                          SkimmingTools = [JETM8OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# Jets for tagging
#====================================================================

OutputJets["JETM8"] = []

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm8Seq,"JETM8")

addTCCTrimmedJets(jetm8Seq,"JETM8")

if DerivationFrameworkIsMonteCarlo:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False)
  addRecursiveSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, N=-1,  mods="truth_groomed", algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False)
  addBottomUpSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, mods="truth_groomed", algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False)

addTrimmedJets("AntiKt", 1.0, "UFOCSSK", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False, mods="tcc_groomed")
addSoftDropJets("AntiKt", 1.0, "UFOCSSK", beta=1.0, zcut=0.1, algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False, mods="tcc_groomed")
addRecursiveSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, N=-1,  mods="tcc_groomed", algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False)
addBottomUpSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, mods="tcc_groomed", algseq=jetm8Seq, outputGroup="JETM8", writeUngroomed=False)

largeRJetAlgs = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10",
    "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5",
    "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5Ninf",
    ]

largeRJetCollections = []
for alg in largeRJetAlgs:
  largeRJetCollections.append(alg+"Jets")

if DerivationFrameworkIsMonteCarlo:
  for alg in largeRJetAlgs:
    addJetTruthLabel(jetalg=alg,sequence=jetm8Seq,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

# Add VR track jets for b-tagging
addVRJets(jetm8Seq, largeRJetCollections)
addVRJets(jetm8Seq, largeRJetCollections, do_ghost=True)
addVRJets(jetm8Seq, largeRJetCollections, training='201903')

#====================================================================
# Set up b-tagging
#====================================================================

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#tag pFlow jets
FlavorTagInit(scheduleFlipped = False, JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = jetm8Seq)

#====================================================================
# Add truth information
#====================================================================

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  from DerivationFrameworkMCTruth.MCTruthCommon import addTopQuarkAndDownstreamParticles
  from DerivationFrameworkMCTruth.MCTruthCommon import addHFAndDownstreamParticles
  from DerivationFrameworkMCTruth.MCTruthCommon import addTruthCollectionNavigationDecorations
  addStandardTruthContents()
  addTopQuarkAndDownstreamParticles()
  addHFAndDownstreamParticles(addB=True, addC=False, generations=0)
  addTruthCollectionNavigationDecorations(TruthCollections=["TruthTopQuarkWithDecayParticles","TruthBosonsWithDecayParticles"],prefix='Top')


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM8Stream )
JETM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM8Stream.AcceptAlgs(["JETM8MainKernel"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM8SlimmingHelper = SlimmingHelper("JETM8SlimmingHelper")
JETM8SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "AntiKt2TruthJets",
                                        "AntiKt2LCTopoJets",
                                        "AntiKt4TruthJets",
                                        "AntiKt4TruthWZJets",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "AntiKt10TruthJets",
                                        "AntiKt10TruthWZJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TruthSoftDropBeta100Zcut10Jets",
                                        "AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets",
                                        "AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKt10LCTopoJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TrackCaloClusterJets",
                                        "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKJets",
                                        "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                        "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                        ]
JETM8SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "TrackCaloClustersCombinedAndNeutral",
                                    "CSSKUFO",
                                    "TruthParticles",
                                    "Kt4EMPFlowEventShape"]

JETM8SlimmingHelper.AppendToDictionary["CSSKUFO"] = "xAOD::TrackCaloClusterContainer"
JETM8SlimmingHelper.AppendToDictionary["CSSKUFOAux"] = "xAOD::TrackCaloClusterAuxContainer"

JETM8SlimmingHelper.ExtraVariables += ["CSSKUFO.pt.eta.phi.taste"]
JETM8SlimmingHelper.ExtraVariables += ['AntiKt10LCTopoJets.SizeParameter',
                                       'AntiKt10TruthJets.SizeParameter',
                                       'AntiKt10TrackCaloClusterJets.SizeParameter',
                                       'AntiKt10UFOCSSKJets.SizeParameter',
                                       'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.SizeParameter',
                                       'AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets.SizeParameter',
                                       'AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets.SizeParameter',
                                       'AntiKt10TruthTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10TruthSoftDropBeta100Zcut10Jets.SizeParameter',
                                       'AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets.SizeParameter',
                                       'AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets.SizeParameter',
                                       'AntiKt10LCTopoJets.GhostTrack',
                                       'AntiKt10TrackCaloClusterJets.GhostTrack',
                                       'AntiKt10UFOCSSKJets.GhostTrack',
                                       'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.GhostTrack',
                                       'AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.GhostTrack',
                                       'AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.GhostTrack',
                                       'AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.GhostTrack',
                                       'AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets.GhostTrack',
                                       'AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets.GhostTrack',
                                       ]

# Add origin corrected clusters to keep LCTopo constituents
addOriginCorrectedClusters(JETM8SlimmingHelper, writeLC=True, writeEM=False)

# Add the jet containers to the stream
addJetOutputs(
    slimhelper = JETM8SlimmingHelper,
    contentlist = [
      "LargeR",
      "AntiKt4EMPFlowJets",
      "AntiKt4TruthJets",
      "AntiKt4TruthWZJets",
      "AntiKt2LCTopoJets",
      "AntiKt2TruthJets",
      "JETM8"
      ],
    smartlist = JETM8SlimmingHelper.SmartCollections
    )

for truthc in [
  "TruthTopQuark",
  "TruthBosons",
  "TruthHF",
  ]:
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")

for truthc in [
  "TruthMuons",
  "TruthElectrons",
  "TruthPhotons",
  "TruthBottom",
  "TruthBSM",
  ]:
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
  JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

printfunc (JETM8SlimmingHelper.AppendToDictionary)

JETM8SlimmingHelper.AppendContentToStream(JETM8Stream)
JETM8Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM8Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")

def removeVars(coll, vars):
    """Terrible hack to force removing a limited set of variables.
    Intended to remove definingParametersCovMatrix if possible.. """
    items = JETM8Stream.GetItems()
    origS = None
    for i in items:
        if coll in i and '.' in i:
            origS = i
            break
    if origS is None:
        return
    existV = origS.split('.')
    cleanedV = []
    for v in existV:
        if v in vars:
            continue
        cleanedV.append(v)
    newS = '.'.join(cleanedV)
    JETM8Stream.RemoveItem( origS ) 
    JETM8Stream.AddItem( newS ) 
removeVars('InDetTrackParticles', ['definingParametersCovMatrix',])
