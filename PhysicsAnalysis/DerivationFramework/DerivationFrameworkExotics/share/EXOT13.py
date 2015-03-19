#********************************************************************
# EXOT13.py 
# reductionConf flag EXOT13 in Reco_tf.py   
#********************************************************************
#from JetRec.JetRecStandard import jtm
#jtm.modifiersMap["groomed"] += [jtm.ktsplitter] # to add substructure to trimmed jets

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *



#=======================================
# SKIMMING   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || EF_3mu6_MSonly || EF_2mu4_mu11_MSonly || EF_g35_medium_g25 || EF_2mu10_MSonly_g12_loose || EF_2mu14'
if (beamEnergy > 6.0e+06):
    expression = 'HLT_3mu6 || HLT_mu20_imedium_2mu4noL1 || HLT_g35_medium1_g25_medium1 || HLT_g12_loose1_2mu10_msonly || HLT_2mu14'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT13StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT13StringSkimmingTool",
                                                                         expression = expression)

ToolSvc += EXOT13StringSkimmingTool
print EXOT13StringSkimmingTool

# Tracks associated with Muons
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
#EXOT13MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT13MuonTPThinningTool",
#                                                                            ThinningService         = "EXOT13ThinningSvc",
#                                                                            MuonKey                 = "Muons",
#                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                            SelectionString = "Muons.pt > 15*GeV")
#ToolSvc += EXOT13MuonTPThinningTool

# Tracks associated with Electrons
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
#EXOT13ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT13ElectronTPThinningTool",
#                                                                                        ThinningService         = "EXOT13ThinningSvc",
#                                                                                        SGKey                   = "Electrons",
#                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",
#                                                                                        SelectionString         = "Electrons.pt > 15*GeV"
#                                                                              )
#ToolSvc += EXOT13ElectronTPThinningTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	"EXOT13Kernel",
									SkimmingTools = [EXOT13StringSkimmingTool])
 #                                                                       ThinningTools = [EXOT13ElectronTPThinningTool,EXOT13MuonTPThinningTool])


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT13Stream )
EXOT13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT13Stream.AcceptAlgs(["EXOT13Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT13ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT13
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT13ContentList import *
EXOT13SlimmingHelper = SlimmingHelper("EXOT13SlimmingHelper")
EXOT13SlimmingHelper.SmartCollections = EXOT13SmartContent 
EXOT13SlimmingHelper.AllVariables = EXOT13AllVariablesContent 
EXOT13SlimmingHelper.StaticContent = EXOT13UnslimmedContent 
EXOT13SlimmingHelper.AppendContentToStream(EXOT13Stream)


