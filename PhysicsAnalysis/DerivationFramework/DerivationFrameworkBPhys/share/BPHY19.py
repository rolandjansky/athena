#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#====================================================================
# BPHY19.py
# Derivation for dimuon + photon conversion (chi_c/b)
# Contact: A. Chisholm <andrew.chisholm@cern.ch>
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation

#--------------------------------------------------------------------
# Setup the JpsiFinder vertex fitter tools
#--------------------------------------------------------------------

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY19_VertexTools = BPHYVertexTools("BPHY19")

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY19JpsiFinder = Analysis__JpsiFinder(
  name                        = "BPHY19JpsiFinder",
  OutputLevel                 = INFO,
  muAndMu                     = True,
  muAndTrack                  = False,
  TrackAndTrack               = False,
  assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = 100000.0,
  invMassLower                = 0.0,
  Chi2Cut                     = 200.,
  oppChargesOnly              = True,
  atLeastOneComb              = True,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY19_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY19_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY19_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = BPHY19_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = BPHY19_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False )

ToolSvc += BPHY19JpsiFinder
print      BPHY19JpsiFinder

#--------------------------------------------------------------------
# Setup the vertex reconstruction tools
#--------------------------------------------------------------------

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu

BPHY19_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "BPHY19_Reco_mumu",
  JpsiFinder             = BPHY19JpsiFinder,
  OutputVtxContainerName = "BPHY19OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY19RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)

ToolSvc += BPHY19_Reco_mumu
print BPHY19_Reco_mumu

#--------------------------------------------------------------------
# Setup the vertex selection and augmentation tools
#--------------------------------------------------------------------

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

# psi(nS)->mu+mu- candidates
BPHY19_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY19_Select_Psi2mumu",
  HypothesisName        = "Psi",
  InputVtxContainerName = "BPHY19OniaCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 4500.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += BPHY19_Select_Psi2mumu
print BPHY19_Select_Psi2mumu

# Y(nS)->mu+mu- candidates
BPHY19_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY19_Select_Upsi2mumu",
  HypothesisName        = "Upsi",
  InputVtxContainerName = "BPHY19OniaCandidates",
  VtxMassHypo           = 9460.30,
  MassMin               = 8000.0,
  MassMax               = 12000.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += BPHY19_Select_Upsi2mumu
print BPHY19_Select_Upsi2mumu


#--------------------------------------------------------------------
# Configure the conversion finder
#--------------------------------------------------------------------

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY19_CascadeVertexFitter = Trk__TrkVKalVrtFitter(
    name                 = "BPHY19_CascadeVertexFit",
    Extrapolator         = BPHY19_VertexTools.InDetExtrapolator,
   #FirstMeasuredPoint   = True,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BPHY19_CascadeVertexFitter
print      BPHY19_CascadeVertexFitter

include("DerivationFrameworkBPhys/configureConversionFinder.py")
BPHY19_ConvTools = BPHYConversionFinderTools("BPHY19")

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysConversionFinder
BPHY19_ConversionFinder   = DerivationFramework__BPhysConversionFinder(
    name = "BPHY19_ConversionFinder",
    VertexFitterTool = BPHY19_ConvTools.InDetSecVxFitterTool,
    VertexEstimator = BPHY19_ConvTools.InDetSecVtxPointEstimator,
    DistanceTool = BPHY19_ConvTools.InDetSecVxTrkDistanceFinder,
    ConversionPostSelector = BPHY19_ConvTools.InDetSecVtxPostSelector,
    CascadeFitter = BPHY19_CascadeVertexFitter,
    InputTrackParticleContainerName = "InDetTrackParticles",
    ConversionContainerName = "BPhysConversionCandidates",
    DiMuonVertexContainer  = "BPHY19OniaCandidates",
    PassFlagsToCheck  = ["passed_Psi","passed_Upsi"],
    RequireDeltaM = True, # Only save conversion if it's a chi_c,b candidate (passes "MaxDeltaM" cut w.r.t. any di-muon candidate)
    MaxDeltaM = 2000.0)

ToolSvc += BPHY19_ConversionFinder
print BPHY19_ConversionFinder

#--------------------------------------------------------------------
# Select the events to save
#--------------------------------------------------------------------

# Require at least one conversion AND one di-muon
BPHY19_expression = "count(BPhysConversionCandidates.passed) > 0 && (count(BPHY19OniaCandidates.passed_Psi) > 0 || count(BPHY19OniaCandidates.passed_Upsi) > 0)"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY19_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY19_SelectEvent",
                                                                expression = BPHY19_expression)
