#====================================================================
# MUON3.py for Z->mumu events
# This requires the reductionConf flag MUON3 in Reco_tf.py   
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
# from DerivationFrameworkJetEtMiss.METCommon import *
import AthenaCommon.SystemOfUnits as Units

if not hasattr(ToolSvc,"IDTrackCaloDepositsDecoratorTool"):
  from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import IDTrackCaloDepositsDecoratorTool
  DecoTool = IDTrackCaloDepositsDecoratorTool("IDTrackCaloDepositsDecoratorTool")
  if hasattr(DecoTool, "TrackDepositInCaloTool"):
    if not hasattr(ToolSvc,"TrkDepositInCaloTool"):
        from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackDepositInCaloTool
        TrkDepositInCaloTool = TrackDepositInCaloTool("TrkDepositInCaloTool")
        TrkDepositInCaloTool.CaloCellContainerName = "AODCellContainer"
        ToolSvc += TrkDepositInCaloTool
    DecoTool.TrackDepositInCaloTool = ToolSvc.TrkDepositInCaloTool

    ToolSvc += DecoTool

#====================================================================
# SET UP STREAM   
#====================================================================
MUON3Stream = MSMgr.NewPoolRootStream(derivationFlags.WriteDAOD_MUON3Stream.StreamName, buildFileName( derivationFlags.WriteDAOD_MUON3Stream))
MUON3Stream.AcceptAlgs(["MUON3Kernel"])

### trigger thinning
triggerList = ['HLT_mu14','HLT_2mu4','HLT_2mu6_bJpsimumu']

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
MUON3ThinningHelper = ThinningHelper( "MUON3ThinningHelper" )
MUON3ThinningHelper.TriggerChains = '|'.join(triggerList)
MUON3ThinningHelper.AppendToStream( MUON3Stream )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON3Stream )
MUON3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON3Stream.AcceptAlgs(["MUON3Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
MUON3AugmentTools = []

thinningORs = []

### Z->mumu events
brPrefix1a = 'MUON3a'
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
MUON3AugmentTool1a = DerivationFramework__dimuonTaggingTool(name = "MUON3AugmentTool1a",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
                                                           Mu1PtMin = 10*Units.GeV,
                                                           Mu1AbsEtaMax = 9999.,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {},
                                                           Mu2PtMin = 10*Units.GeV,
                                                           Mu2AbsEtaMax = 9999.,
                                                           Mu2Types = [],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = True, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = False,
                                                           InvariantMassLow = 60*Units.GeV,
                                                           InvariantMassHigh = -1,
                                                           IDTrackThinningConeSize = 0.3,
                                                           BranchPrefix = brPrefix1a
                                                           )

ToolSvc += MUON3AugmentTool1a
MUON3AugmentTools.append(MUON3AugmentTool1a)
thinningORs.append(brPrefix1a+'DIMU_trkStatus>0')

brPrefix1b = 'MUON3b'
MUON3AugmentTool1b = DerivationFramework__dimuonTaggingTool(name = "MUON3AugmentTool1b",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
                                                           Mu1PtMin = 5*Units.GeV,
                                                           Mu1AbsEtaMax = 9999.,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {},
                                                           Mu2PtMin = 5*Units.GeV,
                                                           Mu2AbsEtaMax = 9999.,
                                                           Mu2Types = [],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = True, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = False,
                                                           InvariantMassLow = 2.0*Units.GeV,
                                                           InvariantMassHigh = 4.8*Units.GeV,
                                                           IDTrackThinningConeSize = 0.3,
                                                           BranchPrefix = brPrefix1b
                                                          )

ToolSvc += MUON3AugmentTool1b
MUON3AugmentTools.append(MUON3AugmentTool1b)
thinningORs.append(brPrefix1b+'DIMU_trkStatus>0')

### isolation decorations
from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1IDTrackDecorator as MUON3AugmentTool2a
MUON3AugmentTool2a.SelectionString = "(InDetTrackParticles.MUON3aDIMU_Status==1000||InDetTrackParticles.MUON3bDIMU_Status==1000)"
MUON3AugmentTool2a.SelectionFlag = ""
MUON3AugmentTool2a.SelectionFlagValue = 0

ToolSvc += MUON3AugmentTool2a
MUON3AugmentTools.append(MUON3AugmentTool2a)

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1MSTrackDecorator as MUON3AugmentTool3
ToolSvc += MUON3AugmentTool3
MUON3AugmentTools.append(MUON3AugmentTool3)

#====================================================================
# SKIMMING
#====================================================================

muonsRequirementsLoose = '(Muons.pt >= 5*GeV)'
muonsRequirementsTight = '(Muons.pt >= 20*GeV)'
skimming_expression_WZ = 'HLT_mu14 && count('+muonsRequirementsTight+') >= 1'
skimming_expression_Jpsi = '(HLT_2mu4 || HLT_2mu6_bJpsimumu) && count('+muonsRequirementsLoose+') >= 2'
skimming_expression = '(' + skimming_expression_WZ + ') || (' + skimming_expression_Jpsi + ')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON3SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "MUON3SkimmingTool1",
                                                                         expression = skimming_expression)
ToolSvc += MUON3SkimmingTool1

#====================================================================
# THINNING
#====================================================================
MUON3ThinningTools = []

# keep tracks marked in augmentation
thinning_expression1 = '||'.join(thinningORs)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON3ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON3ThinningTool1",
                                                                StreamName              = streamName,
                                                                SelectionString         = thinning_expression1,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON3ThinningTool1
MUON3ThinningTools.append(MUON3ThinningTool1)

# keep tracks around muons
thinning_expression2 = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON3ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON3ThinningTool2",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.3,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON3ThinningTool2
MUON3ThinningTools.append(MUON3ThinningTool2)


# keep topoclusters around muons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
MUON3ThinningTool4 = DerivationFramework__CaloClusterThinning(name                    = "MUON3ThinningTool4",
                                                              StreamName              = streamName,
                                                              SGKey                   = "Muons",
                                                              SelectionString         = "Muons.pt>2.5*GeV",
                                                              TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                              ConeSize                = 0.3)
ToolSvc += MUON3ThinningTool4
MUON3ThinningTools.append(MUON3ThinningTool4)

### also for forward tracks
thinning_expression3 = "Muons.muonType==4"
MUON3ThinningTool2f = DerivationFramework__MuonTrackParticleThinning(name                   = "MUON3ThinningTool2f",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression3,
                                                                    ConeSize                = 0.3,
                                                                    InDetTrackParticlesKey  = "InDetForwardTrackParticles")
ToolSvc += MUON3ThinningTool2f
MUON3ThinningTools.append(MUON3ThinningTool2f)

### cell thinning
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["MuonClusterCollection"],
                                       outputCellKey="DFMUONCellContainer")

