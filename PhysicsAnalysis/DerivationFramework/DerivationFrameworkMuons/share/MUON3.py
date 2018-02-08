#====================================================================
# MUON3.py for Jpsi->mumu tag&probe
# This requires the reductionConf flag MUON3 in Reco_tf.py   
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
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
# AUGMENTATION TOOLS
#====================================================================
# enum    MuonType {
#   Combined, MuonStandAlone, SegmentTagged, CaloTagged,
#   SiliconAssociatedForwardMuon
# }
# isolation cuts: check xAODPrimitives/xAODPrimitives/IsolationType.h for definitions
myAugTools = []
skimmingORs = []
thinningORs = []
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
addL2StarB = lambda x: x+[i+'_L2StarB' for i in x]

### Jpsi tagging
brPrefix1 = 'MUON3a'
orTriggers1 = []
andTriggers1a_run1 = ['EF_2mu6_Jpsimumu', 'EF_2mu4T_Jpsimumu', 'EF_mu4Tmu6_Jpsimumu', 'EF_2mu4T_Jpsimumu_Barrel', 'EF_2mu4T_Jpsimumu_BarrelOnly', 'EF_mu4Tmu6_Jpsimumu_Barrel']
andTriggers1_run1 = addL2StarB(andTriggers1a_run1)
# andTriggers1_run2 = ['HLT_2mu4_bJpsimumu','HLT_2mu6_bJpsimumu','HLT_2mu10_bJpsimumu','HLT_mu6_mu4_bJpsimumu','HLT_3mu4_bJpsi','HLT_3mu6_bJpsi','HLT_mu18_2mu4_JpsimumuL2','HLT_mu20_2mu4_JpsimumuL2','HLT_mu18_2mu0noL1_JpsimumuFS','HLT_mu18_2mu4_JpsimumuL2']
andTriggers1_run2 = [] # No trigger in 8TeV data

andTriggers1 = andTriggers1_run2
MUON3AugmentTool1 = DerivationFramework__dimuonTaggingTool(name = "MUON3AugmentTool1",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
                                                           OrTrigs = orTriggers1,
                                                           AndTrigs = andTriggers1,
                                                           Mu1PtMin = 5.*Units.GeV,
                                                           Mu1AbsEtaMax = 999.,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {}, #ptcone20<10 GeV, etcone40<20 GeV
                                                           Mu2PtMin = 5.*Units.GeV,
                                                           Mu2AbsEtaMax = 999.,
                                                           Mu2Types = [0],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = False, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = True,
                                                           InvariantMassLow = 2.0*Units.GeV,
                                                           InvariantMassHigh = 4.8*Units.GeV,
                                                           IDTrackThinningConeSize = 0.5,
                                                           BranchPrefix = brPrefix1
                                                           )

ToolSvc += MUON3AugmentTool1
myAugTools.append(MUON3AugmentTool1)
skimmingORs.append(brPrefix1+'DIMU_pass>0')
thinningORs.append(brPrefix1+'DIMU_trkStatus>0')
# thinningORs.append('('+brPrefix1+'DIMU_pass>0&&'+brPrefix1+'DIMU_trkStatus>0)')
print MUON3AugmentTool1

### Upsilon tagging
brPrefix2 = 'MUON3b'
orTriggers2 = []
andTriggers2a_run1 = ['EF_2mu6_Upsimumu', 'EF_2mu4T_Upsimumu', 'EF_mu4Tmu6_Upsimumu', 'EF_2mu4T_Upsimumu_Barrel', 'EF_2mu4T_Upsimumu_BarrelOnly', 'EF_mu4Tmu6_Upsimumu_Barrel']
andTriggers2_run1 = addL2StarB(andTriggers2a_run1)

# andTriggers2a_run2 = ['HLT_2mu4_bUpsimumu','HLT_2mu6_bUpsimumu','HLT_2mu10_bUpsimumu','HLT_mu6_mu4_bUpsimumu','HLT_3mu4_bUpsi','HLT_3mu6_bUpsi']
andTriggers2a_run2 = [] # No trigger in 8TeV data

andTriggers2 = andTriggers2a_run2
print andTriggers2

MUON3AugmentTool2 = DerivationFramework__dimuonTaggingTool(name = "MUON3AugmentTool2",
                                                           OrTrigs = orTriggers2,
                                                           AndTrigs = andTriggers2,
                                                           Mu1PtMin = 5.*Units.GeV,
                                                           Mu1AbsEtaMax = 999.,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {}, #ptcone20<10 GeV, etcone40<20 GeV
                                                           Mu2PtMin = 5.*Units.GeV,
                                                           Mu2AbsEtaMax = 999.,
                                                           Mu2Types = [0],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = False, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = True,
                                                           InvariantMassLow = 7.0*Units.GeV,
                                                           InvariantMassHigh = 13.*Units.GeV,
                                                           IDTrackThinningConeSize = 0.5,
                                                           BranchPrefix = brPrefix2
                                                           )

ToolSvc += MUON3AugmentTool2
myAugTools.append(MUON3AugmentTool2)
skimmingORs.append(brPrefix2+'DIMU_pass>0')
thinningORs.append(brPrefix2+'DIMU_trkStatus>0')
# thinningORs.append('('+brPrefix2+'DIMU_pass>0&&'+brPrefix2+'DIMU_trkStatus>0)')
print MUON3AugmentTool2

#====================================================================
# SKIMMING
#====================================================================
# skimmingORs.append('EventInfo.eventTypeBitmask==1')
# skimmingORs.insert(0, 'EventInfo.eventTypeBitmask==1')
skimming_expression = '||'.join(skimmingORs)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON3SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "MUON3SkimmingTool1",
                                                                         expression = skimming_expression)
ToolSvc += MUON3SkimmingTool1

#====================================================================
# THINNING
#====================================================================
# keep tracks marked in augmentation
thinning_expression1 = '||'.join(thinningORs)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON3ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON3ThinningTool1",
                                                                ThinningService         = "MUON3ThinningSvc",
                                                                SelectionString         = thinning_expression1,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += MUON3ThinningTool1

# keep tracks around muons
thinning_expression2 = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON3ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON3ThinningTool2",
                                                                    ThinningService         = "MUON3ThinningSvc",
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.5,
                                                                    ApplyAnd                = False,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON3ThinningTool2
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON3Kernel",
                                                                       AugmentationTools = myAugTools,
                                                                       SkimmingTools = [MUON3SkimmingTool1],
                                                                       ThinningTools = [MUON3ThinningTool1, MUON3ThinningTool2]
                                                                       )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON3Stream )
MUON3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON3Stream.AcceptAlgs(["MUON3Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MUON3ThinningSvc", outStreams=[evtStream] )
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON3Stream, 'MUON3')
