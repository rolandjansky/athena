#********************************************************************
# SUSY19.py
# Derivation for pixel dE/dx based search for CHAMPs
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
streamName = derivationFlags.WriteDAOD_SUSY19Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY19Stream )
SUSY19Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY19Stream.AcceptAlgs(["SUSY19KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY19ThinningHelper = ThinningHelper( "SUSY19ThinningHelper" )
thinningTools       = []
AugmentationTools   = []
DecorationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY19 = CfgMgr.AthSequencer("SeqSUSY19")
DerivationFrameworkJob += SeqSUSY19



#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY19TriggerList import triggersNavThin
SUSY19ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY19ThinningHelper.AppendToStream( SUSY19Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
SUSY19TrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name = "SUSY19TrackSelection",
                                                                                 ContainerName = "InDetTrackParticles",
                                                                                 DecorationName = "DFLoose" )
SUSY19TrackSelection.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += SUSY19TrackSelection

AugmentationTools.append(SUSY19TrackSelection)

thinning_expression = "InDetTrackParticles.DFLoose && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta) ) < 3.0)"
#thinning_expression = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# TrackParticles directly
SUSY19TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY19TPThinningTool",
                                                                 ThinningService         = SUSY19ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinning_expression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19TPThinningTool
thinningTools.append(SUSY19TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY19MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY19MuonTPThinningTool",
                                                                         ThinningService         = SUSY19ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19MuonTPThinningTool
thinningTools.append(SUSY19MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY19ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "SUSY19ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY19ThinningHelper.ThinningSvc(),
                                                                                 SGKey              = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY19ElectronTPThinningTool
thinningTools.append(SUSY19ElectronTPThinningTool)

# Photon thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY19PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY19PhotonThinningTool",
                                                                      ThinningService  = SUSY19ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY19PhotonThinningTool
thinningTools.append(SUSY19PhotonThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY19PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name       = "SUSY19PhotonTPThinningTool",
                                                                              ThinningService  = SUSY19ThinningHelper.ThinningSvc(),
                                                                              SGKey      = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY19PhotonTPThinningTool
thinningTools.append(SUSY19PhotonTPThinningTool)



#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY19TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY19TruthThinningTool",
                                                                     ThinningService              = SUSY19ThinningHelper.ThinningSvc(),
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
                                                                     WriteAllLeptons              = True,
                                                                     WriteLeptonsNotFromHadrons   = False,
                                                                     WriteStatus3                 = False,
                                                                     WriteFirstN                  = 10,
                                                                     PreserveAncestors            = True,
                                                                     PreserveGeneratorDescendants = False,
                                                                     SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)

   
    # Decorate Electron with bkg electron type/origin 
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BkgElectronClassificationTool
    AugmentationTools.append(BkgElectronClassificationTool)
    
    ToolSvc += SUSY19TruthThinningTool
    thinningTools.append(SUSY19TruthThinningTool)


#==========================================================================================
# ISOLATION DECORATING ( copied from DerivationFrameworkMuons/TrackIsolationDecorator.py )
#==========================================================================================
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
SUSY19TrackIsoTool = xAOD__TrackIsolationTool("SUSY19TrackIsoTool")
SUSY19TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
SUSY19TrackIsoTool.TrackSelectionTool.minPt= 1000.
SUSY19TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += SUSY19TrackIsoTool

if DerivationFrameworkIsDataOverlay:
    raise RuntimeError('Not sure how to run over overlay data for SUSY19!')
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
SUSY19IsoCorrectionTool = CP__IsolationCorrectionTool (name = "SUSY19IsoCorrectionTool", IsMC = DerivationFrameworkHasTruth)
ToolSvc += SUSY19IsoCorrectionTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
SUSY19CaloClustersInConeTool = xAOD__CaloClustersInConeTool("SUSY19CaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")
ToolSvc += SUSY19CaloClustersInConeTool

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
SUSY19CaloIsoTool = xAOD__CaloIsolationTool("SUSY19CaloIsoTool")
SUSY19CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.SUSY19IsoCorrectionTool
SUSY19CaloIsoTool.ClustersInConeTool = ToolSvc.SUSY19CaloClustersInConeTool
SUSY19CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
SUSY19CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
SUSY19CaloIsoTool.UseEMScale  = True
SUSY19CaloIsoTool.UseCaloExtensionCaching = False
SUSY19CaloIsoTool.saveOnlyRequestedCorrections = True
SUSY19CaloIsoTool.addCaloExtensionDecoration = False
ToolSvc += SUSY19CaloIsoTool

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# Calculate ptcone&ptvarcone, etcone&topoetcone
deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone30, isoPar.topoetcone20]
deco_prefix = ''  #'SUSY19_'

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParametersKVU
SUSY19KVU = DerivationFramework__TrackParametersKVU(name = "SUSY19KVU",
                                                             TrackParticleContainerName = "InDetPixelPrdAssociationTrackParticles",
                                                             VertexContainerName = "PrimaryVertices" )



ToolSvc += SUSY19KVU
DecorationTools.append(SUSY19KVU)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__CaloIsolationDecorator
SUSY19IDTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY19IDTrackDecorator",
                                                                    TrackIsolationTool = SUSY19TrackIsoTool,
                                                                    CaloIsolationTool = SUSY19CaloIsoTool,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    SelectionString = "InDetTrackParticles.pt>10*GeV",
                                                                    ptcones = deco_ptcones,
                                                                    topoetcones = deco_topoetcones,
                                                                    Prefix = deco_prefix,
                                                                    )
ToolSvc += SUSY19IDTrackDecorator
DecorationTools.append(SUSY19IDTrackDecorator)


SUSY19PixelTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY19PixelTrackDecorator",
                                                                       TrackIsolationTool = SUSY19TrackIsoTool,
                                                                       CaloIsolationTool = SUSY19CaloIsoTool,
                                                                       TargetContainer = "InDetPixelPrdAssociationTrackParticles",
                                                                       SelectionString = "InDetPixelPrdAssociationTrackParticles.pt>10*GeV",
                                                                       ptcones = deco_ptcones,
                                                                       topoetcones = deco_topoetcones,
                                                                       Prefix = deco_prefix,
                                                                       )
ToolSvc += SUSY19PixelTrackDecorator
DecorationTools.append(SUSY19PixelTrackDecorator)

        
#====================================================================
# SKIMMING TOOL 
#====================================================================


# ------------------------------------------------------------
# Lepton selection
muonsRequirements = '(Muons.pt > 2.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 3*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
leptonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1)'
stdTrackRequirements = ' ( InDetTrackParticles.pt >= 1*GeV ) && ( ( InDetTrackParticles.ptcone20 / InDetTrackParticles.pt ) < 0.2 )'
pixTrackRequirements = ' ( InDetPixelPrdAssociationTrackParticles.pt >= 1*GeV ) && ( ( InDetPixelPrdAssociationTrackParticles.ptcone20 / InDetPixelPrdAssociationTrackParticles.pt ) < 0.2 ) '
trackExpression='( count('+stdTrackRequirements+') + count('+pixTrackRequirements+')>= 2 )'
expression='('+leptonSelection+' && '+trackExpression+')'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

