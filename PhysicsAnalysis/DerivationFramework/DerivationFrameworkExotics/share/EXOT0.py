#********************************************************************
# EXOT0.py 
# reductionConf flag EXOT0 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


#====================================================================
# THINNING TOOLS
#====================================================================

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT0TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "EXOT0TPThinningTool",
                                                                ThinningService         = "EXOT0ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += EXOT0TPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT0JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "EXOT0JetTPThinningTool",
                                                                ThinningService         = "EXOT0ThinningSvc",
                                                                JetKey                  = "AntiKt4LCTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += EXOT0JetTPThinningTool

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT0MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT0MuonTPThinningTool",
                                                                            ThinningService         = "EXOT0ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ConeSize                =  0.4)
ToolSvc += EXOT0MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT0ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT0ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT0ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        ConeSize                =  0.4)
ToolSvc += EXOT0ElectronTPThinningTool


#====================================================================
# SKIMMING TOOL 
#====================================================================

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask == 1) || (EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad || EF_g35_loose_g25_loose)'
if (beamEnergy > 6.0e+06):
    expression = '(HLT_2e17_loose1 || HLT_2mu14 || HLT_mu24_mu8noL1)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT0SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT0SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOT0SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT0Kernel",
									SkimmingTools = [EXOT0SkimmingTool],
									ThinningTools = [EXOT0TPThinningTool,EXOT0JetTPThinningTool,EXOT0MuonTPThinningTool,EXOT0ElectronTPThinningTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT0Stream )
EXOT0Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT0Stream.AcceptAlgs(["EXOT0Kernel"])
# Thinning 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT0ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT0ContentList import *
EXOT0SlimmingHelper = SlimmingHelper("EXOT0SlimmingHelper")
EXOT0SlimmingHelper.StaticContent = EXOT0Content
EXOT0SlimmingHelper.AllVariables = EXOT0AllVariables
EXOT0SlimmingHelper.SmartCollections = EXOT0SmartCollections
EXOT0SlimmingHelper.AppendContentToStream(EXOT0Stream)

