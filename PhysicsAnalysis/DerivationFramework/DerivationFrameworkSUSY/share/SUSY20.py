#********************************************************************
# SUSY20.py
# Derivation for Displaced Track based search for WIMPs
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

     
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY20Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY20Stream )
SUSY20Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY20Stream.AcceptAlgs(["SUSY20KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY20ThinningHelper = ThinningHelper( "SUSY20ThinningHelper" )
thinningTools       = []
AugmentationTools   = []
DecorationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY20 = CfgMgr.AthSequencer("SeqSUSY20")
DerivationFrameworkJob += SeqSUSY20

#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY20TriggerList import triggersNavThin
SUSY20ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY20ThinningHelper.AppendToStream( SUSY20Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
SUSY20TrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name = "SUSY20TrackSelection",
                                                                                 ContainerName = "InDetTrackParticles",
                                                                                 DecorationName = "DFLoose" )
SUSY20TrackSelection.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += SUSY20TrackSelection

AugmentationTools.append(SUSY20TrackSelection)

thinning_expression = "InDetTrackParticles.DFLoose && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta) ) < 3.0)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# TrackParticles directly
SUSY20TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY20TPThinningTool",
                                                                 ThinningService         = SUSY20ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinning_expression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY20TPThinningTool
thinningTools.append(SUSY20TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY20MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY20MuonTPThinningTool",
                                                                         ThinningService         = SUSY20ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY20MuonTPThinningTool
thinningTools.append(SUSY20MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY20ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "SUSY20ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY20ThinningHelper.ThinningSvc(),
                                                                                 SGKey              = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY20ElectronTPThinningTool
thinningTools.append(SUSY20ElectronTPThinningTool)

# Photon thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY20PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY20PhotonThinningTool",
                                                                      ThinningService  = SUSY20ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY20PhotonThinningTool
thinningTools.append(SUSY20PhotonThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY20PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name       = "SUSY20PhotonTPThinningTool",
                                                                              ThinningService  = SUSY20ThinningHelper.ThinningSvc(),
                                                                              SGKey      = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY20PhotonTPThinningTool
thinningTools.append(SUSY20PhotonTPThinningTool)



# JEFF: run V0 finder
# JEFF: See also:
#       https://gitlab.cern.ch/atlas/athena/-/tree/21.2/InnerDetector/InDetRecAlgs/InDetV0Finder
#       https://twiki.cern.ch/twiki/bin/view/Main/InclusiveSecondaryVertexFinder
#       https://gitlab.cern.ch/atlas/athena/-/tree/master/InnerDetector/InDetRecAlgs/InDetInclusiveSecVtx
#====================================================================
# Add K_S0->pi+pi- reconstruction (TOPQDERIV-69)
#====================================================================
doSimpleV0Finder = False
if doSimpleV0Finder:
    include("DerivationFrameworkBPhys/configureSimpleV0Finder.py")
else:
    include("DerivationFrameworkBPhys/configureV0Finder.py")

SUSY20_V0FinderTools = BPHYV0FinderTools("SUSY20")

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_V0Finder
SUSY20_Reco_V0Finder   = DerivationFramework__Reco_V0Finder(
    name                   = "SUSY20_Reco_V0Finder",
    V0FinderTool           = SUSY20_V0FinderTools.V0FinderTool,
    #OutputLevel            = WARNING,
    OutputLevel            = DEBUG,
    V0ContainerName        = "SUSY20RecoV0Candidates",
    KshortContainerName    = "SUSY20RecoKshortCandidates",
    LambdaContainerName    = "SUSY20RecoLambdaCandidates",
    LambdabarContainerName = "SUSY20RecoLambdabarCandidates",
    CheckVertexContainers  = ["PrimaryVertices"]
)

ToolSvc += SUSY20_Reco_V0Finder
DecorationTools.append(SUSY20_Reco_V0Finder)

#====================================================================
# TRUTH THINNING
#====================================================================
#####################
##NO TRUTH THINNING##
#####################

#==========================================================================================
# ISOLATION DECORATING ( copied from DerivationFrameworkMuons/TrackIsolationDecorator.py )
#==========================================================================================
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
SUSY20TrackIsoTool = xAOD__TrackIsolationTool("SUSY20TrackIsoTool")
SUSY20TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 1.5
SUSY20TrackIsoTool.TrackSelectionTool.maxD0= 1.5
SUSY20TrackIsoTool.TrackSelectionTool.minPt= 1000.
SUSY20TrackIsoTool.TrackSelectionTool.CutLevel= "TightPrimary"
ToolSvc += SUSY20TrackIsoTool

from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
SUSY20IsoCorrectionTool = CP__IsolationCorrectionTool (name = "SUSY20IsoCorrectionTool", IsMC = DerivationFrameworkHasTruth)
ToolSvc += SUSY20IsoCorrectionTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
SUSY20CaloClustersInConeTool = xAOD__CaloClustersInConeTool("SUSY20CaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")
ToolSvc += SUSY20CaloClustersInConeTool

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
SUSY20CaloIsoTool = xAOD__CaloIsolationTool("SUSY20CaloIsoTool")
SUSY20CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.SUSY20IsoCorrectionTool
SUSY20CaloIsoTool.ClustersInConeTool = ToolSvc.SUSY20CaloClustersInConeTool
SUSY20CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
SUSY20CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
SUSY20CaloIsoTool.UseEMScale  = True
SUSY20CaloIsoTool.UseCaloExtensionCaching = False
SUSY20CaloIsoTool.saveOnlyRequestedCorrections = True
SUSY20CaloIsoTool.addCaloExtensionDecoration = False
ToolSvc += SUSY20CaloIsoTool

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# Calculate ptcone&ptvarcone, etcone&topoetcone
deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone30, isoPar.topoetcone20]
deco_prefix = ''  #'SUSY20_'

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParametersKVU
SUSY20KVU = DerivationFramework__TrackParametersKVU(name = "SUSY20KVU",
                                                             TrackParticleContainerName = "InDetPixelPrdAssociationTrackParticles",
                                                             VertexContainerName = "PrimaryVertices" )



ToolSvc += SUSY20KVU
DecorationTools.append(SUSY20KVU)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__CaloIsolationDecorator
SUSY20IDTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY20IDTrackDecorator",
                                                                    TrackIsolationTool = SUSY20TrackIsoTool,
                                                                    CaloIsolationTool = SUSY20CaloIsoTool,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    SelectionString = "InDetTrackParticles.pt>.5*GeV",
                                                                    ptcones = deco_ptcones,
                                                                    topoetcones = deco_topoetcones,
                                                                    Prefix = deco_prefix,
                                                                    )
ToolSvc += SUSY20IDTrackDecorator
DecorationTools.append(SUSY20IDTrackDecorator)


SUSY20PixelTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY20PixelTrackDecorator",
                                                                       TrackIsolationTool = SUSY20TrackIsoTool,
                                                                       CaloIsolationTool = SUSY20CaloIsoTool,
                                                                       TargetContainer = "InDetPixelPrdAssociationTrackParticles",
                                                                       SelectionString = "InDetPixelPrdAssociationTrackParticles.pt>.5*GeV",
                                                                       ptcones = deco_ptcones,
                                                                       topoetcones = deco_topoetcones,
                                                                       Prefix = deco_prefix,
                                                                       )
ToolSvc += SUSY20PixelTrackDecorator
DecorationTools.append(SUSY20PixelTrackDecorator)

        
#====================================================================
# SKIMMING TOOL 
#====================================================================

# ------------------------------------------------------------
# JEFF: Jet selection for skimming (which jet collection to use?)
applyJetCalibration_xAODColl('AntiKt4EMPFlow',SeqSUSY20) # JEFF: trying this, otherwise breaks with PFlow jets
jetRequirements = 'AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 200*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8'
jetSelection = '(count('+jetRequirements+') >= 1)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

SUSY20JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY20JetSkimmingTool",
                                                                        expression = jetSelection)
ToolSvc += SUSY20JetSkimmingTool # JEFF: turn this back on to add jet skimming

# ------------------------------------------------------------
# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
from DerivationFrameworkSUSY.SUSY20TriggerList import triggersMET,triggersSoftMuon,triggersJetPlusMet,triggersSoftMuonEmulation, triggersElectron, triggersPhoton
trigReq=triggersMET+triggersSoftMuon+triggersElectron+triggersPhoton

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND
SUSY20InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY20InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = trigReq)
ToolSvc += SUSY20InclusiveTriggerSkimmingTool

