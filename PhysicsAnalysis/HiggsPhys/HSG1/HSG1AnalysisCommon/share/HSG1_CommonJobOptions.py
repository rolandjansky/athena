## ---------------------------------------------------------
##                  PhotonAnalysisUtils
## ---------------------------------------------------------

# Author: Marcello Fanti
# Milano, August 2007

## input AOD files must be defined at the bottom of this file...
## ----------------------------------------------------------

MessageSvc.OutputLevel = ERROR
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
MessageSvc.warningLimit = 9999999
MessageSvc.infoLimit = 9999999
MessageSvc.debugLimit = 9999999
MessageSvc.verboseLimit = 9999999

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

from AthenaCommon.AppMgr import ServiceMgr

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUanalysis
theJob += PAUanalysis()
theJob.PAUanalysis.UseTrigger = UseTrigger
theJob.PAUanalysis.OnlyTriggerDecision = False
theJob.PAUanalysis.EnforceTriggerLogicalFlow = EnforceTriggerLogicalFlow
theJob.PAUanalysis.ElectronContainerName = ElectronContainer
theJob.PAUanalysis.MuonContainerName = MuonContainer
theJob.PAUanalysis.CaloCellContainer = CaloCells
theJob.PAUanalysis.doTrack = DumpTracks
theJob.PAUanalysis.doEtCone = DumpAdditionalEtCones
theJob.PAUanalysis.doCell = DumpCells
theJob.PAUanalysis.doCluster = DumpClusters

if ( TriggerNames == 'R14' ) :
    theJob.PAUanalysis.TriggerSignatures = [ "g20" ,
                                                       "g20i" ,
                                                       "2g20" ,
                                                       "g25" ,
                                                       "g105" ,
                                                       "g10" ]
if ( TriggerNames == 'R15' ) :
    theJob.PAUanalysis.TriggerSignatures = [ "g20_loose" ,
                                             "g20i_loose" ,
                                             "2g20_loose" ,
                                             "g25_loose" ,
                                             "g105" ,
                                             "g10_loose" ]

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUdumper
ToolSvc += PAUdumper()
ToolSvc.PAUdumper.DumpNtuple = DumpNtuple

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggUserDataTool
ToolSvc += PAUhggUserDataTool()
ToolSvc.PAUhggUserDataTool.DumpNtuple = DumpNtuple

# theJob.PAUanalysis.DumpTruth = 2   # dump truth tree in file dumpTruth.txt (BIG! ~ 200 K/evt)
theJob.PAUanalysis.OutputLevel = INFO
theJob.PAUanalysis.EgDetailContainerName = PhotonsDetailsContainerName

# theJob.PAUanalysis.UsePAUtruthMatchingTool = True

# here if you want to set which process you are analysing...

from PhotonAnalysisUtils.PAUanalysisCodes import PAUanalysisCodes
#PAUanalysis.AnalysisCode = PAUanalysisCodes["gluongluon"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["VBF"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["WHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["ZHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["ttHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDgammagamma"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDgammajet"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDjetjet"]

### The related algtools ###
from AthenaCommon.AppMgr import ToolSvc

##########################################################################################################
#  Configure the extrapolator
##########################################################################################################
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator(name = 'egammaExtrapolator')
theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic calculation of eloss in calorimeters
# all left to MaterialEffects/EnergyLossUpdators

MaterialEffectsUpdator = CfgMgr.Trk__MaterialEffectsUpdator
AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
NoElossMaterialEffectsUpdator.EnergyLoss = False
ToolSvc += NoElossMaterialEffectsUpdator

#setup MaterialEffectsUpdator arrays
MyUpdators = []
MyUpdators += [ToolSvc.AtlasMaterialEffectsUpdator] # for ID
MyUpdators += [ToolSvc.NoElossMaterialEffectsUpdator] # for Calo
#MyUpdators += [ToolSvc.NoElossMaterialEffectsUpdator] # for muon

MySubUpdators = []
MySubUpdators += [ToolSvc.AtlasMaterialEffectsUpdator.name()] # for ID
MySubUpdators += [ToolSvc.NoElossMaterialEffectsUpdator.name()] # for Calo
MySubUpdators += [ToolSvc.NoElossMaterialEffectsUpdator.name()] # for muon

