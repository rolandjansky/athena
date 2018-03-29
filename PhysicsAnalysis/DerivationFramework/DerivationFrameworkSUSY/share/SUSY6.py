#********************************************************************
# SUSY6.py
# Derivation for pixel dE/dx based search for CHAMPs
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

     
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY6Stream )
SUSY6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY6Stream.AcceptAlgs(["SUSY6KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY6ThinningHelper = ThinningHelper( "SUSY6ThinningHelper" )
thinningTools       = []
AugmentationTools   = []
DecorationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY6 = CfgMgr.AthSequencer("SeqSUSY6")
DerivationFrameworkJob += SeqSUSY6


#====================================================================
# Trigger navigation thinning
#====================================================================
#SUSY6ThinningHelper.TriggerChains = 'HLT_xe.*|HLT_2mu14||HLT_j80_xe80.*|HLT_j100_xe80.*|HLT_e.*|HLT_mu.*'
from DerivationFrameworkSUSY.SUSY6TriggerList import *
SUSY6ThinningHelper.TriggerChains = '|'.join(SUSY6dimuonTriggers+SUSY6singleEleTriggers+SUSY6singleMuTriggers)
SUSY6ThinningHelper.AppendToStream( SUSY6Stream )


#====================================================================
# THINNING TOOLS 
#====================================================================

thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (abs(DFCommonInDetTrackZ0AtPV) < 3.0)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY6MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY6MetTPThinningTool",
                                                                ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY6MetTPThinningTool
thinningTools.append(SUSY6MetTPThinningTool)


# TrackParticles directly
SUSY6TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY6TPThinningTool",
                                                                 ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 1*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY6TPThinningTool
thinningTools.append(SUSY6TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY6MuonTPThinningTool",
                                                                         ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY6MuonTPThinningTool
thinningTools.append(SUSY6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY6ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6ElectronTPThinningTool
thinningTools.append(SUSY6ElectronTPThinningTool)

# Photon thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY6PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY6PhotonThinningTool",
                                                                      ThinningService  = SUSY6ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY6PhotonThinningTool
thinningTools.append(SUSY6PhotonThinningTool)

#==========================================================================================
# ISOLATION DECORATING ( copied from DerivationFrameworkMuons/TrackIsolationDecorator.py )
#==========================================================================================
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
IsoCorrectionTool = CP__IsolationCorrectionTool ("NewLeakageCorrTool",IsMC = isMC)
ToolSvc += IsoCorrectionTool

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
MyCaloClustersInConeTool = xAOD__CaloClustersInConeTool("MyCaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")
ToolSvc += MyCaloClustersInConeTool

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsoTool = xAOD__CaloIsolationTool("CaloIsoTool")
CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
CaloIsoTool.ClustersInConeTool = ToolSvc.MyCaloClustersInConeTool
CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
CaloIsoTool.UseEMScale  = True
CaloIsoTool.UseCaloExtensionCaching = False
CaloIsoTool.saveOnlyRequestedCorrections = True
CaloIsoTool.addCaloExtensionDecoration = False
ToolSvc += CaloIsoTool

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# Calculate ptcone&ptvarcone, etcone&topoetcone
deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone30, isoPar.topoetcone20]
deco_prefix = ''  #'SUSY6_'

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParametersKVU
SUSY6DFCommonKVU = DerivationFramework__TrackParametersKVU(name = "SUSY6DFCommonKVU",
                                                             TrackParticleContainerName = "InDetPixelPrdAssociationTrackParticles",
                                                             VertexContainerName = "PrimaryVertices", 
                                                             KVUSGEntryName = "DFCommonInDetTrackKVU" )



ToolSvc += SUSY6DFCommonKVU
DecorationTools.append(SUSY6DFCommonKVU)

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__CaloIsolationDecorator
SUSY6IDTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY6IDTrackDecorator",
                                                                    TrackIsolationTool = TrackIsoTool,
                                                                    CaloIsolationTool = CaloIsoTool,
                                                                    TargetContainer = "InDetTrackParticles",
                                                                    SelectionString = "InDetTrackParticles.pt>10*GeV",
                                                                    ptcones = deco_ptcones,
                                                                    topoetcones = deco_topoetcones,
                                                                    Prefix = deco_prefix,
                                                                    )
ToolSvc += SUSY6IDTrackDecorator
DecorationTools.append(SUSY6IDTrackDecorator)


SUSY6PixelTrackDecorator = DerivationFramework__CaloIsolationDecorator(name = "SUSY6PixelTrackDecorator",
                                                                       TrackIsolationTool = TrackIsoTool,
                                                                       CaloIsolationTool = CaloIsoTool,
                                                                       TargetContainer = "InDetPixelPrdAssociationTrackParticles",
                                                                       SelectionString = "InDetPixelPrdAssociationTrackParticles.pt>10*GeV",
                                                                       ptcones = deco_ptcones,
                                                                       topoetcones = deco_topoetcones,
                                                                       Prefix = deco_prefix,
                                                                       )
ToolSvc += SUSY6PixelTrackDecorator
DecorationTools.append(SUSY6PixelTrackDecorator)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY6TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY6TruthThinningTool",
                                                       ThinningService              = SUSY6ThinningHelper.ThinningSvc(),
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
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True, 
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY6TruthThinningTool
  thinningTools.append(SUSY6TruthThinningTool)

  truth_expression = '(((abs(TruthParticles.pdgId) >= 1000000) && (abs(TruthParticles.pdgId) <= 1000040)) || ((abs(TruthParticles.pdgId) >= 2000000) && (abs(TruthParticles.pdgId) <= 2000040)))'
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  SUSY6TruthSUSYThinningTool = DerivationFramework__GenericTruthThinning( name                         = "SUSY6TruthSUSYThinningTool",
                                                                          ThinningService              = SUSY6ThinningHelper.ThinningSvc(),
                                                                          ParticleSelectionString      = truth_expression,
                                                                          PreserveDescendants          = True,
                                                                          PreserveGeneratorDescendants = False,
                                                                          SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset
                                                                         )
  ToolSvc += SUSY6TruthSUSYThinningTool
  thinningTools.append(SUSY6TruthSUSYThinningTool)

        
#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkSUSY.SUSY6TriggerList import *

dimuonTriggers       = SUSY6dimuonTriggers
jetxeTriggers        = SUSY6jetxeTriggers
singleEleTriggers    = SUSY6singleEleTriggers
singleMuTriggers     = SUSY6singleMuTriggers
xeTriggers           = SUSY6xeTriggers

#if DerivationFrameworkIsMonteCarlo:
#        xeTriggers = [trig for trig in xeTriggers if 'xe100' in trig]

stdTrackRequirements = ' ( InDetTrackParticles.pt >= 10*GeV ) && ( ( InDetTrackParticles.ptvarcone20 / InDetTrackParticles.pt ) < 0.2 )'
pixTrackRequirements = ' ( InDetPixelPrdAssociationTrackParticles.pt >= 10*GeV ) && ( ( InDetPixelPrdAssociationTrackParticles.ptvarcone20 / InDetPixelPrdAssociationTrackParticles.pt ) < 0.2 ) '
trackExpression='( count('+stdTrackRequirements+') + count('+pixTrackRequirements+')>= 2 )'

electronRequirements = '(Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium) && (Electrons.ptvarcone20 / Electrons.pt < 0.15) && (Electrons.topoetcone20 / Electrons.pt < 0.2)'
muonRequirements = '(Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection) && (Muons.ptvarcone30 / Muons.pt < 0.15) && (Muons.topoetcone20 / Muons.pt < 0.3)'
electronExpression = '( count('+electronRequirements+') >= 1 ) && ( ' + ' || '.join(singleEleTriggers) + ') && ' + trackExpression
muonExpression     = '( count('+muonRequirements+') >= 1 ) && (' + ' || '.join(singleMuTriggers) + ') && ' + trackExpression

ZllRequirements=' ( ' + electronExpression + ' ) || ( ' + muonExpression + ' ) '

allTriggers = xeTriggers + dimuonTriggers + jetxeTriggers

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY6ZllSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY6ZllSkimmingTool",
                                                                    expression = ZllRequirements)

ToolSvc += SUSY6ZllSkimmingTool

# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY6TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "JetTriggerSkimmingTool",
                                                                     TriggerListOR = allTriggers)
