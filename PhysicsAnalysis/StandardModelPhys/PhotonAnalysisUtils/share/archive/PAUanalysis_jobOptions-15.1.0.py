# Author: Marcello Fanti
# Milano, August 2007

## input AOD files must be defined at the bottom of this file...
## ----------------------------------------------------------

#UseTrigger = False
UseTrigger = True

do13030=False #Put it to True if you want to read AOD produced with rel_13.0.30 and if you want to dump trigger info

MessageSvc.OutputLevel = INFO
#MessageSvc.OutputLevel = DEBUG
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
MessageSvc.warningLimit = 9999999
MessageSvc.infoLimit = 9999999
MessageSvc.debugLimit = 9999999
MessageSvc.verboseLimit = 9999999

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

#StoreGateSvc.Dump = True  # will dump data store contents (seek '::dump()')

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
# ServiceMgr.EventSelector.BackNavigation = True

## -------------------------------------
## here define your AOD files to be read:
ServiceMgr.EventSelector.InputCollections = [ "AOD.root" ]


# if you run under release 14:
DetDescrVersion = 'ATLAS-GEO-02-01-00'  # for release 14
#DetDescrVersion = 'ATLAS-CSC-02-01-00'  # for release 14
#DetDescrVersion = 'ATLAS-CSC-05-00-00'  # for mc08 prod

include ("RecExCond/AllDet_detDescr.py")

#ServiceMgr.GeoModelSvc.IgnoreTagDifference = True # (rel14) uncomment to read files with different geometry
#GeoModelSvc.IgnoreTagDifference = True            # (rel13) uncomment to read files with different geometry

CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")


### The main algorithm: PAUanalysis ###

CBNT_AthenaAware = topSequence.CBNT_AthenaAware
CBNT_AthenaAware += CfgMgr.PAUanalysis()

CBNT_AthenaAware.PAUanalysis.UseTrigger = UseTrigger

# CBNT_AthenaAware.PAUanalysis.UseTruth = False
# CBNT_AthenaAware.PAUanalysis.DumpTruth = 2   # dump truth tree in file dumpTruth.txt (BIG! ~ 200 K/evt)
CBNT_AthenaAware.PAUanalysis.OutputLevel = INFO

# CBNT_AthenaAware.PAUanalysis.UsePAUtruthMatchingTool = True 

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

from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
theCaloDepthTool=CaloDepthToolFactory(depth='egparam')
theCaloDepthTool.DepthChoice="egparam"
#theCaloDepthTool=CaloDepthToolFactory(depth='cscopt')
ToolSvc += theCaloDepthTool

# add tool ExtrapolateTrackToCalo
exToCalo = CfgMgr.ExtrapolateToCaloTool(Extrapolator = theAtlasExtrapolator,CaloDepthTool=theCaloDepthTool.getFullName())
ToolSvc+=exToCalo
#print      exToCalo

ToolSvc.ExtrapolateToCaloTool.OutputLevel = INFO
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
#This is the standard one
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

ToolSvc += InDetConversionVxFitterTool 
print      InDetConversionVxFitterTool

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
                                                                       ##TrackSummaryTool = InDetTrackSummaryTool,
                                                                       ##Extrapolator     = InDetExtrapolator,
                                                                       TrackSummaryTool = theAtlasTrackSummaryTool,
                                                                       Extrapolator     = theAtlasExtrapolator, 
                                                                       maxSiD0          = 50.0,
                                                                       maxTrtD0         = 100.,
                                                                       maxSiZ0          = 10000., 
                                                                       maxTrtZ0         = 10000., 
                                                                       minPt            = 500.0,
                                                                       RatioCut1        = 0.,  #0.5,
                                                                       RatioCut2        = 0.,  #0.1,
                                                                       RatioCut3        = 0.)  #0.05)

ToolSvc += InDetConversionTrackSelector
print      InDetConversionTrackSelector

# Track pairs selector
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
InDetConversionTrackPairsSelector = InDet__TrackPairsSelector(name                       = "ConversionTrackPairsSelector",
                                                              ConversionFinderHelperTool = InDetConversionHelper,
                                                              DistanceTool               = InDetConversionTrkDistanceFinder,
                                                              MaxFirstHitRadius          = 500.,
                                                              MaxDistBetweenTracks       = [6.,90.,30.],
                                                              MaxEta                     = [0.5,0.9,0.4],
                                                              MaxInitDistance            = [350.,640.,80.])
ToolSvc += InDetConversionTrackPairsSelector
print      InDetConversionTrackPairsSelector

# Vertex point estimator
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
InDetConversionVtxPointEstimator = InDet__VertexPointEstimator(name                   = "ConversionVtxPointEstimator",
                                                               MaxTrkXYDiffAtVtx      = [6.,8.,2.],
                                                               MaxTrkZDiffAtVtx       = [80.,380.,80.],
                                                               MaxTrkXYValue          = [450.,650.,400.],
                                                               MinArcLength           = [30.,80.,350.],
                                                               MaxArcLength           = [600.,650.,680.],
                                                               MaxChi2OfVtxEstimation = 20.)
