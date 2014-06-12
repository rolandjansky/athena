#====================================================================
# DerivationExampleLoose.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework. It is one 
# of two such examples and is the looser of the two.  
# It requires the reductionConf flag TEST1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================

# Set up your skimming tools (you can have as many as you need). 
# The tools must use the DerivationFrameworkInterfaces as in this example.
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__SkimmingToolExample
SkimmingTool1 = DerivationFramework__SkimmingToolExample(       name                    = "SkimmingTool1",
                                                                MuonContainerKey        = "Muons",
                                                                NumberOfMuons           = 1,
                                                                MuonPtCut               = 1000.0)
ToolSvc += SkimmingTool1

#====================================================================
# THINNING TOOLS
#====================================================================

# Set up your thinning tools (you can have as many as you need).
# The tools must use the DerivationFrameworkInterfaces as in this example.
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__ThinningToolExample
ThinningTool1 = DerivationFramework__ThinningToolExample(       name                    = "ThinningTool1",
                                                                ThinningService         = "DerivationFrameworkStreamThinning",
                                                                PhotonContainerKey      = "PhotonAODCollection",
                                                                PhotonPtCut             = 5000.0)
ToolSvc += ThinningTool1

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("LooseSkimKernel",
                                                                        SkimmingTools = [SkimmingTool1],
									ThinningTools = [ThinningTool1]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_RED_TEST1 here) must match the string in PrimaryDPDFlags
# Fix me - these should be brought inside the framework
streamName = primDPD.WriteDAOD_RED_TEST1Stream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_RED_TEST1Stream )
DerivationFrameworkStream = MSMgr.NewPoolStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
DerivationFrameworkStream.AcceptAlgs(["LooseSkimKernel"])

# THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="DerivationFrameworkStreamThinning", outStreams=[evtStream] )

#====================================================================
# SLIMMING IF ANY (NONE DONE HERE) 
#====================================================================
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList = ['CaloClusterContainer#CaloCalTopoCluster']
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


