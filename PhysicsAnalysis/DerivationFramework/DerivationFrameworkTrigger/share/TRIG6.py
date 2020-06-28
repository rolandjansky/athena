#********************************************************************
# TRIG6.py
# For removing empty collections from Trigger-Level Analysis AODs
# Modelled on EXOT2.py
# For running on DataScouting stream, disable AODFix since this attempts
# to initialise some BTagging which requires AntiKt4EMTopo jets:
# --preExec 'rec.doApplyAODFix.set_Value_and_Lock(False)'
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

trig6Seq = CfgMgr.AthSequencer("TRIG6Sequence")


#====================================================================
# SET UP STREAM   
#====================================================================

streamName = derivationFlags.WriteDAOD_TRIG6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TRIG6Stream )
TRIG6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TRIG6Stream.AcceptAlgs(["TRIG6Kernel"])


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
# No skimming - keep all events

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += trig6Seq
trig6Seq += CfgMgr.DerivationFramework__DerivationKernel(name = "TRIG6Kernel",
                                                         SkimmingTools = [],
                                                         ThinningTools = [])


#====================================================================
# Add the DS container to the output stream - no slimming done
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRIG6SlimmingHelper = SlimmingHelper("TRIG6SlimmingHelper")

TRIG6SlimmingHelper.AllVariables = [
    "HLT_xAOD__JetContainer_TrigHLTJetDSSelectorCollection",
    ]

TRIG6SlimmingHelper.AppendContentToStream(TRIG6Stream)

