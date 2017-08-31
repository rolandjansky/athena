#====================================================================
# MUON2.py for Jpsi->mumu tag&probe
# This requires the reductionConf flag MUON2 in Reco_tf.py   
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
import AthenaCommon.SystemOfUnits as Units

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# enum    MuonType {
#   Combined, MuonStandAlone, SegmentTagged, CaloTagged,
#   SiliconAssociatedForwardMuon
# }
# isolation cuts: check xAODPrimitives/xAODPrimitives/IsolationType.h for definitions
orTriggers_run1 = ['EF_mu6_Trk_Jpsi_loose','EF_2mu4T_Jpsimumu_IDTrkNoCut','EF_mu4Tmu6_Jpsimumu_IDTrkNoCut']
orTriggers_run2 = ['HLT_mu4_bJpsi_Trkloose','HLT_mu6_bJpsi_Trkloose','HLT_mu10_bJpsi_Trkloose','HLT_mu18_bJpsi_Trkloose']
andTriggers_run1 = ['EF_mu24i_tight', 'EF_mu24i_tight_MuonEF', 'EF_mu24i_tight_MG', 'EF_mu24i_tight_l2muonSA']
andTriggers_run2 = ['HLT_mu20_iloose_L1MU15', 'HLT_mu24', 'HLT_mu26', 'HLT_mu24_imedium', 'HLT_mu26_imedium']
orTriggers = orTriggers_run2
andTriggers = andTriggers_run2
# andTriggers = []
brPrefix = 'MUON2'
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__dimuonTaggingTool
MUON2AugmentTool1 = DerivationFramework__dimuonTaggingTool(name = "MUON2AugmentTool1",
                                                           OrTrigs = orTriggers,
                                                           AndTrigs = andTriggers,
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
                                                           BranchPrefix = brPrefix
                                                           )

ToolSvc += MUON2AugmentTool1
print MUON2AugmentTool1


from DerivationFrameworkMuons.TrackIsolationDecorator import MUON2IDTrackDecorator as MUON2AugmentTool2
ToolSvc += MUON2AugmentTool2
print MUON2AugmentTool2

from DerivationFrameworkMuons.TrackIsolationDecorator import MUON2MSTrackDecorator as MUON2AugmentTool3
ToolSvc += MUON2AugmentTool3
print MUON2AugmentTool3

#====================================================================
# SKIMMING
#====================================================================
# expression = 'EventInfo.eventTypeBitmask==1||'+brPrefix+'DIMU_pass>0'
expression = brPrefix+'DIMU_pass>0'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON2SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "MUON2SkimmingTool1",
                                                                         expression = expression)
ToolSvc += MUON2SkimmingTool1

#====================================================================
# THINNING
#====================================================================
# keep tracks marked in augmentation
thinning_expression = brPrefix+"DIMU_trkStatus>0"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON2ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON2ThinningTool1",
                                                                ThinningService         = "MUON2ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += MUON2ThinningTool1

# keep tracks around muons
thinning_expression2 = ""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON2ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON2ThinningTool2",
                                                                    ThinningService         = "MUON2ThinningSvc",
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.5,
                                                                    ApplyAnd                = False,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON2ThinningTool2
<<<<<<< HEAD
=======

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
if not hasattr(DerivationFrameworkJob,"MUONSequence"):
    MUONSeq = CfgMgr.AthSequencer("MUONSequence")

    if not hasattr(MUONSeq,"Muons_decoratePromptLepton"):
        from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets 
        addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=MUONSeq, outputGroup="MUON")
        import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config
        MUONSeq += Config.GetDecoratePromptLeptonAlgs()
    DerivationFrameworkJob += MUONSeq

>>>>>>> Moved muonTP files into derivation framework
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON2Kernel",
                                                                       AugmentationTools = [MUON2AugmentTool1, MUON2AugmentTool2, MUON2AugmentTool3],
                                                                       SkimmingTools = [MUON2SkimmingTool1],
                                                                       ThinningTools = [MUON2ThinningTool1, MUON2ThinningTool2]
                                                                       )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON2Stream )
MUON2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON2Stream.AcceptAlgs(["MUON2Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MUON2ThinningSvc", outStreams=[evtStream] )
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON2Stream, 'MUON2')
