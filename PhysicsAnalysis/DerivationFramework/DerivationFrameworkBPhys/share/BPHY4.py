#====================================================================
# BPHY4.py
#====================================================================
#ServiceMgr.MessageSvc.debugLimit=100000000
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY4_VertexTools = BPHYVertexTools("BPHY4")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools 
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__FourMuonTool
BPHY4FourMuonTool = DerivationFramework__FourMuonTool(
  name                        = "BPHY4FourMuonTool",
  OutputLevel                 = INFO,
  ptCut                       = 2500.0,
  etaCut                      = 2.5,
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY4_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY4_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY4_VertexTools.InDetTrackSelectorTool,
  VertexPointEstimator        = BPHY4_VertexTools.VtxPointEstimator)
  
ToolSvc += BPHY4FourMuonTool
print      BPHY4FourMuonTool

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). 

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_4mu
BPHY4_Reco_4mu = DerivationFramework__Reco_4mu(
  name                    = "BPHY4_Reco_4mu",
  OutputLevel             = INFO,
  FourMuonTool            = BPHY4FourMuonTool,
  PairContainerName       = "BPHY4Pairs",
  QuadrupletContainerName = "BPHY4Quads",
  PVContainerName         = "PrimaryVertices",
  RefPVContainerName      = "BPHY4RefittedPrimaryVertices",
  RefitPV                 = True,
  MaxPVrefit              = 100000,
  DoVertexType            = 7)
  
ToolSvc += BPHY4_Reco_4mu
print BPHY4_Reco_4mu

#--------------------------------------------------------------------
## thinning out tracks that are not attached to muons/electrons. The final thinning decision is based on the OR operation
## between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY4MuonTPThinningTool",
                                                                         ThinningService         = "BPHY4ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY4MuonTPThinningTool
BPHY4ThinningTools = [BPHY4MuonTPThinningTool]

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "BPHY4ElectronTPThinningTool",
                                                                               ThinningService         = "BPHY4ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY4ElectronTPThinningTool
BPHY4ThinningTools += [BPHY4ElectronTPThinningTool]

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (BPHY4Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY4Kernel",
   SkimmingTools     = [BPHY4_Reco_4mu],
   ThinningTools     = BPHY4ThinningTools
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY4Stream )
BPHY4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY4Stream.AcceptAlgs(["BPHY4Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY4ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY4SlimmingHelper = SlimmingHelper("BPHY4SlimmingHelper")
BPHY4AllVariables = []
BPHY4SmartVariables = []
BPHY4StaticContent = []

# Needed for trigger objects
BPHY4SlimmingHelper.IncludeMuonTriggerContent = True
BPHY4SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
BPHY4AllVariables += ["PrimaryVertices"]
BPHY4StaticContent += ["xAOD::VertexContainer#BPHY4RefittedPrimaryVertices"]
BPHY4StaticContent += ["xAOD::VertexAuxContainer#BPHY4RefittedPrimaryVerticesAux."]

## ID track particles
BPHY4AllVariables += ["InDetTrackParticles"]
BPHY4SmartVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
BPHY4AllVariables += ["CombinedMuonTrackParticles"]
BPHY4AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
BPHY4AllVariables += ["Muons"]
BPHY4SmartVariables += ["Muons"]

## Electron container
BPHY4SmartVariables += ["Electrons"] 

## Pair/quad candidates 
BPHY4StaticContent += ["xAOD::VertexContainer#%s"        % BPHY4_Reco_4mu.PairContainerName]
BPHY4StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY4_Reco_4mu.PairContainerName]
BPHY4StaticContent += ["xAOD::VertexContainer#%s"        % BPHY4_Reco_4mu.QuadrupletContainerName]
BPHY4StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY4_Reco_4mu.QuadrupletContainerName]

## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY4StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY4_Reco_4mu.PairContainerName]
BPHY4StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY4_Reco_4mu.QuadrupletContainerName]

BPHY4SlimmingHelper.AllVariables = BPHY4AllVariables
BPHY4SlimmingHelper.StaticContent = BPHY4StaticContent
BPHY4SlimmingHelper.SmartCollections = BPHY4SmartVariables
BPHY4SlimmingHelper.AppendContentToStream(BPHY4Stream)
