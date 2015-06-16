#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMCTruth.MCTruthCommon import * 

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH0Kernel")

# Now we add the sequencer to the job
#DerivationFrameworkJob += EvgenAODSequence

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH0Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH0Stream )
TRUTH0Stream = MSMgr.NewPoolRootStream( streamName, fileName )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH0Stream.AcceptAlgs(['TRUTH0Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================
TRUTH0Stream.AddItem("xAOD::EventInfo#*")
TRUTH0Stream.AddItem("xAOD::EventAuxInfo#*")
TRUTH0Stream.AddItem( "xAOD::TruthEventContainer#*" )
TRUTH0Stream.AddItem( "xAOD::TruthEventAuxContainer#*" )
TRUTH0Stream.AddItem( "xAOD::TruthVertexContainer#*" )
TRUTH0Stream.AddItem( "xAOD::TruthVertexAuxContainer#*" )
TRUTH0Stream.AddItem( "xAOD::TruthParticleContainer#*" )
TRUTH0Stream.AddItem( "xAOD::TruthParticleAuxContainer#*" )