theAtlasExtrapolator.MaterialEffectsUpdators = MyUpdators
theAtlasExtrapolator.SubMEUpdators = MySubUpdators
ToolSvc+=theAtlasExtrapolator

# moved to PAUhybridPrimaryVertexTool theJob.PAUanalysis.ExtrapolationTool=theAtlasExtrapolator

from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
theCaloDepthTool=CaloDepthToolFactory(depth='cscopt2')
theCaloDepthTool.DepthChoice="cscopt2"
#theCaloDepthTool=CaloDepthToolFactory(depth='cscopt')
ToolSvc += theCaloDepthTool

# add tool ExtrapolateTrackToCalo
exToCalo = CfgMgr.ExtrapolateToCaloTool(Extrapolator = theAtlasExtrapolator,CaloDepthTool=theCaloDepthTool.getFullName())
ToolSvc+=exToCalo
#print      exToCalo

#ToolSvc.ExtrapolateToCaloTool.CaloDepthTool.DepthChoice = "egparametrized"
ToolSvc.ExtrapolateToCaloTool.OutputLevel = INFO
#
#print "------------------------------------------------->>>extrapolToCalo<<<---------------------------------------------------"
#print ToolSvc.ExtrapolateToCaloTool.CaloDepthTool
#print ToolSvc.ExtrapolateToCaloTool.CaloDepthTool.DepthChoice
#print exToCalo
#print "------------------------------------------------->>>extrapolToCalo<<<---------------------------------------------------"


##########################################################################################################


##########################################################################################################
#  Configure the InDetConversionFinder
############################################################################################################

theAtlasMagField = CfgMgr.Trk__MagneticFieldTool('theAtlasMagField')
ToolSvc += theAtlasMagField

from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
theAtlasTrackSummaryTool = AtlasTrackSummaryTool("theAtlasTrackSummaryTool")

ToolSvc += theAtlasTrackSummaryTool

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetConversionVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InDetConversionVxFitter",
                                                    # Extrapolator        = InDetExtrapolator,
                                                    Extrapolator        = theAtlasExtrapolator,
                                                    IterationNumber     = 30,
                                                    #MagFieldSvc         = InDetMagField,
                                                    MagFieldSvc         = theAtlasMagField,
                                                    Robustness          = 6,
#This line has no more meaning                      Constraint          = 12,
                                                    usePhiCnst          = True,
                                                    useThetaCnst        = True,
                                                    InputParticleMasses = [0.511,0.511],
                                                    MassForConstraint   = 2.,
                                                    VertexForConstraint = [0.,0.,0.],
                                                    CovVrtForConstraint = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.],
                                                    MakeExtendedVertex=True
                                                   # MakeExtendedVertex=False
#                                                    IterationPrecision     = 1.e-5,
                                                    )

ToolSvc += InDetConversionVxFitterTool
print      InDetConversionVxFitterTool

#next instruction no more useful but keep it for a while
#theJob.PAUanalysis.VertexFitterTool = InDetConversionVxFitterTool



###### Vertex fitter : need to put a second instance to prevent perturbation to convFlag value ######
#this is the standard one
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetConversionVxFitterTool = Trk__TrkVKalVrtFitter(name               = "InDetConversionVxFitter",
                                                   Extrapolator        = theAtlasExtrapolator,
                                                   IterationNumber     = 30,
                                                   MagFieldSvc         = theAtlasMagField,
                                                   Robustness          = 6,
                                                   usePhiCnst          = True,
                                                   useThetaCnst        = True,
                                                   InputParticleMasses = [0.511,0.511],
                                                   MakeExtendedVertex=True,
                                                   FirstMeasuredPoint=True
                                                   )

###### Vertex fitter : need to put a second instance to prevent perturbation to convFlag value #####
#This is the one for HybridPrimaryVertex
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetConversionVxFitterToolBis = Trk__TrkVKalVrtFitter(name            = "InDetConversionVxFitterBis",
                                                   Extrapolator        = theAtlasExtrapolator,
                                                   IterationNumber     = 30,
                                                   MagFieldSvc         = theAtlasMagField,
                                                   Robustness          = 6,
                                                   usePhiCnst          = True,
                                                   useThetaCnst        = True,
                                                   InputParticleMasses = [0.511,0.511],
                                                   MakeExtendedVertex=True,
                                                   FirstMeasuredPoint=True
                                                   )

