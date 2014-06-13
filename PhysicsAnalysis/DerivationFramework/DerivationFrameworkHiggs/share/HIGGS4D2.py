#********************************************************************
# HIGGS4D2.py 
# reductionConf flag HIGGS4D2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
expression = 'count( (Muons.pt > 13*GeV) && (DFCommonGoodMuon) ) + count((ElectronCollection.pt > 13*GeV) && (DFCommonElectronsLoose == 1)) >= 2'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGGS4D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGGS4D2SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGGS4D2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGGS4D2Kernel",
									SkimmingTools = [HIGGS4D2SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGGS4D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGGS4D2Stream )
HIGGS4D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGGS4D2Stream.AcceptAlgs(["HIGGS4D2Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGGS4D2ContentList import HIGGS4D2Content
HIGGS4D2SlimmingHelper = SlimmingHelper("HIGGS4D2SlimmingHelper")
HIGGS4D2SlimmingHelper.UserContent = HIGGS4D2Content
HIGGS4D2SlimmingHelper.AppendContentToStream(HIGGS4D2Stream)
