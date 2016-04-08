#********************************************************************
# EXOT6.py 
# reductionConf flag EXOT6 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

exot6Seq = CfgMgr.AthSequencer("EXOT6Sequence")

#====================================================================
# THINNING TOOL 
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# MET/Jet tracks
#met_thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
#EXOT6MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                = "EXOT6MetTPThinningTool",
#                                                                ThinningService         = "EXOT6ThinningSvc",
#                                                                SelectionString         = met_thinning_expression,
#                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT6MetTPThinningTool
#
#EXOT6JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "EXOT6JetTPThinningTool",
#                                                                ThinningService         = "EXOT6ThinningSvc",
#                                                                JetKey                  = "AntiKt4LCTopoJets",
#                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT6JetTPThinningTool



thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT6TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT6TPThinningTool",
                                                                 ThinningService         = "EXOT6ThinningSvc",
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6TPThinningTool

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT6MuonTPThinningTool",
                                                                            ThinningService         = "EXOT6ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT6ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT6ThinningSvc",
                                                                                        SGKey             	= "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6ElectronTPThinningTool

EXOT6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT6PhotonTPThinningTool",
                                                                                        ThinningService         = "EXOT6ThinningSvc",
                                                                                        SGKey             	= "Photons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT6PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT6PhotonCCThinningTool",
                                                                                     ThinningService         = "EXOT6ThinningSvc",
                                                                                     SGKey                   = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     #SelectionString         = "Photons.pt > 30*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT6PhotonCCThinningTool

#====================================================================
# SKIMMING TOOL 
#====================================================================

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '((EventInfo.eventTypeBitmask==1) || EF_g120_loose || EF_xe80_tclcw_tight) && (count(Photons.pt > 100*GeV) > 0) || (count(Electrons.pt > 100*GeV) > 0)'
if (beamEnergy > 6.0e+06):
    expression = '(HLT_g120_loose || HLT_g140_loose || HLT_xe100) && ((count(Photons.Loose && Photons.pt > 100*GeV) > 0) || (count(Electrons.Medium && Electrons.pt > 100*GeV) > 0))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT6SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT6SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOT6SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot6Seq
exot6Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT6Kernel_skim", SkimmingTools = [EXOT6SkimmingTool])
exot6Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT6Kernel",
									ThinningTools = [EXOT6TPThinningTool,EXOT6MuonTPThinningTool,EXOT6ElectronTPThinningTool, EXOT6PhotonTPThinningTool, EXOT6PhotonCCThinningTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT6Stream )
EXOT6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT6Stream.AcceptAlgs(["EXOT6Kernel"])
# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT6ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT6ContentList import EXOT6SmartContent, EXOT6AllVariablesContent, EXOT6ExtraVariables, EXOT6UnslimmedContent

EXOT6SlimmingHelper = SlimmingHelper("EXOT6SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT6SlimmingHelper.SmartCollections = EXOT6SmartContent

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT6SlimmingHelper.AllVariables = EXOT6AllVariablesContent

# Adding extra missing variables from the smart slimming
EXOT6SlimmingHelper.ExtraVariables = EXOT6ExtraVariables

EXOT6SlimmingHelper.StaticContent = EXOT6UnslimmedContent

EXOT6SlimmingHelper.AppendContentToStream(EXOT6Stream)