ToolSvc += InDetConversionVxFitterToolBis
#next line is debugging mode, for providing feedback to Vadim if some bugs are found.
#ToolSvc.InDetConversionVxFitterBis.OutputLevel= DEBUG
print      InDetConversionVxFitterToolBis

# Distance of minimum approach utility
#
from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
InDetConversionTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = 'InDetConversionTrkDistanceFinder')
ToolSvc += InDetConversionTrkDistanceFinder
print      InDetConversionTrkDistanceFinder

# Straight line propagator needed to clean-up single track conversions
#
from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
InDetConversionPropagator = Propagator(name = 'InDetConversionPropagator')
ToolSvc += InDetConversionPropagator
print      InDetConversionPropagator

# Helper Tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
InDetConversionHelper = InDet__ConversionFinderUtils(name = "ConversionFinderUtils")
ToolSvc += InDetConversionHelper
print      InDetConversionHelper

# Track selector tool
#
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
InDetConversionTrackSelector = InDet__InDetConversionTrackSelectorTool(name             = "ConversionTrackSelector",
                                                                       Extrapolator     = theAtlasExtrapolator,
                                                                       maxSiD0          = 10000.,    #50.,      #50.0,   # 10000.,
                                                                       maxTrtD0         = 10000.,    #100.,     #100.,   # 10000.,
                                                                       maxSiZ0          = 10000.,   #350.0,  # 10000.,
                                                                       maxTrtZ0         = 10000.,   #1400.,  # 10000.,
                                                                       minPt            = 500.0,
                                                                       RatioCut1        = 0.0,       #0.5,      #0.5,    # 0.,
                                                                       RatioCut2        = 0.1,      #0.1,    # 0.,
                                                                       RatioCut3        = 0.1)      #0.05)   # 0.,                                                                   maxSiD0          = 50.0,

#                                                                       maxTrtD0         = 100.,
#                                                                       maxSiZ0          = 10000.,
#                                                                       maxTrtZ0         = 10000.,
#                                                                       minPt            = 500.0,
#                                                                       RatioCut1        = 0.,  #0.5,
#                                                                       RatioCut2        = 0.,  #0.1,
#                                                                       RatioCut3        = 0.)  #0.05)

ToolSvc += InDetConversionTrackSelector
print      InDetConversionTrackSelector

# Track pairs selector
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
InDetConversionTrackPairsSelector = InDet__TrackPairsSelector(name                       = "ConversionTrackPairsSelector",
                                                              ConversionFinderHelperTool = InDetConversionHelper,
                                                              DistanceTool               = InDetConversionTrkDistanceFinder,
                                                              MaxFirstHitRadius          = 500.,
                                                              MaxDistBetweenTracks       = [10.,50.,50.],#[6.,90.,30.],
                                                              MaxEta                     = [.3,.5,.5]) #[0.5,0.9,0.4],
#                                                              MaxInitDistance            = [350.,640.,80.])

ToolSvc += InDetConversionTrackPairsSelector
print      InDetConversionTrackPairsSelector

# Vertex point estimator
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
InDetConversionVtxPointEstimator = InDet__VertexPointEstimator(name                   = "ConversionVtxPointEstimator",
#                                                               MaxTrkXYDiffAtVtx      = [6.,8.,2.],
#                                                               MaxTrkZDiffAtVtx       = [80.,380.,80.],
#                                                               MaxTrkXYValue          = [450.,650.,400.],
#                                                               MinArcLength           = [30.,80.,350.],
#                                                               MaxArcLength           = [600.,650.,680.],
#                                                               MaxChi2OfVtxEstimation = 20.)
                                                               MinDeltaR              = [-5.,-25.,-50.],
							       MaxDeltaR              = [5.,10.,10.],
							       MaxPhi                 = [0.05, 0.1, 0.1])
ToolSvc += InDetConversionVtxPointEstimator
print      InDetConversionVtxPointEstimator

