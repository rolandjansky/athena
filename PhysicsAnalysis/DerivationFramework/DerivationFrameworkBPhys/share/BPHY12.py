#====================================================================
# BPHY12.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag BPHY12 in Reco_tf.py   
#====================================================================

#====================================================================
# FLAGS TO PERSONALIZE THE DERIVATION
#====================================================================

skimTruth = False

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
isSimulation = DerivationFrameworkHasTruth


print 'is this simulation? ', isSimulation

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY12_VertexTools = BPHYVertexTools("BPHY12")

print '********************** VERTEX TOOLS ***********************'
print BPHY12_VertexTools
print BPHY12_VertexTools.TrkV0Fitter
print '********************** END VERTEX TOOLS ***********************'

#====================================================================
# TriggerCounting for Kernel1 #Added by Matteo
#====================================================================
#List of trigggers to be counted (high Sig-eff*Lumi ones are in)
triggersToMetadata= [
"HLT_mu11_mu6_bBmumuxv2",
"HLT_2mu10_bBmumuxv2",
"HLT_2mu6_bBmumuxv2_L1LFV-MU6",
"HLT_mu11_mu6_bBmumux_BpmumuKp",
"HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",

"HLT_mu11_mu6_bDimu",
"HLT_4mu4_bDimu6000"
              ]


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY12TriggerCountToMetadata = DerivationFramework__TriggerCountToMetadata(name = "BPHY12TriggerCount",
                                                                          TriggerList = triggersToMetadata,
                                                                          FolderName = "BPHY12")

ToolSvc += BPHY12TriggerCountToMetadata

#====================================================================
# PRESELECTION for Kernel1 #Added by Matteo
#====================================================================
## 1/ Setup the skimming based on triggers
##     

triggerList = [
"HLT_mu11_mu6_bBmumuxv2",
"HLT_2mu10_bBmumuxv2",
"HLT_2mu6_bBmumuxv2_L1LFV-MU6",
"HLT_mu11_mu6_bBmumux_BpmumuKp",
"HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
"HLT_mu11_mu6_bDimu",
"HLT_4mu4_bDimu6000"
              ]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY12TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY12TriggerSkim",
                                                            TriggerListOR = triggerList,
                                                            TriggerListAND = [] )

ToolSvc += BPHY12TriggerSkim

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY12DiMuonFinder = Analysis__JpsiFinder(
    name                        = "BPHY12DiMuonFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 100000.0,
    invMassLower                = 0.0,
    Chi2Cut                     = 200.,
    oppChargesOnly	        = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY12_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY12_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY12_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY12_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY12_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY12DiMuonFinder
print      BPHY12DiMuonFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY12_Reco_DiMuon = DerivationFramework__Reco_mumu(
  name                   = "BPHY12_Reco_DiMuon",
  JpsiFinder             = BPHY12DiMuonFinder,
  OutputVtxContainerName = "BPHY12DiMuonCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY12RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)
  
ToolSvc += BPHY12_Reco_DiMuon
print BPHY12_Reco_DiMuon

#--------------------------------------------------------------------
## 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
##    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
##    Property HypothesisName is used as a prefix for these decorations.
##    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
##    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
##    to determine which events and candidates should be kept in the output stream.
##    Multiple instances of the Select_* tools can be used on a single input collection as long as they 
##    use different "HypothesisName" flags.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY12_Select_DiMuons = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY12_Select_DiMuons",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY12DiMuonCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 1.0,
  MassMax               = 7000.0,
  Chi2Max               = 200.,
  DoVertexType          = 7)
  
ToolSvc += BPHY12_Select_DiMuons
print BPHY12_Select_DiMuons

## 4/ setup a new vertexing tool (necessary due to use of mass constraint)
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BmumuKstVertexFit = Trk__TrkVKalVrtFitter(
    name                = "BmumuKstVertexFit",
    Extrapolator        = BPHY12_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = True,
    MakeExtendedVertex  = True)

ToolSvc += BmumuKstVertexFit
print      BmumuKstVertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY12BmumuKstFinder = Analysis__JpsiPlus2Tracks(
    name                    = "BPHY12BmumuKstFinder",
    OutputLevel             = INFO, #can also be DEBUG, WARNING, VERBOSE
    kaonkaonHypothesis      = False,
    pionpionHypothesis      = False,
    kaonpionHypothesis      = True,
    trkThresholdPt          = 500.0, #minimum track pT in MeV
    trkMaxEta               = 3.0, 
    BThresholdPt            = 1000.,
    BMassLower              = 3000.0, #OI makes no sense below Jpsi mass #same values as BPHY18 (original) - Bs->JpsiKK
    BMassUpper              = 6500.0,
    JpsiContainerKey        = "BPHY12DiMuonCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    #MuonsUsedInJpsi         = "Muons", #Don't remove all muons, just those in J/psi candidate (see the following cut)
    ExcludeCrossJpsiTracks  = False,   #setting this to False rejects the muons from J/psi candidate
    TrkVertexFitterTool     = BmumuKstVertexFit,
    TrackSelectorTool       = BPHY12_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint       = False, #Set to True, according to Bs->JpsiKK DAOD
    DiTrackMassUpper        = 1110., #OI was 1500. Can eventually set these to be the K* mass?
    DiTrackMassLower        = 690.,  #OI was 500
    Chi2Cut                 = 15., #THIS IS CHI2/NDOF, checked the code!!!
    DiTrackPt               = 500.,
    TrkQuadrupletMassLower  = 1000.0, #Two electrons + two tracks (one K, one pi)
    TrkQuadrupletMassUpper  = 100000.0, # same as BPHY18, original
    #FinalDiTrackMassUpper   = 1000.,
    #FinalDiTrackMassLower   = 800.,
    #TrkDeltaZ               = 20., #Normally, this cut should not be used since it is lifetime-dependent
    FinalDiTrackPt          = 500.,
#OI    DoElectrons = True,
    #UseGSFTrackIndices      = [0,1]
    )

ToolSvc += BPHY12BmumuKstFinder
print      BPHY12BmumuKstFinder   
## 6/ setup the combined augmentation/skimming tool for the BeeKst
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk  
BPHY12_Reco_BmumuKst  = DerivationFramework__Reco_dimuTrkTrk(
    name                   = "BPHY12_Reco_BmumuKst",
    Jpsi2PlusTrackName     = BPHY12BmumuKstFinder,
    OutputVtxContainerName = "BPHY12BmumuKstCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY12RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit             = 10000,
    DoVertexType = 7)

ToolSvc += BPHY12_Reco_BmumuKst 
print      BPHY12_Reco_BmumuKst

## b/ augment and select B->eeKst candidates
#  set mass hypothesis (K pi)
BPHY12_Select_BmumuKst = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY12_Select_BmumuKst",
    HypothesisName             = "Bd", #creates output variable pass_Bd
    InputVtxContainerName      = "BPHY12BmumuKstCandidates",
    TrkMasses                  = [105.658, 105.658, 493.677, 139.570],
    VtxMassHypo                = 5279.6, #mass of B
    MassMin                    = 1.0,      #no mass cuts here
    MassMax                    = 10000.0,   #no mass cuts here
    Chi2Max                    = 30.0) #THIS IS CHI2! NOT CHI2/NDOF! Careful!