SUSY19LeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY19LeptonSkimmingTool",
                                                                        expression = expression)
ToolSvc += SUSY19LeptonSkimmingTool

# ------------------------------------------------------------
# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
from DerivationFrameworkSUSY.SUSY19TriggerList import triggersMET,triggersSoftMuon,triggersJetPlusMet,triggersSoftMuonEmulation
#trigReq=triggersMET+triggersSoftMuon+triggersJetPlusMet
trigReq=triggersMET+triggersSoftMuon
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND
SUSY19InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = trigReq)
ToolSvc += SUSY19InclusiveTriggerSkimmingTool

SUSY19TriggerSkimmingTool=None
if DerivationFrameworkHasTruth:
    # one muon + jet + met trigger
    SUSY19SoftOneMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19OneMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_mu4','HLT_xe50_mht','HLT_j110'])
    ToolSvc += SUSY19SoftOneMuonTriggerSkimmingTool
    
    # dimuon + jet + met trigger
    SUSY19SoftTwoMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19TwoMuonTriggerSkimmingTool",
                                                                                     TriggerListAND = ['HLT_2mu4','HLT_j85'])
    ToolSvc += SUSY19SoftTwoMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY19TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY19TriggerSkimmingTool", 
                                                                         FilterList = [SUSY19InclusiveTriggerSkimmingTool,
                                                                                       SUSY19SoftOneMuonTriggerSkimmingTool,
                                                                                       SUSY19SoftTwoMuonTriggerSkimmingTool])
    ToolSvc += SUSY19TriggerSkimmingTool