# Conversion post selector
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
InDetConversionPostSelector = InDet__ConversionPostSelector(name             = "ConversionPostSelector",
    #                                                            MaxChi2PerTrack  = [400., 100., 80.],     # [10000.,10000.,10000.],  #[40.,50.,25.],
							    MaxChi2Vtx  = [50., 50., 50.],
                                                            MaxInvariantMass = [10000., 10000., 10000.], #[60., 60., 30.],       # [10000.,10000.,10000.],  #[60.,50.,25.],
                                                            MinFitMomentum   = [0., 0., 0.], #[2000., 2000., 2000.], # [0.,0.,0.],              #[2000.,2000.,2000.],
                                                            MinRadius        = [-10000., -10000., -10000.], #[30., 35., 250.],      # [0.,0.,0.],              #[30.,35.,350.],
							    MinPt            = 0., # 500.,
                                                            MaxdR            = -10000., #-250.)                 # -10000.)                 #-100.
							    MaxPhiVtxTrk     = 10000.)
ToolSvc += InDetConversionPostSelector
print      InDetConversionPostSelector

# Single track conversion tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
InDetSingleTrackConversion = InDet__SingleTrackConversionTool(name                       = "SingleTrackConversionTool",
                                                              ConversionFinderHelperTool = InDetConversionHelper,
                                                              #TrackSummaryTool           = InDetTrackSummaryTool,
                                                              TrackSummaryTool           = theAtlasTrackSummaryTool,
                                                              MinInitialHitRadius        = 70.,
                                                              MinRatioOfHLhits           = 0.95)#0.5)
ToolSvc += InDetSingleTrackConversion
print      InDetSingleTrackConversion

############################################################################################################
#for the second instance : for HybridPrimaryVertex
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
InDetConversionFinderToolsBis = InDet__InDetConversionFinderTools(name                       = "InDetConversionFinderToolsBis",
#next line : don't put Bis to prevent conflict with HybridPrimaryVertex
                                                                 VertexFitterTool           = InDetConversionVxFitterTool,
                                                                 TrackSelectorTool          = InDetConversionTrackSelector,
                                                                 TrackPairsSelector         = InDetConversionTrackPairsSelector,
                                                                 ConversionFinderHelperTool = InDetConversionHelper,
                                                                 VertexPointEstimator       = InDetConversionVtxPointEstimator,
                                                                 PostSelector               = InDetConversionPostSelector,
                                                                 SingleTrackConversionTool  = InDetSingleTrackConversion,
                                                                 Extrapolator               = theAtlasExtrapolator,
                                                                 TrackParticleCollection    = "TrackParticleCandidate",
                                                                 RemoveTrtTracks            = False,
                                                                 IsConversion               = True)
ToolSvc += InDetConversionFinderToolsBis
print      InDetConversionFinderToolsBis

#This is the standard one
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
InDetConversionFinderTools = InDet__InDetConversionFinderTools(name                       = "InDetConversionFinderTools",
                                                               VertexFitterTool           = InDetConversionVxFitterTool,
                                                               TrackSelectorTool          = InDetConversionTrackSelector,
                                                               TrackPairsSelector         = InDetConversionTrackPairsSelector,
                                                               ConversionFinderHelperTool = InDetConversionHelper,
                                                               VertexPointEstimator       = InDetConversionVtxPointEstimator,
                                                               PostSelector               = InDetConversionPostSelector,
                                                               SingleTrackConversionTool  = InDetSingleTrackConversion,
                                                               #Extrapolator               = InDetExtrapolator,
                                                               Extrapolator               = theAtlasExtrapolator,
                                                               TrackParticleCollection    = "TrackParticleCandidate",
                                                               RemoveTrtTracks            = False,
                                                               IsConversion               = True)
ToolSvc += InDetConversionFinderTools
print      InDetConversionFinderTools



############################################################################################################
#for the second instance
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
InDetConversionFinderToolsBis = InDet__InDetConversionFinderTools(name                       = "InDetConversionFinderToolsBis",
                                                                 VertexFitterTool           = InDetConversionVxFitterToolBis,
                                                                 TrackSelectorTool          = InDetConversionTrackSelector,
                                                                 TrackPairsSelector         = InDetConversionTrackPairsSelector,
                                                                 ConversionFinderHelperTool = InDetConversionHelper,
                                                                 VertexPointEstimator       = InDetConversionVtxPointEstimator,
                                                                 PostSelector               = InDetConversionPostSelector,
                                                                 SingleTrackConversionTool  = InDetSingleTrackConversion,
                                                                 Extrapolator               = theAtlasExtrapolator,
                                                                 TrackParticleCollection    = "TrackParticleCandidate",
                                                                 RemoveTrtTracks            = False,
                                                                 IsConversion               = True)
