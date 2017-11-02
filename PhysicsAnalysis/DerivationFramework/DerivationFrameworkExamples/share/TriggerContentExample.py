from AthenaCommon.Constants import *

#====================================================================
# TriggerContentExample.py
# This example demonstrates how to include trigger information in
# the derivations and how to thin the trigger navigation container
# which if left to its own devices occupies 22 KB/event..
# It requires the reductionConf flag TEST10 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TEST10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST10Stream )
TEST10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST10Stream.AcceptAlgs(["TEST10Kernel"])

#====================================================================
# THINNING  
#====================================================================
# Establish the thinning helper (which will set up the services behind the scenes)
# Plug in the required triggers
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TEST10ThinningHelper = ThinningHelper( "TEST10ThinningHelper" )
TEST10ThinningHelper.TriggerChains = '^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*'
TEST10ThinningHelper.AppendToStream( TEST10Stream )


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (TEST10Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST10Kernel")
TEST10Stream.AcceptAlgs(["TEST10Kernel"])

#====================================================================
# CONTENT LIST  
#====================================================================
# Add the required contents (offline) 
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TEST10SlimmingHelper = SlimmingHelper("TEST10SlimmingHelper")
TEST10SlimmingHelper.SmartCollections = ["Electrons",
                                         "Muons"
                                         ]
# Add the required trigger slices
TEST10SlimmingHelper.IncludeMuonTriggerContent = True
TEST10SlimmingHelper.IncludeEGammaTriggerContent = True
#TEST10SlimmingHelper.IncludeBPhysTriggerContent = True
##JetTauEtMissTriggerContent: split in 4 slices
#TEST10SlimmingHelper.IncludeJetTriggerContent = True
#TEST10SlimmingHelper.IncludeEtMissTriggerContent = True
#TEST10SlimmingHelper.IncludeTauTriggerContent = True
#TEST10SlimmingHelper.IncludeBJetTriggerContent = True
#TEST10SlimmingHelper.IncludeMinBiasTriggerContent = True
TEST10SlimmingHelper.AppendContentToStream(TEST10Stream)
