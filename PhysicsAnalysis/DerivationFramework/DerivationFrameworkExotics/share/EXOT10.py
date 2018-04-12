#********************************************************************
# EXOT10.py
# reductionConf flag EXOT10 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot10Seq = CfgMgr.AthSequencer("EXOT10Sequence")

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT10Stream )
EXOT10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT10Stream.AcceptAlgs(["EXOT10Kernel"])

#====================================================================
# THINNING
#====================================================================

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT10ThinningHelper = ThinningHelper( "EXOT10ThinningHelper" )
EXOT10ThinningHelper.TriggerChains = 'HLT_2g20_loose | HLT_2g50_loose | HLT_g35_medium_g25_medium'
EXOT10ThinningHelper.AppendToStream( EXOT10Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

EXOT10TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT10TPThinningTool",
								 ThinningService	 = EXOT10ThinningHelper.ThinningSvc(),
								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
								 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT10TPThinningTool

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT10MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EXOT10MuonTPThinningTool",
		                                                                   ThinningService         = EXOT10ThinningHelper.ThinningSvc(),
		                                                                   MuonKey                 = "Muons",
		                                                                   InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ConeSize                = 0.4)

#ToolSvc += EXOT10MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT10ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT10ElectronTPThinningTool",
                                                                                 ThinningService         = EXOT10ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	 = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 #SelectionString         = "Electrons.pt > 30*GeV",
                                                                                 ConeSize                = 0.4)
#ToolSvc += EXOT10ElectronTPThinningTool

# Tracks associated with Photons
EXOT10PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT10PhotonTPThinningTool",
                                                                               ThinningService         = EXOT10ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Photons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               #SelectionString         = "Photons.pt > 30*GeV",
                                                                               ConeSize                = 0.6)
#ToolSvc += EXOT10PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT10PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT10PhotonCCThinningTool",
                                                                                     ThinningService         = EXOT10ThinningHelper.ThinningSvc(),
                                                                                     SGKey             	     = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     #SelectionString         = "Photons.pt > 30*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT10PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT10ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT10ElectronCCThinningTool",
                                                                                     ThinningService         = EXOT10ThinningHelper.ThinningSvc(),
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     #SelectionString         = "Electrons.pt > 30*GeV",
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
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
expression = '( (EventInfo.eventTypeBitmask == 1) || HLT_g35_loose_g25_loose || HLT_g35_medium_g25_medium || HLT_g35_medium_g25_medium_L12EM15V || HLT_2g20_tight ||  HLT_2g50_loose ) && ( (count(Photons.pt > 30*GeV) >= 2) || (count(Photons.pt > 30*GeV) > 0 && count(Electrons.pt > 30*GeV) > 0) || (count(Electrons.pt > 30*GeV) >= 2) )'
EXOT10SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT10SkimmingTool1", expression = expression)
ToolSvc += EXOT10SkimmingTool
print EXOT10SkimmingTool

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT10"] = []
reducedJetList = [
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot10Seq,"EXOT10")

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EXOT10_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EXOT10_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EXOT10_MaxCellDecoratorTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot10Seq
exot10Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT10Kernel_skim", SkimmingTools = [EXOT10SkimmingTool])
exot10Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT10Kernel",
                                                        AugmentationTools = [EXOT10_MaxCellDecoratorTool],
							ThinningTools = [EXOT10PhotonCCThinningTool, EXOT10ElectronCCThinningTool])

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
EXOT10SlimmingHelper.ExtraVariables = EXOT10ExtraVariables
EXOT10SlimmingHelper.IncludeEGammaTriggerContent = True
addMETOutputs(EXOT10SlimmingHelper, ["EXOT10"])

EXOT10SlimmingHelper.AppendContentToStream(EXOT10Stream)