ToolSvc += BPHY19_SelectEvent
print BPHY19_SelectEvent

#--------------------------------------------------------------------
# Thin Collections
#--------------------------------------------------------------------

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk

# Keep tracks from di-muon vertices
BPHY19Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY19Thin_vtxTrk",
  ThinningService            = "BPHY19ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY19OniaCandidates"],
  PassFlags                  = ["passed_Psi", "passed_Upsi"] )

ToolSvc += BPHY19Thin_vtxTrk

# Keep tracks from conversions
BPHY19Thin_ConvTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY19Thin_ConvTrk",
  ThinningService            = "BPHY19ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPhysConversionCandidates"],
  PassFlags                  = ["passed"] )

ToolSvc += BPHY19Thin_ConvTrk

# Keep tracks from all muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY19MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY19MuonTPThinningTool",
                                                                         ThinningService         = "BPHY19ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += BPHY19MuonTPThinningTool

#--------------------------------------------------------------------
# Truth Particle Thinning
#--------------------------------------------------------------------

BPHY19_TruthIDString = ""
BPHY19_TruthIDString += "TruthParticles.pdgId == 443" #J/psi
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 100443" #psi(2S)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 10441" #chi_c0(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 20443" #chi_c1(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 445" #chi_c2(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 553" #Y(1S)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 100553" #Y(2S)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 200553" #Y(3S)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 10551" #chi_b0(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 110551" #chi_b0(2P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 210551" #chi_b0(3P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 20553" #chi_b1(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 120553" #chi_b1(2P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 220553" #chi_b1(3P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 555" #chi_b2(1P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 100555" #chi_b2(2P)
BPHY19_TruthIDString += " || "
BPHY19_TruthIDString += "TruthParticles.pdgId == 200555" #chi_b2(3P)

print "PDG IDs to save:"
print BPHY19_TruthIDString

# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY19TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY19TruthThinTool",
                                                        ThinningService         = "BPHY19ThinningSvc",
                                                        ParticleSelectionString = BPHY19_TruthIDString,
                                                        PreserveDescendants     = True,
                                                        PreserveAncestors      = True)
ToolSvc += BPHY19TruthThinTool
print BPHY19TruthThinTool

#--------------------------------------------------------------------
# Create the derivation kernel
#--------------------------------------------------------------------

BPHY19ThinningTools = [BPHY19Thin_vtxTrk, BPHY19MuonTPThinningTool, BPHY19Thin_ConvTrk]
if isSimulation:
    BPHY19ThinningTools.append(BPHY19TruthThinTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY19Kernel",
   AugmentationTools = [BPHY19_Reco_mumu, BPHY19_Select_Psi2mumu, BPHY19_Select_Upsi2mumu,BPHY19_ConversionFinder],
   SkimmingTools     = [BPHY19_SelectEvent],
   ThinningTools     = BPHY19ThinningTools
   )

#--------------------------------------------------------------------
# Create the stream
#--------------------------------------------------------------------

streamName = derivationFlags.WriteDAOD_BPHY19Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY19Stream )
BPHY19Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY19Stream.AcceptAlgs(["BPHY19Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY19ThinningSvc", outStreams=[evtStream] )

#--------------------------------------------------------------------
# Generic Collection Slimming
#--------------------------------------------------------------------

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY19SlimmingHelper = SlimmingHelper("BPHY19SlimmingHelper")
AllVariables = []
StaticContent = []

# Trigger informtion
BPHY19SlimmingHelper.IncludeMuonTriggerContent = True
BPHY19SlimmingHelper.IncludeBPhysTriggerContent = True

## Primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY19RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY19RefittedPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]

## Muon container
AllVariables += ["Muons"]

## Di-muon candidates
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY19_Reco_mumu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY19_Reco_mumu.OutputVtxContainerName]

# Conversions
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY19_ConversionFinder.ConversionContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY19_ConversionFinder.ConversionContainerName]

# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY19SlimmingHelper.AllVariables = AllVariables
BPHY19SlimmingHelper.StaticContent = StaticContent
BPHY19SlimmingHelper.AppendContentToStream(BPHY19Stream)