ToolSvc += InDetConversionFinderToolsBis
print      InDetConversionFinderToolsBis
############################################################################################################

from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
theAtlasMagField = Trk__MagneticFieldTool('theAtlasMagField')
ToolSvc += theAtlasMagField

###### Vertex fitter ######
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InDetConversionVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InDetConversionVxFitter",
                                                    # Extrapolator        = InDetExtrapolator,
                                                    Extrapolator        = theAtlasExtrapolator,
                                                    IterationNumber     = 30,
                                                    #MagFieldSvc         = InDetMagField,
                                                    MagFieldSvc         = theAtlasMagField,
                                                    Robustness          = 6,
#This line has no more meaning                      Constraint          = 12,
                                                    usePhiCnst          = True,
                                                    useThetaCnst        = True,
                                                    InputParticleMasses = [0.511,0.511],
                                                    MassForConstraint   = 2.,
                                                    VertexForConstraint = [0.,0.,0.],
                                                    CovVrtForConstraint = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.])
ToolSvc += InDetConversionVxFitterTool

####################################################################################################################



from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
TrackToVertexTool = Reco__TrackToVertex()
ToolSvc += TrackToVertexTool

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
EMTrackIsolationExtrapolator = AtlasExtrapolator()
ToolSvc+=EMTrackIsolationExtrapolator

from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
EMTrackIsolationTrackSummaryTool = AtlasTrackSummaryTool()
ToolSvc += EMTrackIsolationTrackSummaryTool

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
OuterTrackIsolationTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "OuterTrackIsolationTrackSelectorTool",
                                                                             pTMin                = 1000.,
                                                                             IPd0Max              = 1.*mm,
                                                                             IPz0Max              = 9999.,
                                                                             nHitBLayer           = 1,
                                                                             nHitPix              = 0,
                                                                             nHitSct              = 0,
                                                                             nHitSi               = 7,
                                                                             #TrtMaxEtaAcceptance  = 0, #don't check nb TRT hits
                                                                             nHitTrt              = 0,
                                                                             #nSharedBLayer        = 999,
                                                                             #nSharedPix           = 999,
                                                                             #nSharedSct           = 999,
                                                                             #nSharedSi            = 999,
                                                                             useTrackQualityInfo  = False,
                                                                             #d0MaxPreselection    = 9999.,
                                                                             TrackSummaryTool     = EMTrackIsolationTrackSummaryTool,
                                                                             Extrapolator         = EMTrackIsolationExtrapolator,
                                                                             OutputLevel          = WARNING)
ToolSvc += OuterTrackIsolationTrackSelectorTool
OuterTrackIsolationTrackSelectorTool.OutputLevel = INFO

InnerTrackIsolationTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InnerTrackIsolationTrackSelectorTool",
                                                                             pTMin                = 1000.,
                                                                             IPd0Max              = 1.*mm,
                                                                             IPz0Max              = 9999.,
                                                                             nHitBLayer           = 1,
                                                                             nHitPix              = 0,
                                                                             nHitSct              = 0,
                                                                             nHitSi               = 7,
                                                                             #TrtMaxEtaAcceptance  = 0, #don't check nb TRT hits
                                                                             nHitTrt              = 0,
                                                                             #nSharedBLayer        = 999,
                                                                             #nSharedPix           = 999,
                                                                             #nSharedSct           = 999,
                                                                             #nSharedSi            = 999,
                                                                             useTrackQualityInfo  = False,
                                                                             #d0MaxPreselection    = 9999.,
                                                                             TrackSummaryTool     = EMTrackIsolationTrackSummaryTool,
                                                                             Extrapolator         = EMTrackIsolationExtrapolator,
                                                                             OutputLevel          = WARNING)
