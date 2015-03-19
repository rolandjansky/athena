#********************************************************************
# EXOT14.py 
# reductionConf flag EXOT14 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EXOT14MuonTPThinningTool",
                                                                           ThinningService         = "EXOT14ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           SelectionString         = "Muons.pt > 20*GeV",
                                                                           ConeSize                = 0.4)
ToolSvc += EXOT14MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT14ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14ElectronTPThinningTool",
                                                                                 ThinningService         = "EXOT14ThinningSvc",
                                                                                 SGKey             	 = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 SelectionString         = "Electrons.pt > 10*GeV",
                                                                                 ConeSize                = 0.4)
ToolSvc += EXOT14ElectronTPThinningTool

# Tracks associated with Photons
EXOT14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14PhotonTPThinningTool",
                                                                               ThinningService         = "EXOT14ThinningSvc",
                                                                               SGKey                   = "Photons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "Photons.pt > 20*GeV",
                                                                               ConeSize                = 0.6)
ToolSvc += EXOT14PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
EXOT14PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT14PhotonCCThinningTool",
                                                                                     ThinningService         = "EXOT14ThinningSvc",
                                                                                     SGKey             	     = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Photons.pt > 20*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT14PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT14ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT14ElectronCCThinningTool",
                                                                                     ThinningService         = "EXOT14ThinningSvc",
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT14ElectronCCThinningTool


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
                                                DiJetsMass = 7e5,
                                                EtaSeparation = 3.,
                                                LeadingJetPtCut = 60e3,
                                                SubleadingJetPtCut = 40e3,
                                                Triggers = ["HLT_xe70"])

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

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT14Kernel",
									SkimmingTools = [EXOT14SkimmingTool],
									ThinningTools = [EXOT14MuonTPThinningTool, EXOT14ElectronTPThinningTool, EXOT14PhotonTPThinningTool, 
											EXOT14PhotonCCThinningTool, EXOT14ElectronCCThinningTool])

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
EXOT14SlimmingHelper.AppendContentToStream(EXOT14Stream)