ToolSvc += SUSY6TriggerSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SUSY6SkimmingORTool = DerivationFramework__FilterCombinationOR(name = "SUSY6SkimmingORTool",
                                                          FilterList = [SUSY6ZllSkimmingTool, SUSY6TriggerSkimmingTool])
ToolSvc += SUSY6SkimmingORTool


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
   SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel("SUSY6KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY6")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY6KernelSkim",
  AugmentationTools = DecorationTools,
  SkimmingTools = [SUSY6SkimmingORTool]
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY6)

#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY6"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY6, "SUSY6")


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
SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY6KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY6SlimmingHelper = SlimmingHelper("SUSY6SlimmingHelper")
SUSY6SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "AntiKt4EMTopoJets",
"AntiKt4EMPFlowJets",
 "MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",
 "PrimaryVertices"]
SUSY6SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "InDetTrackParticles", "MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",
 "MET_Core_AntiKt4EMTopo","METAssoc_AntiKt4EMTopo", "InDetPixelPrdAssociationTrackParticles"]
SUSY6SlimmingHelper.ExtraVariables = [ "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
          "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID"]
SUSY6SlimmingHelper.IncludeMuonTriggerContent = True
SUSY6SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY6SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeJetTriggerContent = False
SUSY6SlimmingHelper.IncludeTauTriggerContent = True
SUSY6SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeBJetTriggerContent = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY6SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY6SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   

SUSY6SlimmingHelper.AppendContentToStream(SUSY6Stream)
