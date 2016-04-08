#====================================================================
# MUON1.py for Z->mumu events
# This requires the reductionConf flag MUON1 in Reco_tf.py   
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
# from DerivationFrameworkJetEtMiss.METCommon import *
import AthenaCommon.SystemOfUnits as Units

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# enum    MuonType {
#   Combined, MuonStandAlone, SegmentTagged, CaloTagged,
#   SiliconAssociatedForwardMuon
# }
# isolation cuts: check xAODPrimitives/xAODPrimitives/IsolationType.h for definitions
brPrefix = 'MUON1'
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
MUON1AugmentTool1 = DerivationFramework__dimuonTaggingTool(name = "MUON1AugmentTool1",
                                                           Mu1PtMin = 24*Units.GeV,
                                                           Mu1AbsEtaMax = 2.5,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {},
                                                           Mu2PtMin = 4*Units.GeV,
                                                           Mu2AbsEtaMax = 9999.,
                                                           Mu2Types = [],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = True, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = False,
                                                           InvariantMassLow = 60*Units.GeV,
                                                           InvariantMassHigh = -1,
                                                           IDTrackThinningConeSize = 0.5,
                                                           BranchPrefix = brPrefix
                                                           )

ToolSvc += MUON1AugmentTool1
print MUON1AugmentTool1

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1IDTrackDecorator as MUON1AugmentTool2
ToolSvc += MUON1AugmentTool2
print MUON1AugmentTool2

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1MSTrackDecorator as MUON1AugmentTool3
ToolSvc += MUON1AugmentTool3
print MUON1AugmentTool3
#====================================================================
# SKIMMING
#====================================================================
# expression = 'EventInfo.eventTypeBitmask==1||'+brPrefix+'DIMU_pass>0'
expression = brPrefix+'DIMU_pass>0'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON1SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "MUON1SkimmingTool1",
                                                                         expression = expression)
ToolSvc += MUON1SkimmingTool1

#====================================================================
# THINNING
#====================================================================
MUON1ThinningTools = []
# keep tracks marked in augmentation
thinning_expression1 = brPrefix+"DIMU_trkStatus>0"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON1ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON1ThinningTool1",
                                                                ThinningService         = "MUON1ThinningSvc",
                                                                SelectionString         = thinning_expression1,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += MUON1ThinningTool1
MUON1ThinningTools.append(MUON1ThinningTool1)

# keep tracks around muons
thinning_expression2 = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON1ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON1ThinningTool2",
                                                                    ThinningService         = "MUON1ThinningSvc",
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.5,
                                                                    ApplyAnd                = False,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON1ThinningTool2
MUON1ThinningTools.append(MUON1ThinningTool2)


# keep cells around muons
# from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CellThinning
# MUON1ThinningTool3 = DerivationFramework__CellThinning(name                    = "MUON1ThinningTool3",
#                                                        ThinningService         = "MUON1ThinningSvc",
#                                                        SGKey_muons             = "Muons",
#                                                        SGKey_AODCells          = "AODCellContainer",
#                                                        ConeSize                = 0.5) # conesize is actually not used.
# ToolSvc += MUON1ThinningTool3
# print MUON1ThinningTool3
# MUON1ThinningTools.append(MUON1ThinningTool3)

# keep topoclusters around muons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
MUON1ThinningTool4 = DerivationFramework__CaloClusterThinning(name                    = "MUON1ThinningTool4",
                                                              ThinningService         = "MUON1ThinningSvc",
                                                              SGKey                   = "Muons",
                                                              SelectionString         = "Muons.pt>4*GeV",
                                                              TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                              ConeSize                = 0.5)
ToolSvc += MUON1ThinningTool4
print MUON1ThinningTool4
MUON1ThinningTools.append(MUON1ThinningTool4)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON1Kernel",
#                                                                        AugmentationTools = [MUON1AugmentTool1],
                                                                       AugmentationTools = [MUON1AugmentTool1, MUON1AugmentTool2, MUON1AugmentTool3],
                                                                       SkimmingTools = [MUON1SkimmingTool1],
                                                                       ThinningTools = MUON1ThinningTools
                                                                       )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON1Stream )
MUON1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON1Stream.AcceptAlgs(["MUON1Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MUON1ThinningSvc", outStreams=[evtStream] )
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON1Stream, 'MUON1')
