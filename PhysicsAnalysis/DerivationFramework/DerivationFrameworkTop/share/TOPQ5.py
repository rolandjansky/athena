#====================================================================
# TOPQ5
# SINGLE TOP SELECTION PLUS J/PSI->MUMU CANDIDATE
#   >=1 electron(pT>20GeV) OR
#   >=1 muon(pT>20GeV)
#   2 additional muons with 0GeV<M_mumu<20GeV
# reductionConf flag TOPQ5 in Reco_tf.py
# Shares most settings with TOPQ1
#====================================================================

#====================================================================
# IMPORTS - Order Matters
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags
DFisMC = (globalflags.DataSource()=='geant4')

# no truth info for data xAODs
if DFisMC:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_TOPQ5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ5Stream )
TOPQ5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ5Stream.AcceptAlgs(["TOPQ5Kernel"])

#====================================================================
# PDF Weight Metadata
#====================================================================
if DFisMC:
  from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# TRIGGER NAVIGATION THINNING
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
import DerivationFrameworkTop.TOPQCommonThinning
TOPQ5ThinningHelper = ThinningHelper("TOPQ5ThinningHelper")
TOPQ5ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('leptonicTriggers' if globalflags.DataSource()!='geant4' else 'allTriggers')
TOPQ5ThinningHelper.AppendToStream(TOPQ5Stream)

#====================================================================
# J/PSI RECONSTRUCTION
#====================================================================

include("DerivationFrameworkBPhys/configureVertexing.py")
TOPQ5_VertexTools = BPHYVertexTools("TOPQ5")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
TOPQ5JpsiFinder = Analysis__JpsiFinder(
  name                        = "TOPQ5JpsiFinder",
  OutputLevel                 = INFO,
  muAndMu                     = True,
  muAndTrack                  = False,
  TrackAndTrack               = False,
  assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = 20000.0,
  invMassLower                = 0.0,
  Chi2Cut                     = 200.,
  oppChargesOnly                    = True,
  atLeastOneComb              = True,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = TOPQ5_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = TOPQ5_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = TOPQ5_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = TOPQ5_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = TOPQ5_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False )

ToolSvc += TOPQ5JpsiFinder
print      TOPQ5JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
TOPQ5_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "TOPQ5_Reco_mumu",
  JpsiFinder             = TOPQ5JpsiFinder,
  OutputVtxContainerName = "TOPQ5OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "TOPQ5RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)

ToolSvc += TOPQ5_Reco_mumu
print TOPQ5_Reco_mumu

#--------------------------------------------------------------------
## 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
##    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
##    Property HypothesisName is used as a prefix for these decorations.
##    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
##    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
##    to determine which events and candidates should be kept in the output stream.
##    Multiple instances of the Select_* tools can be used on a single input collection as long as they
##    use different "HypothesisName" flags.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
TOPQ5_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "TOPQ5_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "TOPQ5OniaCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 0.0,
  MassMax               = 20000.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += TOPQ5_Select_Jpsi2mumu
print TOPQ5_Select_Jpsi2mumu

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ5', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ5', ToolSvc)

jpsiExpression = "count(TOPQ5OniaCandidates.passed_Jpsi) > 0"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TOPQ5_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "TOPQ5_SelectEvent",
                                                                expression = jpsiExpression)
ToolSvc += TOPQ5_SelectEvent
print TOPQ5_SelectEvent

skimmingTools_lep.append(TOPQ5_SelectEvent)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ5',TOPQ5ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ5Sequence = CfgMgr.AthSequencer("TOPQ5Sequence")

# First skim on leptons
TOPQ5Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ5SkimmingKernel_lep",
                                                              SkimmingTools = skimmingTools_lep,
                                                              AugmentationTools = [TOPQ5_Reco_mumu,TOPQ5_Select_Jpsi2mumu])

# Then build fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addStandardJetsForTop
addStandardJetsForTop(TOPQ5Sequence,'TOPQ5')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ5Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ5Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ5SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP'],
      ['AntiKt4EMPFlowJets'],
      TOPQ5Sequence)

# Removing manual scheduling of ELReset, see https://its.cern.ch/jira/browse/ATLASRECTS-3988
# if not hasattr(TOPQ5Sequence,"ELReset"):
#   TOPQ5Sequence += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )


# THIS IS NO LONGER NEEDED IN REL 21, REMOVE IN FUTURE (May-17)
# # Then apply the TruthWZ fix
# if DFisMC:
#   replaceBuggyAntiKt4TruthWZJets(TOPQ5Sequence,'TOPQ5')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ5Sequence += TOPQCommonTruthKernel

DerivationFrameworkTop.TOPQCommonJets.addMSVVariables("AntiKt4EMTopoJets", TOPQ5Sequence, ToolSvc)

# Then apply thinning
TOPQ5Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ5Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ5Sequence, 'TOPQ5')

# Add BDT decoration algs
TOPQ5Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ5Sequence += JetTagConfig.GetDecoratePromptTauAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ5Sequence

#====================================================================
# SLIMMING
#====================================================================
from DerivationFrameworkTop.TOPQCommonExtraContent import TOPQStaticContent
TOPQStaticContent += ['xAOD::VertexContainer#TOPQ5RefittedPrimaryVertices']
TOPQStaticContent += ['xAOD::VertexAuxContainer#TOPQ5RefittedPrimaryVerticesAux.']

import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ5', TOPQ5Stream)

## Jpsi candidates
TOPQ5Stream.AddItem("xAOD::VertexContainer#TOPQ5OniaCandidates")
TOPQ5Stream.AddItem("xAOD::VertexAuxContainer#TOPQ5OniaCandidatesAux.")
TOPQ5Stream.AddItem("xAOD::VertexAuxContainer#TOPQ5OniaCandidatesAux.-vxTrackAtVertex")