#SUSY20TriggerSkimmingTool = None
SUSY20TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY20TriggerSkimmingTool", 
                                                                         FilterList = [SUSY20InclusiveTriggerSkimmingTool])
ToolSvc += SUSY20TriggerSkimmingTool    
    


# ------------------------------------------------------------
# Final MET-based skim selection, with trigger selection and lepton selection
SUSY20SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY20SkimmingTool",
                                                               FilterList = [SUSY20JetSkimmingTool, SUSY20TriggerSkimmingTool])

ToolSvc += SUSY20SkimmingTool

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY20_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY20_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY20_MaxCellDecoratorTool

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
   SeqSUSY20 += CfgMgr.DerivationFramework__DerivationKernel("SUSY20KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY20")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY20 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY20KernelSkim",
  AugmentationTools = DecorationTools,
  SkimmingTools = [SUSY20SkimmingTool]
)



#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY20)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY20 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY20KernelAug",
  AugmentationTools = AugmentationTools,
  ThinningTools = thinningTools,
)

# Sicong Try VrtSecInclusive
#
#
#
#

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
ServiceMgr += InDetSCT_ConditionsSummarySvc

from RecExConfig.RecFlags import rec

# get inputFileSummary - will use it to extract info for MC/DATA
from RecExConfig.InputFilePeeker import inputFileSummary

