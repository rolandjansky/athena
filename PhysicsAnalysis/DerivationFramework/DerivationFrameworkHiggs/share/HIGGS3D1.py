#********************************************************************
# HIGGS3D1.py 
# reductionConf flag HIGGS3D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#====================================================================
# SKIMMING TOOL 
#====================================================================
# Tools required to make this derivation work:
# (1) track extrapolation for electron tracks
# Placeholder skim just to make it run:
electronRequirements = '(ElectronCollection.pt > 7*GeV) && (abs(ElectronCollection.eta) < 2.6) && ((ElectronCollection.author == 1) || (ElectronCollection.author == 3))'
muonsRequirements = '(Muons.pt > 7*GeV) && (abs(Muons.eta) < 2.6) && (Muons.quality == 2)'
eeSelection = 'count('+electronRequirements+') >= 2'
mmSelection = '(count('+muonsRequirements+') >= 2)'
emSelection = '(count('+electronRequirements+') >= 1) && (count('+muonsRequirements+') >= 1)'
expression = eeSelection+' || '+mmSelection+' || '+emSelection
#electronRequirements = '(el_pt > 7*GeV) && (abs(el_eta) < 2.6) && ((el_author == 1) || (el_author == 3)) && (abs(el_trackz0pv * sin(el_tracktheta)) < 2.0) && (abs(el_trackd0pv / el_tracksigd0pv) < 15.0)'
#muonsRequirements = '(mu_pt > 7*GeV) && (abs(mu_eta) < 2.6) && (mu_loose == 1) && (abs(mu_id_z0_exPV * sin(mu_theta_exPV)) < 4.0)' 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGGS3D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGGS3D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGGS3D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGGS3D1Kernel",
									SkimmingTools = [HIGGS3D1SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGGS3D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGGS3D1Stream )
HIGGS3D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGGS3D1Stream.AcceptAlgs(["HIGGS3D1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGGS3D1ContentList import HIGGS3D1Content
HIGGS3D1SlimmingHelper = SlimmingHelper("HIGGS3D1SlimmingHelper")
HIGGS3D1SlimmingHelper.UserContent = HIGGS3D1Content
HIGGS3D1SlimmingHelper.AppendContentToStream(HIGGS3D1Stream)
