#********************************************************************
# EXOT9.py 
# reductionConf flag EXOT9 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT9MuonTPThinningTool",
                                                                            ThinningService         = "EXOT9ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ConeSize                =  0.4)
ToolSvc += EXOT9MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT9ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT9ThinningSvc",
                                                                                        SGKey             	= "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        ConeSize                =  0.4)
ToolSvc += EXOT9ElectronTPThinningTool


#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    triggerStrategy = '(EventInfo.eventTypeBitmask==1) || (EF_xe80_tclcw || EF_xe80_tclcw_loose || EF_e24vhi_medium1 || EF_e60_medium1 || EF_mu24i_tight || EF_mu36_tight || EF_g120_loose)'
if (beamEnergy > 6.0e+06):
    triggerStrategy = '(EventInfo.eventTypeBitmask==1) || (HLT_mu26_imedium || HLT_mu50 || HLT_mu60_msonly_0eta105 || HLT_e28_tight_iloose || HLT_e60_medium || HLT_g140_loose || HLT_xe100 || HLT_g60_loose_xe60 || HLT_e140_loose)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT9SkimmingTool = DerivationFramework__xAODStringSkimmingTool(	name = "EXOT9SkimmingTool1", 
									expression = triggerStrategy)
ToolSvc += EXOT9SkimmingTool
print EXOT9SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	"EXOT9Kernel",
									SkimmingTools = [EXOT9SkimmingTool],
									ThinningTools = [EXOT9MuonTPThinningTool,EXOT9ElectronTPThinningTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT9Stream )
EXOT9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT9Stream.AcceptAlgs(["EXOT9Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT9ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT9ContentList import *
EXOT9SlimmingHelper = SlimmingHelper("EXOT9SlimmingHelper")
EXOT9SlimmingHelper.StaticContent = EXOT9Content
EXOT9SlimmingHelper.AllVariables = EXOT9AllVariables
EXOT9SlimmingHelper.SmartCollections = EXOT9SmartCollections
EXOT9SlimmingHelper.AppendContentToStream(EXOT9Stream)