# import the data types 
import EventKernel.ParticleDataType

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

include ("RecExCond/RecExCommon_flags.py")

include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#----------------------------------------------------------
#  VrtSecInclusive creates also TrackSummary tool.
#   TrackSummary tool creates InDetExtrapolator and AtlasMagneticFieldTool
#
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
SUSY20_VSI = VrtSecInclusive()


##SeqSUSY20.VrtSecInclusive.OutputLevel = DEBUG
SUSY20_VSI.OutputLevel = DEBUG
#SUSY20_VSI.OutputLevel = INFO
#SUSY20_VSI.CutBLayHits = 1
#SUSY20_VSI.CutPixelHits = 1
SUSY20_VSI.CutSctHits = 1
SUSY20_VSI.TrkA0ErrCut = 200000
SUSY20_VSI.TrkZErrCut = 200000
SUSY20_VSI.SelTrkMaxCutoff = 2000  
SUSY20_VSI.a0TrkPVDstMinCut = 0.0
SUSY20_VSI.TrkPtCut = 300
SUSY20_VSI.SelVrtChi2Cut=4.5
SUSY20_VSI.CutSharedHits=5
SUSY20_VSI.TrkChi2Cut=5.0
SUSY20_VSI.TruthTrkLen=1
SUSY20_VSI.DoSAloneTRT=False
SUSY20_VSI.DoTruth = False
# following is when there is no GEN_AOD in input file,
# e.g., when I run on output of InDetRecExample or on a ESD file
# when running on AOD output of InDetRecEx, explicitly uncomment the next line and comment out rec.readESD
#     SUSY20_VSI.MCEventContainer = "TruthEvent"

if rec.readESD():
    SUSY20_VSI.MCEventContainer = "TruthEvent"

if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    SUSY20_VSI.DoTruth=True


from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
	                                      Extrapolator        = ToolSvc.AtlasExtrapolator,
	                                      IterationNumber     = 30
					     )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO
SUSY20_VSI.VertexFitterTool=InclusiveVxFitterTool
SUSY20_VSI.Extrapolator = ToolSvc.AtlasExtrapolator

print VrtSecInclusive
SeqSUSY20 += SUSY20_VSI

#==============================================================================
# VrtSecInclusive IP Augmentor
#==============================================================================

from VrtSecInclusive.IPAugmentor import IPAugmentor

IPAugmentor = IPAugmentor("VsiIPAugmentor")
IPAugmentor.doAugmentDVimpactParametersToMuons     = True
IPAugmentor.doAugmentDVimpactParametersToElectrons = True
IPAugmentor.VertexFitterTool=InclusiveVxFitterTool

SeqSUSY20 += IPAugmentor

#MSMgr.GetStream("StreamDAOD_SUSY20").AddItem( [# 'xAOD::TrackParticleContainer#InDetTrackParticles*',
#                                               #   'xAOD::TrackParticleAuxContainer#InDetTrackParticles*',
#                                                  'xAOD::VertexContainer#VrtSecInclusive*',
#                                                  'xAOD::VertexAuxContainer#VrtSecInclusive*'
#                                                  ] )


# Sicong End Try VrtSecInclusive

#====================================================================
# Prompt Lepton Tagger
#====================================================================


import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY20, "SUSY20")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY20, "SUSY20")

# Electron and Muon algorithms: PromptLeptonIso and PromptLeptonVeto
SeqSUSY20 += JetTagConfig.GetDecoratePromptLeptonAlgs()

