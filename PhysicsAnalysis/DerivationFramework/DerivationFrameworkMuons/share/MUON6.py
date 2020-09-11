#====================================================================
# MUON6.py for muon momentum calibration
# This requires the reductionConf flag MUON6 in Reco_tf.py   
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
import AthenaCommon.SystemOfUnits as Units


#====================================================================
# SET UP STREAM   
#====================================================================
MUON6Stream = MSMgr.NewPoolRootStream(derivationFlags.WriteDAOD_MUON6Stream.StreamName, buildFileName(derivationFlags.WriteDAOD_MUON6Stream))
MUON6Stream.AcceptAlgs(["MUON6Kernel"])

kept_triggers = ['HLT_.*mu\d+.*']

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
MUON6ThinningHelper = ThinningHelper( "MUON6ThinningHelper" )
MUON6ThinningHelper.TriggerChains = '|'.join(kept_triggers)
MUON6ThinningHelper.AppendToStream( MUON6Stream )

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
MUON6AugmentTools = []

# Selection
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
MUON6AugmentToolSel = DerivationFramework__dimuonTaggingTool(name = 'MUON6AugmentToolSel',
                                                            Mu1PtMin = 3*Units.GeV,
                                                            Mu2PtMin = 3*Units.GeV,
                                                            UseTrackProbe = False,
                                                            TrackContainerKey = 'InDetTrackParticles',
                                                            OppositeCharge = False,
                                                            InvariantMassLow = 1.5*Units.GeV,
                                                            InvariantMassHigh = -1,
                                                            IDTrackThinningConeSize = 0.4,
                                                            BranchPrefix = 'MUON6'
                                                           )

ToolSvc += MUON6AugmentToolSel
MUON6AugmentTools.append(MUON6AugmentToolSel)
print MUON6AugmentToolSel

# Isolation decorations
from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1IDTrackDecorator as MUON6AugmentToolIsoDecID
MUON6AugmentToolIsoDecID.SelectionString = "(InDetTrackParticles.MUON6DIMU_Status==1000)"
MUON6AugmentToolIsoDecID.SelectionFlag = ""
MUON6AugmentToolIsoDecID.SelectionFlagValue = 0

ToolSvc += MUON6AugmentToolIsoDecID
MUON6AugmentTools.append(MUON6AugmentToolIsoDecID)
print MUON6AugmentToolIsoDecID

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1MSTrackDecorator as MUON6AugmentToolIsoDecMS
ToolSvc += MUON6AugmentToolIsoDecMS
MUON6AugmentTools.append(MUON6AugmentToolIsoDecMS)
print MUON6AugmentToolIsoDecMS

#====================================================================
# SKIMMING
#====================================================================
skimming_expression ='MUON6DIMU_pass>0'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON6SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "MUON6SkimmingTool", expression = skimming_expression)
ToolSvc += MUON6SkimmingTool

#====================================================================
# THINNING
#====================================================================
MUON6ThinningTools = []

# keep tracks with |z0sintheta| < 3
thinning_expression_z0 = "( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON6ThinningToolZ0 = DerivationFramework__TrackParticleThinning(name                    = "MUON6ThinningToolZ0",
                                                                 ThinningService         = MUON6ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinning_expression_z0,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ApplyAnd                = True)
ToolSvc += MUON6ThinningToolZ0
MUON6ThinningTools.append(MUON6ThinningToolZ0)

# keep tracks around muons
thinning_expression_around = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON6ThinningToolAround = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON6ThinningToolAround",
                                                                         ThinningService         = MUON6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         SelectionString         = thinning_expression_around,
                                                                         ConeSize                = 0.4,
                                                                         ApplyAnd                = True,
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON6ThinningToolAround
MUON6ThinningTools.append(MUON6ThinningToolAround)

# keep tracks marked in augmentation
thinning_expression_augment = 'MUON6DIMU_trkStatus>0'
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON6ThinningToolAugment = DerivationFramework__TrackParticleThinning(name                    = "MUON6ThinningToolAugment",
                                                                      ThinningService         = MUON6ThinningHelper.ThinningSvc(),
                                                                      SelectionString         = thinning_expression_augment,
                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                      ApplyAnd                = False)
