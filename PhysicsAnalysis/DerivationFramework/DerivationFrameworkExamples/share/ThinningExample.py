#====================================================================
# ThinningExample.py
# This an example job options script showing how to run thinning in 
# the derivation framework. 
# It requires the reductionConf flag TEST8 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# THINNING TOOLS 
#====================================================================

# Set up your thinning tools (you can have as many as you need). 
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__ThinningToolExample
TEST8ThinningTool = DerivationFramework__ThinningToolExample( name                    = "TEST8ThinningTool",
								ThinningService		= "TEST8ThinningSvc",
								TrackPtCut              = 20000.0)
ToolSvc += TEST8ThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST8Kernel",
									ThinningTools = [TEST8ThinningTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TEST8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST8Stream )
TEST8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST8Stream.AcceptAlgs(["TEST8Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="TEST8ThinningSvc", outStreams=[evtStream] )
#====================================================================
# SLIMMING  
#====================================================================
TEST8Stream.AddItem("xAOD::EventInfo#*")
TEST8Stream.AddItem("xAOD::EventAuxInfo#*")
TEST8Stream.AddItem("xAOD::TrackParticleContainer#*")
TEST8Stream.AddItem("xAOD::TrackParticleAuxContainer#*")
TEST8Stream.AddItem("xAOD::VertexContainer#*")
TEST8Stream.AddItem("xAOD::VertexAuxContainer#*")
