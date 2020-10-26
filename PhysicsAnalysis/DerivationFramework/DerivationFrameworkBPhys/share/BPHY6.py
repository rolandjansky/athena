#====================================================================
# BPHY6.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag BPHY6 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
isSimulation = DerivationFrameworkHasTruth

print isSimulation
#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY6_VertexTools = BPHYVertexTools("BPHY6")


# General Variables
dimuon_chi2_max = 50.
dimuon_mass_min = 100.
dimuon_mass_max = 150e3

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__MuonExtrapolationTool
BPHY6_Extrap_Tool = DerivationFramework__MuonExtrapolationTool(
  name = "BPHY6_ExtrapolationTool",
  OutputLevel = INFO )
ToolSvc += BPHY6_Extrap_Tool
 


#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY6JpsiFinder = Analysis__JpsiFinder(
  name                        = "BPHY6JpsiFinder",
  OutputLevel                 = INFO,
  muAndMu                     = True,
  muAndTrack                  = False,
  TrackAndTrack               = False,
  assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = dimuon_mass_max,
  invMassLower                = dimuon_mass_min,
  Chi2Cut                     = dimuon_chi2_max,
  oppChargesOnly	          = True,
  atLeastOneComb              = True,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY6_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY6_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY6_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = BPHY6_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = BPHY6_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False )
  
ToolSvc += BPHY6JpsiFinder
print      BPHY6JpsiFinder


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu



BPHY6_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "BPHY6_Reco_mumu",
  JpsiFinder             = BPHY6JpsiFinder,
  OutputVtxContainerName = "BPHY6OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY6RefittedPrimaryVertices")
  
ToolSvc += BPHY6_Reco_mumu
print BPHY6_Reco_mumu


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY6_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2700.0,
  MassMax               = 3500.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Jpsi2mumu
print BPHY6_Select_Jpsi2mumu


## b/ augment and select Psi(2S)->mumu candidates
BPHY6_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Psi2mumu",
  HypothesisName        = "Psi",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 3686.09,
  MassMin               = 3200.0,
  MassMax               = 4200.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Psi2mumu
print BPHY6_Select_Psi2mumu

# Added by ASC
## c/ augment and select Upsilon(nS)->mumu candidates
BPHY6_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Upsi2mumu",
  HypothesisName        = "Upsi",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 9460.30,
  MassMin               = 8000.0,
  MassMax               = 12000.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Upsi2mumu
print BPHY6_Select_Upsi2mumu

BPHY6_Select_Bmumu2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Bmumu2mumu",
  HypothesisName        = "Bmumu",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 5366.77,
  MassMin               = 4200.0,
  MassMax               = 8000.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Bmumu2mumu
print BPHY6_Select_Bmumu2mumu

BPHY6_Select_Zmumu2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Zmumu2mumu",
  HypothesisName        = "Zmumu",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 91187.6,
  MassMin               = 60000.0,
  MassMax               = 120000.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Zmumu2mumu
print BPHY6_Select_Zmumu2mumu

BPHY6_Select_Onia2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY6_Select_Onia2mumu",
  HypothesisName        = "Onia",
  InputVtxContainerName = "BPHY6OniaCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = dimuon_mass_min,
  MassMax               = dimuon_mass_max,
  Chi2Max               = 20)
  
ToolSvc += BPHY6_Select_Onia2mumu
print BPHY6_Select_Onia2mumu



trigger_list = [r'HLT_\d?mu\d+']

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY6TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "BPHY6TrigSkimmingTool",
                                                                TriggerListOR               = trigger_list )
ToolSvc += BPHY6TrigSkimmingTool



expression = "count(BPHY6OniaCandidates.passed_Onia) > 0 "
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY6_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY6_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY6_SelectEvent
print      BPHY6_SelectEvent

#--------------------------------------------------------------------
## 6/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which hasn't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the 
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY6Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY6Thin_vtxTrk",
  ThinningService            = "BPHY6ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY6OniaCandidates"],
  PassFlags                  = ["passed_Onia"], )

ToolSvc += BPHY6Thin_vtxTrk


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY6MuonTPThinningTool",
                                                                         ThinningService         = "BPHY6ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY6MuonTPThinningTool

# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY6TruthThinTool = DerivationFramework__GenericTruthThinning(name             = "BPHY6TruthThinTool",
                                                        ThinningService         = "BPHY6ThinningSvc",
                                                        ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443 || TruthParticles.pdgId == 553 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553 || TruthParticles.pdgId == 23 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == 511 || TruthParticles.pdgId == 521 || TruthParticles.pdgId == 541",
                                                        PreserveDescendants     = True,
                                                        PreserveAncestors       = True)
ToolSvc += BPHY6TruthThinTool
print      BPHY6TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!

# Added by ASC
BPHY6ThinningTools = [BPHY6Thin_vtxTrk, BPHY6MuonTPThinningTool]
if isSimulation:
    BPHY6ThinningTools.append(BPHY6TruthThinTool)


# Build a tool to apply the OR combination of the String expression skimming tool, and the Trigger Skimming Tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SkimmingORTool = CfgMgr.DerivationFramework__FilterCombinationOR("BPHY6SkimmingOR",
                                                                  FilterList = [BPHY6_SelectEvent,BPHY6TrigSkimmingTool],)
ToolSvc += SkimmingORTool
print SkimmingORTool

# The name of the kernel (BPHY6Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY6Kernel",
   AugmentationTools = [BPHY6_Reco_mumu,
                        BPHY6_Select_Jpsi2mumu, BPHY6_Select_Psi2mumu, BPHY6_Select_Upsi2mumu,BPHY6_Select_Bmumu2mumu,
                        BPHY6_Select_Zmumu2mumu,BPHY6_Select_Onia2mumu, BPHY6_Extrap_Tool],
   SkimmingTools     =  [SkimmingORTool],
   #   ThinningTools     = [BPHY6Thin_vtxTrk, BPHY6MuonTPThinningTool]
   ThinningTools     = BPHY6ThinningTools

   )



#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY6Stream )
BPHY6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY6Stream.AcceptAlgs(["BPHY6Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY6ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY6SlimmingHelper = SlimmingHelper("BPHY6SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY6SlimmingHelper.IncludeMuonTriggerContent = True
BPHY6SlimmingHelper.IncludeBPhysTriggerContent = True

AllVariables += ["LVL1MuonRoIs"]

## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY6RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY6RefittedPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]

AllVariables += ["HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID"]
AllVariables += ["HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig"]
AllVariables += ["HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF"]
AllVariables += ["HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF"]
AllVariables += ["HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrig"]



## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]
AllVariables += ["MuonSpectrometerTrackParticles"]

## muon container
AllVariables += ["Muons"]
AllVariables += ["HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo"]
AllVariables += ["HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo"]
AllVariables += ["HLT_xAOD__MuonContainer_MuonEFInfo"]


AllVariables += ["HLT_xAOD__TrigBphysContainer_L2BMuMuXFex" ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_EFBMuMuXFex" ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_L2BMuMuFex"  ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_EFBMuMuFex"  ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_L2TrackMass" ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_EFTrackMass" ]
AllVariables += ["HLT_xAOD__TrigBphysContainer_L2MultiMuFex"]
AllVariables += ["HLT_xAOD__TrigBphysContainer_EFMultiMuFex"]


## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY6_Reco_mumu.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY6_Reco_mumu.OutputVtxContainerName]

if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY6SlimmingHelper.AllVariables = AllVariables
BPHY6SlimmingHelper.StaticContent = StaticContent
BPHY6SlimmingHelper.AppendContentToStream(BPHY6Stream)


