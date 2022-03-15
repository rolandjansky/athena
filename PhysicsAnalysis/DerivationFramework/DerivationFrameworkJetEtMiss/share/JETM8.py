#====================================================================
# JETM8.py 
# reductionConf flag JETM8 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo,
DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets, addJetOutputs
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import addVRJets
#from DerivationFrameworkFlavourTag.FlavourTagCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
jetTriggers = TriggerLists.jetTrig()

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 150.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1)'
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

jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM8MainKernel",
                                                          SkimmingTools = [JETM8OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# Jets for tagging
#====================================================================

OutputJets["JETM8"] = []

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
                                        "AntiKt10TruthJets",
                                        "AntiKt10TruthWZJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TruthSoftDropBeta100Zcut10Jets",
                                        "AntiKt10LCTopoJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKJets",
                                        "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                        ]
JETM8SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "CSSKUFO",
                                    "TruthParticles",
                                    "Kt4EMPFlowEventShape"]

JETM8SlimmingHelper.ExtraVariables += ["CSSKUFO.pt.eta.phi.taste"]
JETM8SlimmingHelper.ExtraVariables += ['AntiKt10LCTopoJets.SizeParameter',
                                       'AntiKt10TruthJets.SizeParameter',
                                       'AntiKt10TruthJets.Split12',
                                       'AntiKt10TruthJets.Split23',
                                       'AntiKt10UFOCSSKJets.SizeParameter',
                                       'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.SizeParameter',
                                       'AntiKt10TruthTrimmedPtFrac5SmallR20Jets.SizeParameter',
                                       'AntiKt10TruthSoftDropBeta100Zcut10Jets.SizeParameter',
                                       'AntiKt10LCTopoJets.GhostTrack',
                                       'AntiKt10UFOCSSKJets.GhostTrack',
                                       'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.GhostTrack',
                                       'AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets.GhostTrack',
                                       'AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.GhostTrack',
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