ToolSvc += InnerTrackIsolationTrackSelectorTool
InnerTrackIsolationTrackSelectorTool.OutputLevel = INFO

from TrackIsolationTools.TrackIsolationToolsConf import TrackIsolationTool
NewTrackIsolationTool = TrackIsolationTool(
    name = "NewTrackIsolationTool",
    DoTrackIsolation         = True,
    DoSumConversionTracks    = False,
    InnerCone                = 0.1,
    PtMaxInnerCone           = 99999999.,
    UseInnerConeTrkSelection = True,
    TrackParticleCollection  = "TrackParticleCandidate",
    ConversionContainer      = "ConversionCandidate",
    CaloCellContainer        = CaloCells, # ESD: "AllCalo", AOD: "AODCellContainer"
    TrackSelector            = OuterTrackIsolationTrackSelectorTool,
    InnerConeTrackSelector   = InnerTrackIsolationTrackSelectorTool,
    TrackToVertex            = TrackToVertexTool,
    DeltaRTolerance          = 0.
)
ToolSvc += NewTrackIsolationTool
NewTrackIsolationTool.OutputLevel = ERROR

print "---------------> NewTrackIsolation"
print NewTrackIsolationTool

####################################################################################################################





from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUphotonIdentificationTool
ToolSvc += PAUphotonIdentificationTool()
ToolSvc.PAUphotonIdentificationTool.EgDetailContainerName = PhotonsDetailsContainerName

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUphotonCorrectionTool
ToolSvc += PAUphotonCorrectionTool()
ToolSvc.PAUphotonCorrectionTool.OutputLevel = INFO

if not 'doReRunEgamma' in dir(): #to be used by PAUphotonSelector only
    doReRunEgamma = False

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUphotonSelector
ToolSvc += PAUphotonSelector()
ToolSvc.PAUphotonSelector.OutputLevel = INFO
#ToolSvc.PAUphotonSelector.doRecovery = True
ToolSvc.PAUphotonSelector.PhotonContainer = PhotonContainerMerged
ToolSvc.PAUphotonSelector.TrackIsolationTool = NewTrackIsolationTool
ToolSvc.PAUphotonSelector.UsePhotonRecoveryTool = usePhotonRecoveryTool
ToolSvc.PAUphotonSelector.UsePAUconversionFlagTool = usePAUconversionFlagTool
ToolSvc.PAUphotonSelector.EgDetailContainerName = PhotonsDetailsContainerName
ToolSvc.PAUphotonSelector.RemovePhotonWithoutEgDetail = doReRunEgamma

#FANTI ToolSvc.PAUphotonSelector.LoopOverElectronContainer = LoopOverElectronContainer
# --- Photon selection:
#ToolSvc.PAUphotonSelector.GammaTypeNames = [ "unconv" ] # default is [ "unconv" , "conv1" , "conv2" ]
#ToolSvc.PAUphotonSelector.PtIsolationCut = ...    # Pt cut in MeV (default is 4000; negative value for not using isolation tool)
#ToolSvc.PAUphotonSelector.PhotonIDbyCuts = ...    # photon ID (default==0 use isem(PhotonTight); >0 use G.Unal's itune ; -1 use likelihood)
#ToolSvc.PAUphotonSelector.PhotonIDlogLHcut = ...  # set cut for photon ID loglikelihood (default==9.4; works if PhotonIDbyCuts==-1)
# --- Event selection:
#ToolSvc.PAUphotonSelector.PtCut1 = ...  # inclusive selection: pt cut (MeV) for leading photon (default is 40 GeV)
#ToolSvc.PAUphotonSelector.PtCut2 = ...  # inclusive selection: pt cut (MeV) for sub-leading photon (default is 25 GeV)
# --- printout
#ToolSvc.PAUphotonSelector.DumpReco = True    # dump reco e/gammas in file dumpReco.txt (~ 0.8 K/evt)
#ToolSvc.PAUphotonSelector.FileEventSummary = "myEventSummary.txt" # comment out to switch off event summary
#ToolSvc.PAUphotonSelector.FileEventDetails = "myEventDetails.txt" # comment out to switch off event details
#ToolSvc.PAUphotonSelector.DumpTriggerDecision = True    # dump at INFO level, default is False

