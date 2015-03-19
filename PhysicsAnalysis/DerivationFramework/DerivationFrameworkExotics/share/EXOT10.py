#********************************************************************
# EXOT10.py 
# reductionConf flag EXOT10 in Reco_tf.py   
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
EXOT10MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EXOT10MuonTPThinningTool",
                                                                           ThinningService         = "EXOT10ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           SelectionString         = "Muons.pt > 30*GeV",
                                                                           ConeSize                = 0.4)
ToolSvc += EXOT10MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT10ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT10ElectronTPThinningTool",
                                                                                 ThinningService         = "EXOT10ThinningSvc",
                                                                                 SGKey             	 = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 SelectionString         = "Electrons.pt > 30*GeV",
                                                                                 ConeSize                = 0.4)
ToolSvc += EXOT10ElectronTPThinningTool

# Tracks associated with Photons
EXOT10PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT10PhotonTPThinningTool",
                                                                               ThinningService         = "EXOT10ThinningSvc",
                                                                               SGKey                   = "Photons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "Photons.pt > 30*GeV",
                                                                               ConeSize                = 0.6)
ToolSvc += EXOT10PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
EXOT10PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT10PhotonCCThinningTool",
                                                                                     ThinningService         = "EXOT10ThinningSvc",
                                                                                     SGKey             	     = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Photons.pt > 30*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT10PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT10ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT10ElectronCCThinningTool",
                                                                                     ThinningService         = "EXOT10ThinningSvc",
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 30*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT10ElectronCCThinningTool

#=======================================
# INVARIANT MASS TOOL   
#=======================================
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
#EXOT10PhotonInvariantMassTool = DerivationFramework__InvariantMassTool( name                    	= "EXOT10PhotonInvariantMassTool",
#                                                                        ContainerName               = "Photons",
#                                                                        ObjectRequirements          = "?"
#                                                                        StoreGateEntryName          = "Mass")
#ToolSvc += EXOT10PhotonInvariantMassTool

#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '( ( ((EventInfo.eventTypeBitmask==1) || EF_g35_loose1_g25_loose1) || (EF_g35_medium1_g25_medium1 || EF_g35_medium1_g25_medium1_L12EM15V) ) || EF_2g20_tight1 || EF_2g50_loose ) && ( (count(Photons.pt > 30*GeV) >= 2) || (count(Photons.pt > 30*GeV) > 0 && count(Electrons.pt > 30*GeV) > 0) || (count(Electrons.pt > 30*GeV) >= 2) )'
if (beamEnergy > 6.0e+06):
    expression = '( (EventInfo.eventTypeBitmask == 1) || HLT_g35_loose1_g25_loose1 || HLT_g35_medium1_g25_medium1 || HLT_g35_medium1_g25_medium1_L12EM15V || HLT_2g20_tight1 ||  HLT_2g50_loose ) && ( (count(Photons.pt > 30*GeV) >= 2) || (count(Photons.pt > 30*GeV) > 0 && count(Electrons.pt > 30*GeV) > 0) || (count(Electrons.pt > 30*GeV) >= 2) )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT10SkimmingTool = DerivationFramework__xAODStringSkimmingTool(	name = "EXOT10SkimmingTool1", 
									expression = expression)
ToolSvc += EXOT10SkimmingTool
print EXOT10SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT10Kernel",
									SkimmingTools = [EXOT10SkimmingTool],
									ThinningTools = [EXOT10MuonTPThinningTool, EXOT10ElectronTPThinningTool, EXOT10PhotonTPThinningTool, 
											EXOT10PhotonCCThinningTool, EXOT10ElectronCCThinningTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT10Stream )
EXOT10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT10Stream.AcceptAlgs(["EXOT10Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT10ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT10ContentList import *
EXOT10SlimmingHelper = SlimmingHelper("EXOT10SlimmingHelper")
EXOT10SlimmingHelper.StaticContent = EXOT10Content

EXOT10SlimmingHelper.SmartCollections = EXOT10SmartContent
EXOT10SlimmingHelper.AllVariables = EXOT10AllVariablesContent
EXOT10SlimmingHelper.UserContent = EXOT10Content
EXOT10SlimmingHelper.AppendContentToStream(EXOT10Stream)

