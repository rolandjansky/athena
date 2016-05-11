#====================================================================
# L1CALO3.py 
# reductionConf flag L1CALO3 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkInDet.InDetCommon import *
#from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#from DerivationFrameworkEGamma.EGammaCommon import *
from AthenaCommon.GlobalFlags import globalflags

expression = "true"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
L1CALO3StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "L1CALO3StringSkimmingTool", expression = expression)

ToolSvc += L1CALO3StringSkimmingTool
print "L1CALO3.py L1CALO3StringSkimmingTool: ", L1CALO3StringSkimmingTool


#=======================================
# THINNING
#=======================================
thinningTools=[]

## Electrons Track Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
L1CALO3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "L1CALO3ElectronTPThinningTool",
                                                                                ThinningService         = "L1CALO3ThinningSvc",
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += L1CALO3ElectronTPThinningTool
thinningTools.append(L1CALO3ElectronTPThinningTool)
print "L1CALO3.py L1CALO3ElectronTPThinningTool: ", L1CALO3ElectronTPThinningTool


augmentationTools=[]
#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "L1CALO3Kernel",
                                                                        SkimmingTools = [L1CALO3StringSkimmingTool],
                                                                        ThinningTools = thinningTools,
                                                                        AugmentationTools = augmentationTools)
#====================================================================
# SET UP STREAM   
#====================================================================
#streamName = derivationFlags.WriteDAOD_L1CALO3Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_L1CALO3Stream )
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_L1CALO3.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_L1CALO3 )
L1CALO3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
L1CALO3Stream.AcceptAlgs(["L1CALO3Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="L1CALO3ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
L1CALO3SlimmingHelper = SlimmingHelper("L1CALO3SlimmingHelper")


##Smart Slimming
L1CALO3SlimmingHelper.SmartCollections = [
#					  "Electrons",
#					  "Photons",
#					  "TauJets",
#					  "PrimaryVertices",
#					  "MET_Reference_AntiKt4EMTopo",
#					  "AntiKt4EMTopoJets",
					  ]
L1CALO3SlimmingHelper.AllVariables = [
#				      "egammaClusters",
#		                      "CaloCalTopoClusters",
#				      "TauPi0Clusters",
#				      "CMXCPHits",
#				      "CMXCPTobs",
#				      "CMXEtSums",
#				      "CMXJetHits",
#				      "CMXJetTobs",
#				      "CMXRoIs",
				      
#				      "CPMTobRoIs",
#				      "CPMTobRoIsRoIB",
#				      "CPMTowers",
#				      "CPMTowersOverlap",
#				      "JEMEtSums",
#				      "JEMTobRoIs",
#				      "JEMTobRoIsRoIB",
				      
#				      "JetElements",
#				      "JetElementsOverlap",
#				      "LVL1EmTauRoIs",
#				      "LVL1EnergySumRoI",
#				      "LVL1JetEtRoI",
#				      "LVL1JetRoIs",
#				      "RODHeaders",
				      
#				      "xAODTriggerTowers",
				      
				      ]

#L1CALO3SlimmingHelper.IncludeMuonTriggerContent   = True
#L1CALO3SlimmingHelper.IncludeEGammaTriggerContent = True
#L1CALO3SlimmingHelper.IncludeJetTriggerContent    = True
#L1CALO3SlimmingHelper.IncludeTauTriggerContent    = True
#L1CALO3SlimmingHelper.IncludeEtMissTriggerContent = True
#L1CALO3SlimmingHelper.IncludeBJetTriggerContent = True
#L1CALO3SlimmingHelper.IncludeBPhysTriggerContent = True
#L1CALO3SlimmingHelper.IncludeMinBiasTriggerContent = True


L1CALO3SlimmingHelper.AppendContentToStream(L1CALO3Stream)