import IsolationAlgs.IsoUpdatedTrackCones as isoCones
if not hasattr(DerivationFrameworkJob,"IsolationBuilderTight1000"):
  DerivationFrameworkJob += isoCones.GetUpdatedIsoTrackCones()

from DerivationFrameworkMuons import  JPsiVertexFitSetup
MUON3AugmentTools += JPsiVertexFitSetup.AddJPsiVertexingAlgs('MUON3',False)

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
MUON3Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "MUON3Thin_vtxTrk",
  StreamName                 = streamName,
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["MUON3JpsiCandidates"],
  PassFlags                  = ["passed_Jpsi"] )

ToolSvc += MUON3Thin_vtxTrk
MUON3ThinningTools.append(MUON3Thin_vtxTrk)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
MUON3Seq = CfgMgr.AthSequencer("MUON3Sequence")
DerivationFrameworkJob += MUON3Seq
MUON3Seq += CfgMgr.DerivationFramework__DerivationKernel("MUON3Kernel",
                                                                       AugmentationTools = MUON3AugmentTools,
                                                                       SkimmingTools = [MUON3SkimmingTool1],
                                                                       ThinningTools = MUON3ThinningTools
                                                                       )
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
if hasattr(ToolSvc,"MUON3MuonTP_Reco_mumu"):
    conf.Items['MUON3']+=["xAOD::VertexContainer#%s"        % ToolSvc.MUON3MuonTP_Reco_mumu.OutputVtxContainerName]
    conf.Items['MUON3']+=["xAOD::VertexAuxContainer#%sAux." % ToolSvc.MUON3MuonTP_Reco_mumu.OutputVtxContainerName]
    conf.Items['MUON3']+=["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % ToolSvc.MUON3MuonTP_Reco_mumu.OutputVtxContainerName]

conf.Items['MUON3']+=["CaloCellContainer#DFMUONCellContainer"]
conf.Items['MUON3']+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]
conf.Config(MUON3Stream, 'MUON3')
