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

#from MuonPerformanceAlgs.CommonMuonTPConfig import GetIDTrackCaloDepositsDecorator
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
MUON1Stream = MSMgr.NewPoolRootStream(derivationFlags.WriteDAOD_MUON1Stream.StreamName, buildFileName( derivationFlags.WriteDAOD_MUON1Stream))
MUON1Stream.AcceptAlgs(["MUON1Kernel"])

### trigger thinning
triggerList1 = ['HLT_.*mu\d+.*']

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
MUON1ThinningHelper = ThinningHelper( "MUON1ThinningHelper" )
MUON1ThinningHelper.TriggerChains = '|'.join(triggerList1)
print MUON1ThinningHelper.TriggerChains
MUON1ThinningHelper.AppendToStream( MUON1Stream )

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
MUON1AugmentTools = []

skimmingORs = []
thinningORs = []

### Z->mumu events
brPrefix1a = 'MUON1a'
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
MUON1AugmentTool1a = DerivationFramework__dimuonTaggingTool(name = "MUON1AugmentTool1a",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
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
                                                           BranchPrefix = brPrefix1a
                                                           )

ToolSvc += MUON1AugmentTool1a
MUON1AugmentTools.append(MUON1AugmentTool1a)
skimmingORs.append(brPrefix1a+'DIMU_pass>0')
thinningORs.append(brPrefix1a+'DIMU_trkStatus>0')
print MUON1AugmentTool1a


### Jpsi for tag-probe
andTriggers1b = ['HLT_mu20_iloose_L1MU15', 'HLT_mu24', 'HLT_mu26', 'HLT_mu24_imedium', 'HLT_mu26_imedium']
orTriggers1b = ['HLT_mu4','HLT_mu6','HLT_mu14','HLT_mu6_idperf','HLT_mu4_bJpsi_Trkloose','HLT_mu6_bJpsi_Trkloose ','HLT_mu10_bJpsi_Trkloose','HLT_mu18_bJpsi_Trkloose','HLT_mu20_2mu0noL1_JpsimumuFS','HLT_mu18_2mu0noL1_JpsimumuFS','HLT_mu20_2mu4_JpsimumuL2','HLT_mu18_2mu4_JpsimumuL2','HLT_mu4_mu4_idperf_bJpsimumu_noid']

brPrefix1b = 'MUON1b'
MUON1AugmentTool1b = DerivationFramework__dimuonTaggingTool(name = "MUON1AugmentTool1b",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
                                                           OrTrigs = orTriggers1b,
                                                           AndTrigs = andTriggers1b,
                                                           Mu1PtMin = 4*Units.GeV,
                                                           Mu1AbsEtaMax = 2.5,
                                                           Mu1Types = [0],
                                                           Mu1Trigs = [],
                                                           Mu1IsoCuts = {},
                                                           Mu2PtMin = 2.5*Units.GeV,
                                                           Mu2AbsEtaMax = 9999.,
                                                           Mu2Types = [],
                                                           Mu2Trigs = [],
                                                           Mu2IsoCuts = {},
                                                           UseTrackProbe = True, # bool
                                                           TrackContainerKey = 'InDetTrackParticles', # str
                                                           OppositeCharge = False,
                                                           InvariantMassLow = 2.0*Units.GeV,
                                                           InvariantMassHigh = 4.8*Units.GeV,
                                                           IDTrackThinningConeSize = 0.5,
                                                           BranchPrefix = brPrefix1b
                                                          )

ToolSvc += MUON1AugmentTool1b
MUON1AugmentTools.append(MUON1AugmentTool1b)
skimmingORs.append(brPrefix1b+'DIMU_pass>0')
thinningORs.append(brPrefix1b+'DIMU_trkStatus>0')
print MUON1AugmentTool1b


### Jpsi for calibration
brPrefix1c = 'MUON1c'
orTriggers1c = []
andTriggers1c = []

MUON1AugmentTool1c = DerivationFramework__dimuonTaggingTool(name = "MUON1AugmentTool1c",
                                                           IDTrackCaloDepoDecoTool = ToolSvc.IDTrackCaloDepositsDecoratorTool,
                                                           OrTrigs = orTriggers1c,
                                                           AndTrigs = andTriggers1c,
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
                                                           BranchPrefix = brPrefix1c
                                                           )

ToolSvc += MUON1AugmentTool1c
MUON1AugmentTools.append(MUON1AugmentTool1c)
skimmingORs.append(brPrefix1c+'DIMU_pass>0')
thinningORs.append(brPrefix1c+'DIMU_trkStatus>0')
print MUON1AugmentTool1c

### Upsilon tagging
brPrefix1d = 'MUON1d'
orTriggers1d = []
andTriggers1d = [] # No trigger in 8TeV data

MUON1AugmentTool1d = DerivationFramework__dimuonTaggingTool(name = "MUON1AugmentTool1d",
                                                           OrTrigs = orTriggers1d,
                                                           AndTrigs = andTriggers1d,
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
                                                           BranchPrefix = brPrefix1d
                                                           )

ToolSvc += MUON1AugmentTool1d
MUON1AugmentTools.append(MUON1AugmentTool1d)
skimmingORs.append(brPrefix1d+'DIMU_pass>0')
thinningORs.append(brPrefix1d+'DIMU_trkStatus>0')
print MUON1AugmentTool1d

