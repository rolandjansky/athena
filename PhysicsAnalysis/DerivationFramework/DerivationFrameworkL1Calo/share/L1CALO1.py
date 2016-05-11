#====================================================================
# L1CALO1.py 
# reductionConf flag L1CALO1 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkInDet.InDetCommon import *
#from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#from DerivationFrameworkEGamma.EGammaCommon import *
from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# SET UP STREAM   
#====================================================================
#streamName = derivationFlags.WriteDAOD_L1CALO1Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_L1CALO1Stream )
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_L1CALO1.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_L1CALO1 )
L1CALO1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
L1CALO1Stream.AcceptAlgs(["L1CALO1Kernel"])


from DerivationFrameworkCore.ThinningHelper import ThinningHelper
L1CALO1ThinningHelper = ThinningHelper( "L1CALO1ThinningHelper" )
L1CALO1ThinningHelper.AppendToStream( L1CALO1Stream )

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="L1CALO1ThinningSvc", outStreams=[evtStream] )

##============================================================================
##
##============================================================================
thinningTools=[]

#=======================================
# THINNING
#=======================================

from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import DerivationFramework__TriggerTowerThinningAlg
L1CALO1CaloThinningTool = DerivationFramework__TriggerTowerThinningAlg( name = "L1CaloThinning",
									ThinService = L1CALO1ThinningHelper.ThinningSvc(),
    									TriggerTowerLocation = "xAODTriggerTowers",
    									MinCaloCellEnergy = 0.8,
    									MinADC = 36,
    									UseRandom = True,
    									MinRandom = 0.01)
ToolSvc += L1CALO1CaloThinningTool
thinningTools.append(L1CALO1CaloThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "L1CALO1Kernel",
                                                                        SkimmingTools =[],
                                                                        ThinningTools = thinningTools,
									AugmentationTools =  [],
									)


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
L1CALO1SlimmingHelper = SlimmingHelper("L1CALO1SlimmingHelper")


##Smart Slimming
L1CALO1SlimmingHelper.SmartCollections = ["Electrons",
					  "Photons",
					  "TauJets",
					  "PrimaryVertices",
					  "MET_Reference_AntiKt4EMTopo",
					  "AntiKt4EMTopoJets",
					  ]
L1CALO1SlimmingHelper.AllVariables = ["egammaClusters",
		                      "CaloCalTopoClusters",
				      "TauPi0Clusters",
				      "CMXCPHits",
				      "CMXCPTobs",
				      "CMXEtSums",
				      "CMXJetHits",
				      "CMXJetTobs",
				      "CMXRoIs",
				      
				      "CPMTobRoIs",
				      "CPMTobRoIsRoIB",
				      "CPMTowers",
				      "CPMTowersOverlap",
				      "JEMEtSums",
				      "JEMTobRoIs",
				      "JEMTobRoIsRoIB",
				      
				      "JetElements",
				      "JetElementsOverlap",
				      "LVL1EmTauRoIs",
				      "LVL1EnergySumRoIs",
				      "LVL1JetEtRoIs",
				      "LVL1JetRoIs",
				      "LVL1MuonRoIs",
				      "RODHeaders",
				      
				      "xAODTriggerTowers",

				      "L1TopoRawData",
				      
				      ]
L1CALO1SlimmingHelper.IncludeMuonTriggerContent   = True
L1CALO1SlimmingHelper.IncludeEGammaTriggerContent = True
L1CALO1SlimmingHelper.IncludeJetTriggerContent    = True
L1CALO1SlimmingHelper.IncludeTauTriggerContent    = True
L1CALO1SlimmingHelper.IncludeEtMissTriggerContent = True
L1CALO1SlimmingHelper.IncludeBJetTriggerContent = True
L1CALO1SlimmingHelper.IncludeBPhysTriggerContent = True
L1CALO1SlimmingHelper.IncludeMinBiasTriggerContent = True

L1CALO1SlimmingHelper.AppendContentToStream(L1CALO1Stream)