else:
    # for data we can keep all of the prescaled triggers, may be nice to have them.
    SUSY19SoftMuonTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY19SoftMuonTriggerSkimmingTool",
                                                                                     TriggerListOR = triggersSoftMuonEmulation)
    ToolSvc += SUSY19SoftMuonTriggerSkimmingTool

    # OR of soft muon stuff or inclusive MET triggers
    SUSY19TriggerSkimmingTool = DerivationFramework__FilterCombinationOR(name = "SUSY19TriggerSkimmingTool", 
                                                                         FilterList = [SUSY19InclusiveTriggerSkimmingTool,
                                                                                       SUSY19SoftMuonTriggerSkimmingTool])
    ToolSvc += SUSY19TriggerSkimmingTool
    
# ------------------------------------------------------------

# ------------------------------------------------------------
# Final MET-based skim selection, with trigger selection and lepton selection
SUSY19SkimmingTool_MET = DerivationFramework__FilterCombinationAND(name = "SUSY19SkimmingTool_MET", 
                                                                   FilterList = [SUSY19LeptonSkimmingTool, 
                                                                                 SUSY19TriggerSkimmingTool])
ToolSvc += SUSY19SkimmingTool_MET

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY19_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY19_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY19_MaxCellDecoratorTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY19KernelSkim",
  AugmentationTools = DecorationTools,
  SkimmingTools = [SUSY19SkimmingTool_MET]
)



#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY19)

## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY19, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY19, algname='JetForwardJvtToolBDTAlg')


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY19KernelAug",
  AugmentationTools = AugmentationTools,
  ThinningTools = thinningTools,
)

#====================================================================
# Prompt Lepton Tagger
#====================================================================


import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY19, "SUSY19")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY19, "SUSY19")

# Electron and Muon algorithms: PromptLeptonIso and PromptLeptonVeto
SeqSUSY19 += JetTagConfig.GetDecoratePromptLeptonAlgs()

# Tau algorithm: PromptTauVeto
SeqSUSY19 += JetTagConfig.GetDecoratePromptTauAlgs()


#====================================================================
# TrackAssociatedCaloSampleDecorator
#====================================================================
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__TrackAssociatedCaloSampleDecorator

SUSY19_TrackAssociatedCaloSampleDecorator = DerivationFramework__TrackAssociatedCaloSampleDecorator(
  name             = "SUSY19_TrackAssociatedCaloSampleDecorator",
  ContainerName    = "InDetTrackParticles"
)
ToolSvc += SUSY19_TrackAssociatedCaloSampleDecorator
SeqSUSY19 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY19KernelDeco",
  AugmentationTools = [SUSY19_TrackAssociatedCaloSampleDecorator]
)

#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY19ContentList import *
SUSY19SlimmingHelper = SlimmingHelper("SUSY19SlimmingHelper")
SUSY19SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons","AntiKt4EMTopoJets",
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
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "InDetTrackParticles"
                                         ]

SUSY19SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth",
 "MET_Core_AntiKt4EMTopo","METAssoc_AntiKt4EMTopo", "InDetPixelPrdAssociationTrackParticles"]
SUSY19SlimmingHelper.ExtraVariables = SUSY19ExtraVariables
SUSY19SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
SUSY19SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
SUSY19SlimmingHelper.IncludeMuonTriggerContent = True
SUSY19SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY19SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY19SlimmingHelper.IncludeJetTriggerContent = True
SUSY19SlimmingHelper.IncludeTauTriggerContent = False
SUSY19SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY19SlimmingHelper.IncludeBJetTriggerContent = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:

  SUSY19SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY19SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   

SUSY19SlimmingHelper.AppendContentToStream(SUSY19Stream)