ToolSvc += BPHY12_Select_BmumuKst
print      BPHY12_Select_BmumuKst

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

if skimTruth or not isSimulation: #Only Skim Data
    expression = "count(BPHY12BmumuKstCandidates.passed_Bd) > 0"
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    BPHY12_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY12_SelectEvent",
                                                                expression = expression)
    ToolSvc += BPHY12_SelectEvent
    print BPHY12_SelectEvent

    #====================================================================
    # Make event selection based on an OR of the input skimming tools (though it seems we only have one here!)
    #====================================================================
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
    BPHY12SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
        name       = "BPHY12SkimmingOR",
        FilterList = [BPHY12_SelectEvent, BPHY12TriggerSkim]) #OR of all your different filters
    ToolSvc += BPHY12SkimmingOR
    print      BPHY12SkimmingOR

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
BPHY12Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY12Thin_vtxTrk",
  ThinningService            = "BPHY12ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY12BmumuKstCandidates"],
  PassFlags                  = ["passed_Bd"] )

ToolSvc += BPHY12Thin_vtxTrk

## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY12MuonTPThinningTool",
                                                                         ThinningService         = "BPHY12ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY12MuonTPThinningTool

# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY12TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY12TruthThinTool",
                                                        ThinningService         = "BPHY12ThinningSvc",
                                                        ParticleSelectionString = "TruthParticles.pdgId == 511 || TruthParticles.pdgId == -511 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == -531",
                                                        PreserveDescendants     = True,
                                                        PreserveAncestors      = True)
ToolSvc += BPHY12TruthThinTool
print BPHY12TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!

# Added by ASC
BPHY12ThinningTools = [BPHY12Thin_vtxTrk, BPHY12MuonTPThinningTool]
if isSimulation:
    BPHY12ThinningTools.append(BPHY12TruthThinTool)

# The name of the kernel (BPHY12Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY12Kernel",
   AugmentationTools = [BPHY12_Reco_DiMuon, BPHY12_Select_DiMuons,
                        BPHY12_Reco_BmumuKst, BPHY12_Select_BmumuKst],
   SkimmingTools     = [BPHY12SkimmingOR] if skimTruth or not isSimulation else [],
   ThinningTools     = BPHY12ThinningTools
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY12Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY12Stream )
BPHY12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY12Stream.AcceptAlgs(["BPHY12Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY12ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY12SlimmingHelper = SlimmingHelper("BPHY12SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY12SlimmingHelper.IncludeMuonTriggerContent = True
BPHY12SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY12RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY12RefittedPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
AllVariables += ["Muons"]

## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY12_Reco_DiMuon.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY12_Reco_DiMuon.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY12_Reco_DiMuon.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        % BPHY12_Reco_BmumuKst.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY12_Reco_BmumuKst.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY12_Reco_BmumuKst.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY12SlimmingHelper.AllVariables = AllVariables
BPHY12SlimmingHelper.StaticContent = StaticContent
BPHY12SlimmingHelper.AppendContentToStream(BPHY12Stream)
