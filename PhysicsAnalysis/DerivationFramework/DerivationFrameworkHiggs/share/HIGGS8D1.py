#********************************************************************
# HIGGS8D1.py 
# reductionConf flag HIGGS8D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# INVARIANT MASS TOOL 
#====================================================================
# Needed to make this derivation work: invariant mass tool

requirement = 'PhotonCollection.pt > 18*GeV'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
HIGGS8D1DiPhotonTool = DerivationFramework__InvariantMassTool( name = "HIGGS8D1DiPhotonTool",
                                                               ObjectRequirements = requirement,
							       StoreGateEntryName = "DiPhotonMass",
							       MassHypothesis = 0.0,
							       ContainerName = "PhotonCollection" )
ToolSvc += HIGGS8D1DiPhotonTool

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = 'count(DiPhotonMass > 90.0*GeV) >= 1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGGS8D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGGS8D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGGS8D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGGS8D1Kernel",
									AugmentationTools = [HIGGS8D1DiPhotonTool],
									SkimmingTools = [HIGGS8D1SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGGS8D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGGS8D1Stream )
HIGGS8D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGGS8D1Stream.AcceptAlgs(["HIGGS8D1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGGS8D1ContentList import HIGGS8D1Content
HIGGS8D1SlimmingHelper = SlimmingHelper("HIGGS8D1SlimmingHelper")
HIGGS8D1SlimmingHelper.UserContent = HIGGS8D1Content
HIGGS8D1SlimmingHelper.AppendContentToStream(HIGGS8D1Stream)