ToolSvc += MUON6ThinningToolAugment
MUON6ThinningTools.append(MUON6ThinningToolAugment)

# keep topoclusters around muons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
MUON6ThinningToolTopo = DerivationFramework__CaloClusterThinning(name                    = "MUON6ThinningToolTopo",
                                                                 ThinningService         = MUON6ThinningHelper.ThinningSvc(),
                                                                 SGKey                   = "Muons",
                                                                 SelectionString         = "Muons.pt>4*GeV",
                                                                 TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                 ConeSize                = 0.5)
ToolSvc += MUON6ThinningToolTopo
MUON6ThinningTools.append(MUON6ThinningToolTopo)

### also for forward tracks
thinning_expression_forward = "Muons.muonType==4"
MUON6ThinningToolForward = DerivationFramework__MuonTrackParticleThinning(name                   = "MUON6ThinningToolForward",
                                                                          ThinningService        = MUON6ThinningHelper.ThinningSvc(),
                                                                          MuonKey                = "Muons",
                                                                          SelectionString        = thinning_expression_forward,
                                                                          ConeSize               = 0.5,
                                                                          ApplyAnd               = False,
                                                                          InDetTrackParticlesKey = "InDetForwardTrackParticles")
ToolSvc += MUON6ThinningToolForward
MUON6ThinningTools.append(MUON6ThinningToolForward)

### cell thinning
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["MuonClusterCollection"], outputCellKey="DFMUONCellContainer")

if DerivationFrameworkHasTruth:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  MUON6TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "MUON6TruthThinningTool",
                                                       ThinningService              = MUON6ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteCHadrons                = False,
                                                       WriteBHadrons                = False,
                                                       WriteGeant                   = False,
                                                       WriteTauHad                  = False,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = False,
                                                       WriteBosons                  = False,
                                                       WriteBosonProducts           = False,
                                                       WriteBSMProducts             = False,
                                                       WriteTopAndDecays            = False,
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = True,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = False,
                                                       PreserveParentsSiblingsChildren = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += MUON6TruthThinningTool
  MUON6ThinningTools.append(MUON6TruthThinningTool)

  from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonTruthClassifierFallback
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier
  MUON6MuonTruthClassifierFallback = DerivationFramework__MuonTruthClassifierFallback( name = "MUON6MuonTruthClassifierFallback", MCTruthClassifierTool = MCTruthClassifier, ContainerKey="Muons")
  ToolSvc += MUON6MuonTruthClassifierFallback
  MUON6AugmentTools.append(MUON6MuonTruthClassifierFallback)

  from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonTruthIsolationTool
  MUON6MuonTruthIsolationTool = DerivationFramework__MuonTruthIsolationTool( name = "MUON6MuonTruthIsolationTool", ContainerKey="Muons")
  ToolSvc += MUON6MuonTruthIsolationTool
  MUON6AugmentTools.append(MUON6MuonTruthIsolationTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
MUON6Seq = CfgMgr.AthSequencer("MUON6Sequence")
DerivationFrameworkJob += MUON6Seq
MUON6Seq += CfgMgr.DerivationFramework__DerivationKernel("MUON6Kernel", AugmentationTools = MUON6AugmentTools,
                                                                        SkimmingTools = [MUON6SkimmingTool],
                                                                        ThinningTools = MUON6ThinningTools
                                                                       )

#====================================================================
#       Decorate the vertices with the sum pt of their tracks
#====================================================================
alg_name = "MUONVertexDecorationAlg"
if not hasattr(DerivationFrameworkJob, alg_name ): 
    from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import VertexDecoratorAlg
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
   
    the_alg = VertexDecoratorAlg(alg_name)
    InDetTool = InDet__InDetTrackSelectionTool("TrackSelectionTool_VertexDecoration")
    InDetTool.minPt = 400
    InDetTool.CutLevel = "Loose"
    ToolSvc+= InDetTool
    the_alg.TrackSelectionTool = InDetTool
    DerivationFrameworkJob += the_alg

from DerivationFrameworkMuons import ConstituentPileupIso
ConstituentPileupIso.ConstituentPileupIso(MUON6Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON6Stream, 'MUON6')