ToolSvc += InDetConversionVtxPointEstimator
print      InDetConversionVtxPointEstimator

# Conversion post selector
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
InDetConversionPostSelector = InDet__ConversionPostSelector(name             = "ConversionPostSelector",
                                                            MaxChi2PerTrack  = [80., 100., 50.], #[40.,50.,25.],
                                                            MaxInvariantMass = [60.,50.,25.],
                                                            MinFitMomentum   = [2000.,2000.,2000.],
                                                            MinRadius        = [30.,35.,350.])
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
                                                              MinRatioOfHLhits           = 0.5)
ToolSvc += InDetSingleTrackConversion
print      InDetSingleTrackConversion

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
                                                               Extrapolator               = theAtlasExtrapolator,
                                                               TrackParticleCollection    = "TrackParticleCandidate",
                                                               RemoveTrtTracks            = False,
                                                               IsConversion               = True)
ToolSvc += InDetConversionFinderTools
print      InDetConversionFinderTools



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



from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
theAtlasMagField = Trk__MagneticFieldTool('theAtlasMagField')
ToolSvc += theAtlasMagField

####################################################################################################################

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUphotonSelector
ToolSvc += PAUphotonSelector()
ToolSvc.PAUphotonSelector.OutputLevel = INFO

# --- Photon selection:
#ToolSvc.PAUphotonSelector.GammaTypeNames = [ "unconv" ] # default is [ "unconv" , "conv1" , "conv2" ] 
#ToolSvc.PAUphotonSelector.PtIsolationCut = ...    # Pt cut in MeV (default is 4000; negative value for not using isolation tool)
ToolSvc.PAUphotonSelector.PhotonIDbyCuts = 2   # set itune for G.Unal's photon ID (default==0 => itune2; -1 uses likelihood)
#ToolSvc.PAUphotonSelector.PhotonIDlogLHcut = ...  # set cut for photon ID loglikelihood (default==9.4; works if PhotonIDbyCuts==-1)
# --- Event selection:
#ToolSvc.PAUphotonSelector.PtCut1 = ...  # inclusive selection: pt cut (MeV) for leading photon (default is 40 GeV)
#ToolSvc.PAUphotonSelector.PtCut2 = ...  # inclusive selection: pt cut (MeV) for sub-leading photon (default is 25 GeV)
# --- printout
#ToolSvc.PAUphotonSelector.DumpReco = True    # dump reco e/gammas in file dumpReco.txt (~ 0.8 K/evt)
#ToolSvc.PAUphotonSelector.FileEventSummary = "myEventSummary.txt" # comment out to switch off event summary
#ToolSvc.PAUphotonSelector.FileEventDetails = "myEventDetails.txt" # comment out to switch off event details
#ToolSvc.PAUphotonSelector.DumpTriggerDecision = True    # dump at INFO level, default is False

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthTool
ToolSvc += PAUtruthTool()
# --- Truth handling:
#ToolSvc.PAUtruthTool.EGammasFromTruth = True # all e/gamma are kept (otherwise only gammas from H->gg) (default is true)
#ToolSvc.PAUtruthTool.MuonsFromTruth = True   # all muons are kept (default is true)
#ToolSvc.PAUtruthTool.HadronsFromTruth = True # all hadrons are kept (default is true)
#ToolSvc.PAUtruthTool.PartonsFromTruth = True # all partons are kept (default is false)
#ToolSvc.PAUtruthTool.RTruthConv = ...  # maximum conversion/interaction radius (in mm, default is 800)
#ToolSvc.PAUtruthTool.ZTruthConv = ...  # maximum conversion/interaction z (in mm, default is 2800)
#ToolSvc.PAUtruthTool.PtTruthCut = ...  # reject truth particles produced with pt<PtTruthCut (in MeV)
#ToolSvc.PAUtruthTool.deltaRmatch = ... # deltaR for angular Reco/Truth match (default is 0.1)

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthMatchingTool
ToolSvc += PAUtruthMatchingTool()
#ToolSvc.PAUtruthMatchingTool.OutputLevel = VERBOSE


from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUconversionFlagTool
ToolSvc += PAUconversionFlagTool()
ToolSvc.PAUconversionFlagTool.ExtrapolateToCaloToolName=exToCalo.name()
ToolSvc.PAUconversionFlagTool.VertexFinderToolName=  InDetConversionFinderTools.getFullName()
#ToolSvc.PAUconversionFlagTool.VertexFinderToolName=  InDetConversionFinderToolsBis.getFullName()
ToolSvc.PAUconversionFlagTool.OutputLevel = INFO
#ToolSvc.PAUconversionFlagTool.useBDT=True
#ToolSvc.PAUconversionFlagTool.BDTCut=0.2
#ToolSvc.PAUconversionFlagTool.DBTFileName="TMVAnalysis_BDT.weights.txt"