### isolation decorations
from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1IDTrackDecorator as MUON1AugmentTool2a
MUON1AugmentTool2a.SelectionString = "(InDetTrackParticles.MUON1aDIMU_Status==1000||InDetTrackParticles.MUON1bDIMU_Status==1000)"
MUON1AugmentTool2a.SelectionFlag = ""
MUON1AugmentTool2a.SelectionFlagValue = 0

ToolSvc += MUON1AugmentTool2a
MUON1AugmentTools.append(MUON1AugmentTool2a)
print MUON1AugmentTool2a

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON1MSTrackDecorator as MUON1AugmentTool3
ToolSvc += MUON1AugmentTool3
MUON1AugmentTools.append(MUON1AugmentTool3)
print MUON1AugmentTool3

#====================================================================
# SKIMMING
#====================================================================
skimming_expression = '||'.join(skimmingORs)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON1SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "MUON1SkimmingTool1",
                                                                         expression = skimming_expression)
ToolSvc += MUON1SkimmingTool1

#====================================================================
# THINNING
#====================================================================
MUON1ThinningTools = []

# keep tracks marked in augmentation
thinning_expression1 = '||'.join(thinningORs)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON1ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON1ThinningTool1",
                                                                ThinningService         = MUON1ThinningHelper.ThinningSvc(),
                                                                SelectionString         = thinning_expression1,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += MUON1ThinningTool1
MUON1ThinningTools.append(MUON1ThinningTool1)

# keep tracks around muons
thinning_expression2 = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON1ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON1ThinningTool2",
                                                                    ThinningService         = MUON1ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.5,
                                                                    ApplyAnd                = False,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON1ThinningTool2
MUON1ThinningTools.append(MUON1ThinningTool2)


# keep topoclusters around muons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
MUON1ThinningTool4 = DerivationFramework__CaloClusterThinning(name                    = "MUON1ThinningTool4",
                                                              ThinningService         = MUON1ThinningHelper.ThinningSvc(),
                                                              SGKey                   = "Muons",
                                                              SelectionString         = "Muons.pt>4*GeV",
                                                              TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                              ConeSize                = 0.5)
ToolSvc += MUON1ThinningTool4
print MUON1ThinningTool4
MUON1ThinningTools.append(MUON1ThinningTool4)

### also for forward tracks
thinning_expression3 = "Muons.muonType==4"
MUON1ThinningTool2f = DerivationFramework__MuonTrackParticleThinning(name                   = "MUON1ThinningTool2f",
                                                                    ThinningService         = MUON1ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression3,
                                                                    ConeSize                = 0.5,
                                                                    ApplyAnd                = False,
                                                                    InDetTrackParticlesKey  = "InDetForwardTrackParticles")
ToolSvc += MUON1ThinningTool2f
MUON1ThinningTools.append(MUON1ThinningTool2f)

### cell thinning
from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["MuonClusterCollection"],
                                       outputCellKey="DFMUONCellContainer")
#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
if not hasattr(DerivationFrameworkJob,"MUONSequence"):
    MUONSeq = CfgMgr.AthSequencer("MUONSequence")

    if not hasattr(MUONSeq,"Muons_decoratePromptLepton"):
        JetTagConfig.ConfigureAntiKt4PV0TrackJets(MUONSeq,"MUON1")
        MUONSeq += JetTagConfig.GetDecoratePromptLeptonAlgs()
    DerivationFrameworkJob += MUONSeq

from DerivationFrameworkMuons import  JPsiVertexFitSetup
MUON1AugmentTools += JPsiVertexFitSetup.AddJPsiVertexingAlgs('MUON1',False)
for t in MUON1AugmentTools: print t

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
MUON1Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "MUON1Thin_vtxTrk",
  ThinningService            = MUON1ThinningHelper.ThinningSvc(),
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["MUON1JpsiCandidates"],
  PassFlags                  = ["passed_Jpsi"] )

ToolSvc += MUON1Thin_vtxTrk
MUON1ThinningTools.append(MUON1Thin_vtxTrk)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON1Kernel",
                                                                       AugmentationTools = MUON1AugmentTools,
                                                                       SkimmingTools = [MUON1SkimmingTool1],
                                                                       ThinningTools = MUON1ThinningTools
                                                                       )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
if hasattr(ToolSvc,"MUON1MuonTP_Reco_mumu"):
    conf.Items['MUON1']+=["xAOD::VertexContainer#%s"        % ToolSvc.MUON1MuonTP_Reco_mumu.OutputVtxContainerName]
    conf.Items['MUON1']+=["xAOD::VertexAuxContainer#%sAux." % ToolSvc.MUON1MuonTP_Reco_mumu.OutputVtxContainerName]
    conf.Items['MUON1']+=["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % ToolSvc.MUON1MuonTP_Reco_mumu.OutputVtxContainerName]

conf.Items['MUON1']+=["CaloCellContainer#DFMUONCellContainer"]
conf.Items['MUON1']+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]
conf.extraVariables['MUON1'] += JetTagConfig.GetExtraPromptVariablesForDxAOD()
conf.Config(MUON1Stream, 'MUON1')
