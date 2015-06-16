#====================================================================
# MCTEST3.py
# These job options are to enable groups to quickly set up tests of
# truth dressing in the derivation framework. 
# It requires the reductionConf flag MCTEST3 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# DRESSING TOOL
#====================================================================

# Set up your dressing tool
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool
MCTEST3Tool = DerivationFramework__TruthDressingTool(name = "MCTEST3Tool",
						     usePhotonsFromHadrons	 	= False,
                 dressingConeSize = 0.1,
                 particleIDsToDress = [11,13]
						     )
ToolSvc += MCTEST3Tool

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
MCTEST3Tool2 = DerivationFramework__TruthIsolationTool(name = "MCTEST3ToolIso1",
                 IsolationConeSize = 0.2,
                 IsolationVarName = 'EtCone20',
                 ChargedParticlesOnly = False
						     )
ToolSvc += MCTEST3Tool2
MCTEST3Tool3 = DerivationFramework__TruthIsolationTool(name = "MCTEST3ToolIso2",
                 IsolationConeSize = 0.3,
                 IsolationVarName = 'PtCone30',
                 ChargedParticlesOnly = True
						     )
ToolSvc += MCTEST3Tool3

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MCTEST3Kernel",
                                                                        AugmentationTools = [MCTEST3Tool,
                                                                          MCTEST3Tool2, MCTEST3Tool3])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MCTEST3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MCTEST3Stream )
MCTEST3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MCTEST3Stream.AcceptAlgs(["MCTEST3Kernel"])

#====================================================================
# SLIMMING  
#====================================================================
MCTEST3Stream.AddItem("xAOD::EventInfo_v1#*")
MCTEST3Stream.AddItem("xAOD::EventAuxInfo_v1#*")
MCTEST3Stream.AddItem("xAOD::ElectronContainer_v1#ElectronCollection")
MCTEST3Stream.AddItem("xAOD::ElectronAuxContainer#ElectronCollectionAux.")
MCTEST3Stream.AddItem("xAOD::MuonContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::MuonAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TauJetContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TauJetAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::JetContainer_v1#AntiKt4LCTopoJets")
MCTEST3Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.")
MCTEST3Stream.AddItem("xAOD::JetContainer_v1#AntiKt4TruthJets")
MCTEST3Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4TruthJetsAux.")
MCTEST3Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4LCTopo")
MCTEST3Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4LCTopoAux.")
MCTEST3Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4Truth")
MCTEST3Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4TruthAux.")
MCTEST3Stream.AddItem("xAOD::TrackParticleContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TrackParticleAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::CaloClusterContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::CaloClusterAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::MissingETComponentMap_v1#*")
MCTEST3Stream.AddItem("xAOD::MissingETAuxComponentMap_v1#*")
MCTEST3Stream.AddItem("xAOD::MissingETContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::MissingETAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::VertexContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::VertexAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthVertexContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthVertexAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthParticleContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthParticleAuxContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthEventContainer_v1#*")
MCTEST3Stream.AddItem("xAOD::TruthEventAuxContainer_v1#*")