ToolSvc.PAUconversionFlagTool.SingleTrkConvTool = InDetSingleTrackConversion

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhybridPrimaryVertexTool
ToolSvc += PAUhybridPrimaryVertexTool()
ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolationTool=theAtlasExtrapolator
ToolSvc.PAUhybridPrimaryVertexTool.OutputLevel= INFO
ToolSvc.PAUhybridPrimaryVertexTool.SingleTrkConvTool = InDetSingleTrackConversion
ToolSvc.PAUhybridPrimaryVertexTool.VertexFitterTool = InDetConversionVxFitterToolBis
ToolSvc.PAUhybridPrimaryVertexTool.ExtrapolateToCaloToolName=exToCalo.name()
ToolSvc.PAUhybridPrimaryVertexTool.VertexFinderToolName=  InDetConversionFinderToolsBis.getFullName()
ToolSvc.PAUhybridPrimaryVertexTool.OutputLevel = INFO

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtrackIsolationTool
ToolSvc += PAUtrackIsolationTool()
ToolSvc.PAUtrackIsolationTool.ExtrapolationTool=theAtlasExtrapolator

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggFitter
ToolSvc += PAUhggFitter()
ToolSvc.PAUhggFitter.ExtrapolationTool=theAtlasExtrapolator

from AthenaCommon import CfgMgr
CfgMgr.PAUanalysis(SingleTrkConvTool = InDetSingleTrackConversion)

##########################################
# setup TTree registration svc
# save ROOT histograms and Tuple

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.OutputLevel = INFO
ServiceMgr.THistSvc.Input += ["RefFile DATAFILE='PT2dist.root' OPT='OLD'"]
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='AnalysisSkeleton.aan.root' OPT='RECREATE'"]
# uncomment the following if you need HggUserData in separate file
# then update UDStream options accordingly!
#ServiceMgr.THistSvc.Output += ["Hgg DATAFILE='HggUD.aan.root' OPT='RECREATE'"]

from AnalysisTools.AthAnalysisToolsConf import AANTupleStream

CollectionTreeStream = AANTupleStream( 'CollectionTreeStream' )
topSequence += CollectionTreeStream
topSequence.CollectionTreeStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.CollectionTreeStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.CollectionTreeStream.WriteInputDataHeader = True
topSequence.CollectionTreeStream.OutputLevel = INFO

PAUStream = AANTupleStream( 'PAUStream' )
topSequence += PAUStream
topSequence.PAUStream.StreamName = "AANT" # must be coherent with : PAUanalysis.PAUStreamName
topSequence.PAUStream.TreeName   = "PAUReco"  # must be coherent with : PAUanalysis.PAUTreeName
topSequence.PAUStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.PAUStream.WriteInputDataHeader = True
topSequence.PAUStream.OutputLevel = INFO

UDStream = AANTupleStream( 'UDStream' )
topSequence += UDStream
topSequence.UDStream.StreamName = "AANT" # must be coherent with : PAUanalysis.PAUStreamName
topSequence.UDStream.TreeName   = "HggUserData"  # must be coherent with : PAUanalysis.PAUTreeName
topSequence.UDStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.UDStream.WriteInputDataHeader = True
topSequence.UDStream.OutputLevel = INFO

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

###################### Detail time measurement and auditors
# Use auditors
theApp.AuditAlgorithms=True
theApp.Dlls += [ "GaudiAud" ]
#theApp.EvtMax=250
#ServiceMgr.EventSelector.SkipEvents=25

#include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
#LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
#LSFTimeKeeperSvc.OutputLevel=WARNING

#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "ChronoAuditor"]

#AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
#AthenaPoolCnvSvc.UseDetailChronoStat = TRUE






###########################################   TRIGGER   ###################################################
if (UseTrigger==True and do13030==False):

    #from AthenaCommon.GlobalFlags import GlobalFlags
    #GlobalFlags.DetGeo.set_atlas()

    ## set up trigger decision tool
    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
    tdt = TrigDecisionTool()
    ToolSvc += tdt

    # might be needed for TriggerConfigGetter...
    from RecExConfig.RecFlags  import rec
    rec.readRDO=False
    rec.readAOD=True
    rec.doWriteAOD=False
    rec.doWriteESD=False

    # To read files with trigger config stored as in-file meta-data,
    # i.e. 13.0.40 and above: ds
    # To read AOD produced with 13.0.30 you need to change ds to aod: 
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']

    #Added to check

   # doAOD=False
#    doESD=False
#    doTAG=False
#    doTrth=Truth
#    doTriggerConfigOnly=True
#    include("RecExCommon/RecExCommon_topOptions.py")
#    from TrigAnalysisTest.TrigAnalysisTestConfig import TrigDecisionChecker
#    theTrigDecisionChecker=TrigDecisionChecker()
#    theTrigDecisionChecker.OutputLevel=DEBUG
 #   theTrigDecisionChecker.WriteEventDecision=True
#    topSequence += theTrigDecisionChecker
#    from PerfMonComps.PerfMonFlags import jobproperties
#    jobproperties.PerfMonFlags.doMonitoring = True
    
    # set up trigger config service
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg =  TriggerConfigGetter()
