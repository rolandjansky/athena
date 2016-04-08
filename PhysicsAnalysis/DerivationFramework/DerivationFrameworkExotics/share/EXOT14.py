#********************************************************************
# EXOT14.py 
# reductionConf flag EXOT14 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

exot14Seq = CfgMgr.AthSequencer("EXOT14Sequence")

#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EXOT14MuonTPThinningTool",
                                                                           ThinningService         = "EXOT14ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           SelectionString         = "Muons.pt > 8*GeV",
                                                                           ConeSize                = 0.4)
ToolSvc += EXOT14MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT14ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14ElectronTPThinningTool",
                                                                                 ThinningService         = "EXOT14ThinningSvc",
                                                                                 SGKey             	 = "Electrons",
                                                                                 GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 SelectionString         = "Electrons.pt > 8*GeV",
                                                                                 BestMatchOnly           = True,
                                                                                 ConeSize                = 0.4)
ToolSvc += EXOT14ElectronTPThinningTool

# Tracks associated with Photons
EXOT14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14PhotonTPThinningTool",
                                                                               ThinningService         = "EXOT14ThinningSvc",
                                                                               SGKey                   = "Photons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "Photons.pt > 8*GeV",
                                                                               ConeSize                = 0.6)
ToolSvc += EXOT14PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT14PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT14PhotonCCThinningTool",
                                                                                     ThinningService         = "EXOT14ThinningSvc",
                                                                                     SGKey             	     = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Photons.pt > 8*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT14PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT14ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT14ElectronCCThinningTool",
                                                                                     ThinningService         = "EXOT14ThinningSvc",
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 8*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT14ElectronCCThinningTool

if globalflags.DataSource() == 'geant4':
    # MC Thinning
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import \
        DerivationFramework__MenuTruthThinning

    EXOT14MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                       = 'EXOT14MCThinningTool',
        ThinningService            = 'EXOT14ThinningSvc',
        WriteEverything            = False,
        WritePartons               = False,
        PartonPtThresh             = -1.0,
        WriteHadrons               = False,
        WriteBHadrons              = False,
        WriteGeant                 = False,
        GeantPhotonPtThresh        = -1.0,
        WriteTauHad                = True,
        WriteBSM                   = True,
        WriteBosons                = True,
        WriteBSMProducts           = True,
        WriteTopAndDecays          = True,
        WriteAllLeptons            = True,
        #WriteLeptonsNotFromHadrons = True,
        WriteStatus3               = False,
        WriteFirstN                = -1,
        #PreserveDescendants        = False,
        )
    ToolSvc += EXOT14MCThinningTool



ToolSvc += CfgMgr.JetCalibrationTool("m_JESTool",
                                     IsData = (globalflags.DataSource() == 'data'), # "data" or "geant4"
                                     ConfigFile = "JES_Full2012dataset_Preliminary_MC14.config",
                                     CalibSequence = "EtaJES",
                                     JetCollection = "AntiKt4LCTopo")

from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__SkimmingToolEXOT14
EXOT14SkimmingTool = DerivationFramework__SkimmingToolEXOT14(
                                                name = "EXOT14SkimmingTool",
                                                RequireGRL = True,
                                                ReqireLArError = True,
                                                RequireTrigger = False,
                                                RequireJetPts = True,
                                                RequireJetsDEta = True,
                                                RequireDiJetMass = True,
                                                RequireJetsDPhi = False,
                                                DiJetsMass = 5e5,
                                                EtaSeparation = 2.5,
                                                LeadingJetPtCut = 25e3,
                                                SubleadingJetPtCut = 25e3,
                                                Triggers = ["L1_XE60", "L1_XE70", "L1_KF-XE55", "L1_KF-XE60", "L1_KF-XE65", "L1_KF-XE75", "HLT_xe70", "HLT_xe80", "HLT_xe90", "HLT_xe100", "HLT_xe80_tc_lcw", "HLT_xe100_tc_lcw"])

ToolSvc += EXOT14SkimmingTool
print EXOT14SkimmingTool


#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
# expression =  'HLT_xe70'
expression =  '    count(AntiKt4LCTopoJets.pt > 50*GeV) > 0'
expression += ' && count(AntiKt4LCTopoJets.pt > 25*GeV) > 1'
expression += ' && count(AntiKt4LCTopoJets.pt > 25*GeV && AntiKt4LCTopoJets.eta > 0) > 0'
expression += ' && count(AntiKt4LCTopoJets.pt > 25*GeV && AntiKt4LCTopoJets.eta < 0) > 0'
expression += ' && sum( (AntiKt4LCTopoJets.pt > 25*GeV) * abs(AntiKt4LCTopoJets.eta)) > 1'

# from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
# EXOT14StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(	name = "EXOT14StringSkimmingTool", 
# 									expression = expression)
# ToolSvc += EXOT14StringSkimmingTool
# print EXOT14StringSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

#different thinning tools for data/MC
EXOT14ThinningTools=[EXOT14MuonTPThinningTool, EXOT14ElectronTPThinningTool, EXOT14PhotonTPThinningTool,EXOT14PhotonCCThinningTool, EXOT14ElectronCCThinningTool]
if globalflags.DataSource() == 'geant4':
    EXOT14ThinningTools.append(EXOT14MCThinningTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot14Seq
exot14Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT14Kernel_skim", SkimmingTools = [EXOT14SkimmingTool])
exot14Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT14Kernel",
                                                          ThinningTools = EXOT14ThinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT14Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT14Stream )
EXOT14Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT14Stream.AcceptAlgs(["EXOT14Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT14ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT14ContentList import *
EXOT14SlimmingHelper = SlimmingHelper("EXOT14SlimmingHelper")
EXOT14SlimmingHelper.StaticContent = EXOT14Content

EXOT14SlimmingHelper.SmartCollections = EXOT14SmartContent
EXOT14SlimmingHelper.AllVariables = EXOT14AllVariablesContent
EXOT14SlimmingHelper.UserContent = EXOT14Content
EXOT14SlimmingHelper.ExtraVariables = EXOT14ExtraVariablesContent

EXOT14SlimmingHelper.IncludeJetTriggerContent = True
EXOT14SlimmingHelper.IncludeEtMissTriggerContent = True

EXOT14SlimmingHelper.AppendContentToStream(EXOT14Stream)