if doESD:
    from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
    TruthParticleCnvTool.McEvents = MCEventCollection
    TruthParticleCnvTool.TruthParticlesOutput = MCParticleContainer

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthTool
ToolSvc += PAUtruthTool()
ToolSvc.PAUtruthTool.OutputLevel = INFO
# --- Truth handling:
#ToolSvc.PAUtruthTool.EGammasFromTruth = True # all e/gamma are kept (otherwise only gammas from H->gg) (default is true)
#ToolSvc.PAUtruthTool.MuonsFromTruth = True   # all muons are kept (default is true)
#ToolSvc.PAUtruthTool.HadronsFromTruth = True # all hadrons are kept (default is true)
#ToolSvc.PAUtruthTool.PartonsFromTruth = True # all partons are kept (default is false)
#ToolSvc.PAUtruthTool.RTruthConv = ...  # maximum conversion/interaction radius (in mm, default is 800)
#ToolSvc.PAUtruthTool.ZTruthConv = ...  # maximum conversion/interaction z (in mm, default is 2800)
#ToolSvc.PAUtruthTool.PtTruthCut = ...  # reject truth particles produced with pt<PtTruthCut (in MeV)
#ToolSvc.PAUtruthTool.deltaRmatch = ... # deltaR for angular Reco/Truth match (default is 0.1)
# --- Containers' SG keys
ToolSvc.PAUtruthTool.MCParticleContainer = MCParticleContainer
ToolSvc.PAUtruthTool.MCEventCollection = MCEventCollection
ToolSvc.PAUtruthTool.TruthJetsContainer = TruthJetsContainer
ToolSvc.PAUtruthTool.ConvertFromESD = doESD

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthMatchingTool
ToolSvc += PAUtruthMatchingTool()
ToolSvc.PAUtruthMatchingTool.OutputLevel = INFO

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUconversionFlagTool
ToolSvc += PAUconversionFlagTool()
ToolSvc.PAUconversionFlagTool.ExtrapolateToCaloToolName=exToCalo.name()
#ToolSvc.PAUconversionFlagTool.VertexFinderToolName=  InDetConversionFinderTools.getFullName()
ToolSvc.PAUconversionFlagTool.VertexFinderToolName=  InDetConversionFinderToolsBis.getFullName()
ToolSvc.PAUconversionFlagTool.OutputLevel = INFO
#ToolSvc.PAUconversionFlagTool.useBDT=True
#ToolSvc.PAUconversionFlagTool.BDTCut=0.2
#ToolSvc.PAUconversionFlagTool.DBTFileName="TMVAnalysis_BDT.weights.txt"

ToolSvc.PAUconversionFlagTool.SingleTrkConvTool = InDetSingleTrackConversion




from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhybridPrimaryVertexTool

ToolSvc += PAUhybridPrimaryVertexTool()

from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
hybridCaloDepthTool=CaloDepthToolFactory(depth='cscopt2')
#showerdefault
ToolSvc.PAUhybridPrimaryVertexTool.CaloDepthTool=hybridCaloDepthTool.getFullName()

ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolationTool=theAtlasExtrapolator
ToolSvc.PAUhybridPrimaryVertexTool.OutputLevel= INFO
ToolSvc.PAUhybridPrimaryVertexTool.SingleTrkConvTool = InDetSingleTrackConversion
#ToolSvc.PAUhybridPrimaryVertexTool.VertexFitterTool = InDetConversionVxFitterTool
ToolSvc.PAUhybridPrimaryVertexTool.VertexFitterTool = InDetConversionVxFitterToolBis
ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolateToCaloToolName=exToCalo.name()
ToolSvc.PAUhybridPrimaryVertexTool.VertexFinderToolName=  InDetConversionFinderToolsBis.getFullName()



from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtrackIsolationTool
ToolSvc += PAUtrackIsolationTool()
ToolSvc.PAUtrackIsolationTool.ExtrapolationTool=theAtlasExtrapolator

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggFitter
ToolSvc += PAUhggFitter()
ToolSvc.PAUhggFitter.ExtrapolationTool=theAtlasExtrapolator

from AthenaCommon import CfgMgr
CfgMgr.PAUanalysis(SingleTrkConvTool = InDetSingleTrackConversion)