# Tau algorithm: PromptTauVeto
SeqSUSY20 += JetTagConfig.GetDecoratePromptTauAlgs()

#====================================================================
# TrackAssociatedCaloSampleDecorator
#====================================================================
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__TrackAssociatedCaloSampleDecorator

SUSY20_TrackAssociatedCaloSampleDecorator = DerivationFramework__TrackAssociatedCaloSampleDecorator(
  name             = "SUSY20_TrackAssociatedCaloSampleDecorator",
  ContainerName    = "InDetTrackParticles"
)
ToolSvc += SUSY20_TrackAssociatedCaloSampleDecorator
SeqSUSY20 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY20KernelDeco",
  AugmentationTools = [SUSY20_TrackAssociatedCaloSampleDecorator]
)

#====================================================================
# CONTENT LIST  
#====================================================================
# Sicong Note: SmartCollections, AllVariables, ExtraVariables, StaticContent
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY20ContentList import *
SUSY20SlimmingHelper = SlimmingHelper("SUSY20SlimmingHelper")
SUSY20SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons",
                                         "AntiKt4EMTopoJets",
                                         "AntiKt4EMPFlowJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "PrimaryVertices",
                                         #"BTagging_AntiKt4EMTopo",
                                         #"BTagging_AntiKt4EMPFlow",
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                         "BTagging_AntiKt4EMPFlow_201810",
                                         "BTagging_AntiKt4EMPFlow_201903",
                                         "AntiKt4EMTopoJets_BTagging201810", # JEFF
                                         "BTagging_AntiKt4EMTopo_201810", # JEFF
                                         "InDetTrackParticles"
                                         ]

SUSY20SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth",
                                     "MET_Core_AntiKt4EMTopo", # JEFF: why no equivalent for PFlow?
                                     "METAssoc_AntiKt4EMTopo", # JEFF: why no equivalent for PFlow?
                                     "InDetPixelPrdAssociationTrackParticles"
                                     #,"InDetTrackParticles" #Sicong
                                     ]
 


SUSY20SlimmingHelper.ExtraVariables = SUSY20ExtraVariables
SUSY20SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
SUSY20SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()

SUSY20SlimmingHelper.IncludeMuonTriggerContent = True
SUSY20SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY20SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY20SlimmingHelper.IncludeJetTriggerContent = True
SUSY20SlimmingHelper.IncludeTauTriggerContent = False

SUSY20SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY20SlimmingHelper.IncludeBJetTriggerContent = False

# JEFF V0
StaticContent = []

for containerName in ["SUSY20RecoV0Candidates","SUSY20RecoKshortCandidates","SUSY20RecoLambdaCandidates","SUSY20RecoLambdabarCandidates"]:
  StaticContent += [
            'xAOD::VertexContainer#%s'%containerName,
            'xAOD::VertexAuxContainer#%sAux'%containerName
            + '.-vxTrackAtVertex'  
            + '.-vertexType'
            + '.-neutralParticleLinks'
            + '.-neutralWeights'
            + '.-KshortLink'
            + '.-LambdaLink'
            + '.-LambdabarLink'
            + '.-gamma_fit'
            + '.-gamma_mass'
            + '.-gamma_massError'
            + '.-gamma_probability',
  ]
# JEFF V0

# Sicong: Try VSI
SUSY20SlimmingHelper.AllVariables += [ "VrtSecInclusive_SecondaryVertices" ]

# Include dvtrack variables from re-running of VSI 
original_dvtrack_vars = "is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV".split(".")
SUSY20SlimmingHelper.ExtraVariables += [ "InDetTrackParticles." + ".".join(original_dvtrack_vars) ]

vtx_vars = ["trackParticleLinks","neutralParticleLinks"]
SUSY20SlimmingHelper.ExtraVariables += [ "VrtSecInclusive_SecondaryVertices." + ".".join(vtx_vars) ]
StaticContent += [
            'xAOD::VertexContainer#VrtSecInclusive_SecondaryVertices',
            'xAOD::VertexAuxContainer#VrtSecInclusive_SecondaryVerticesAux'
            + '.-vxTrackAtVertex'
            + '.-vertexType'
            + '.-neutralParticleLinks'
            + '.-trackParticleLinks'
            + '.-neutralWeights'
 ]
SUSY20SlimmingHelper.StaticContent = StaticContent
# Sicong: End VSI

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:

  SUSY20SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY20SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]

SUSY20SlimmingHelper.AppendContentToStream(SUSY20Stream)
