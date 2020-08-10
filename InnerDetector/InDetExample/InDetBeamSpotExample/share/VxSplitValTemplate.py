# $Id: VxSplitValTemplate.py 216126 2009-09-29 16:12:59Z atlidbs $
#
# Top-level job options file to produce histograms for vertex validation.
# Jobs require a collection of vertices and tracks.  This job will take
# each vertex, split it's tracks into two collections, revertex those tracks,
# and then compare return histograms of interesting quantities.  This
# template is based on InDetRecExample/ReadInDet_jobOptions.py.
#
# Written by Peter Loscutoff in March 2009.

#
# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
#
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'inputfiles' in jobConfig:                    jobConfig['inputfiles'] = [ 'input.root' ]
if not 'evtmax' in jobConfig:                        jobConfig['evtmax'] = -1
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'histfile' in jobConfig:                      jobConfig['outputfile'] = jobConfig['outputfileprefix']+'hist.root'
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
# Template-specific defaults (names follow corresponding job option parameters where possible)
if not 'DetDescrVersion' in jobConfig:               jobConfig['DetDescrVersion'] = 'ATLAS-CSC-02-00-00'
#if not 'PropagatorType' in jobConfig:               This defaults to whatever is used in InDetRecExample
#if not 'PrimaryVertexAlg' in jobConfig:             This defaults to whatever is used in InDetRecExample
if not 'nVtxPerEvent' in jobConfig:                  jobConfig['nVtxPerEvent'] = 1
if not 'priVtxOnly' in jobConfig:                    jobConfig['priVtxOnly'] = True
if not 'doTruth' in jobConfig:                       jobConfig['doTruth'] = False
if not 'doAxis' in jobConfig:                        jobConfig['doAxis'] = True
#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = jobConfig['outputlevel']
  
#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
DetDescrVersion = jobConfig['DetDescrVersion']

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource  = 'geant4'
globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
#jobproperties.Beam.numberOfCollisions = 0.0

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows
#DetFlags.pixel_setOff()
#DetFlags.SCT_setOff()
#DetFlags.TRT_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
if 'PropagatorType' in jobConfig.keys(): InDetFlags.propagatorType = jobConfig['PropagatorType']
if 'PrimaryVertexAlg' in jobConfig.keys(): InDetFlags.primaryVertexSetup = jobConfig['PrimaryVertexAlg']
InDetFlags.doPrintConfigurables = True

# Beam constraint and primary vertex finding
InDetFlags.useBeamConstraint = False
InDetFlags.doPrimaryVertex3DFinding = True

printfunc ("InDetRec_jobOptions: InDetKeys not set - setting to defaults")
from InDetRecExample.InDetKeys import InDetKeys

#--------------------------------------------------------------
# load tools and fill AlgSequence
#--------------------------------------------------------------

#--------------------------------------------------------------
# load ServiceMgr, ToolSvc, topSequence
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#
from AthenaCommon.AppMgr import ToolSvc

#--------------------------------------------------------------
# change MessageSvc 
#--------------------------------------------------------------

# output level
ServiceMgr.MessageSvc.OutputLevel  = OutputLevel
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# run splitter algorithm to create the neccecary track collections
#--------------------------------------------------------------

# set up pool reading
import AthenaPoolCnvSvc.ReadAthenaPool

from InDetVertexSplitter.InDetVertexSplitterConf import InDet__InDetVertexSplitter
InDetVertexSplitter = InDet__InDetVertexSplitter()
InDetVertexSplitter.MaxVertexNumber = jobConfig['nVtxPerEvent']
InDetVertexSplitter.PrimaryOnly = jobConfig['priVtxOnly']
InDetVertexSplitter.OutputLevel = OutputLevel

topSequence += InDetVertexSplitter
printfunc (         InDetVertexSplitter)


#--------------------------------------------------------------
# Load POOL support, setup for reconstruction
#--------------------------------------------------------------

# --- GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
include("BFieldAth/BFieldAth_jobOptions.py")

# LEVEL 5: Extrapolator

include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

if InDetFlags.propagatorType() is "STEP":
  from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
else:
  from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetPropagator = Propagator(name = 'InDetPropagator')
if InDetFlags.propagatorType() is "RungeKutta":
  InDetPropagator.AccuracyParameter = 0.0001
ToolSvc += InDetPropagator

# set up the propagator for outside ID (A.S. needed as a fix for 14.5.0 )
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
InDetStepPropagator = StepPropagator(name = 'InDetStepPropagator')
ToolSvc += InDetStepPropagator
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetPropagator)

#
# Setup the Navigator (default, could be removed)
#
from TrkExTools.TrkExToolsConf import Trk__Navigator
InDetNavigator = Trk__Navigator(name                = 'InDetNavigator',
                                TrackingGeometrySvc = AtlasTrackingGeometrySvc)
ToolSvc += InDetNavigator
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetNavigator)

#
# Setup the MaterialEffectsUpdator
#
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
if not InDetFlags.solenoidOn():
  InDetMaterialUpdator.EnergyLoss = False
  InDetMaterialUpdator.ForceMomentum = True
  InDetMaterialUpdator.ForcedMomentumValue = 1000*MeV

ToolSvc += InDetMaterialUpdator

if InDetFlags.doPrintConfigurables:
  printfunc (     InDetMaterialUpdator)

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
 
InDetSubPropagators = []
InDetSubUpdators = []
 
# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
InDetSubPropagators += [ InDetPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
 
# default for Calo is (Rk,MatLandau)
InDetSubPropagators += [ InDetPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
 
# default for MS is (STEP,Mat)
InDetSubPropagators += [ InDetStepPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
#----------------------------------------------------------------------------------------------------------            

from TrkExTools.TrkExToolsConf import Trk__Extrapolator
InDetExtrapolator = Trk__Extrapolator(name                    = 'InDetExtrapolator',
                                      Propagators             = [ InDetPropagator, InDetStepPropagator ],
                                      MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                      Navigator               = InDetNavigator,
                                      SubPropagators          = InDetSubPropagators,
                                      SubMEUpdators           = InDetSubUpdators,
                                      DoCaloDynamic           = False)
ToolSvc += InDetExtrapolator
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetExtrapolator  )


# LEVEL 4

from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                      PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap())
ToolSvc += InDetPrdAssociationTool
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetPrdAssociationTool)

from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
InDetBoundaryCheckTool = InDet__InDetBoundaryCheckTool(
    name="InDetBoundaryCheckTool",
    UsePixel=DetFlags.haveRIO.pixel_on(),
    UseSCT=DetFlags.haveRIO.SCT_on()
)
ToolSvc += InDetBoundaryCheckTool

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                      Extrapolator = InDetExtrapolator,
                                                      BoundaryCheckTool=InDetBoundaryCheckTool)

ToolSvc += InDetHoleSearchTool
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetHoleSearchTool)


from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = "InDetSummaryHelper",
                                                                 AssoTool     = InDetPrdAssociationTool,
                                                                 DoSharedHits = False,
                                                                 HoleSearch   = InDetHoleSearchTool)
ToolSvc += InDetTrackSummaryHelperTool
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetTrackSummaryHelperTool                                                                   )


from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name         = "InDetImpactPoint3dEstimator",
                                                          Extrapolator = InDetExtrapolator)
ToolSvc += InDetImpactPoint3dEstimator
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetImpactPoint3dEstimator)


if InDetFlags.magField() is "fast" :
  from TrkMagFieldTools_xk.TrkMagFieldTools_xkConf import Trk__MagneticFieldTool_xk
  InDetMagField = Trk__MagneticFieldTool_xk('InDetMagField')
else:
  from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
  InDetMagField = Trk__MagneticFieldTool('InDetMagField')
ToolSvc += InDetMagField
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetMagField)


# LEVEL 3

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                              InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                              doSharedHits           = False,
                                              doHolesInDet           = True)
#InDetTrackSummaryTool.OutputLevel = DEBUG
ToolSvc += InDetTrackSummaryTool
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetTrackSummaryTool)


from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
InDetAnnealingMaker = Trk__DetAnnealingMaker(name = "InDetAnnealingMaker")
InDetAnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
ToolSvc += InDetAnnealingMaker
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetAnnealingMaker)


from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dAtaPlaneFactory
InDetIp3dAtaPointFactory = Trk__ImpactPoint3dAtaPlaneFactory(name                   = "InDetIp3dAtaPointFactory",
                                                               Extrapolator           = InDetExtrapolator,
                                                               ImpactPoint3dEstimator = InDetImpactPoint3dEstimator)
ToolSvc += InDetIp3dAtaPointFactory
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetIp3dAtaPointFactory)


from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
InDetLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetFullLinearizedTrackFactory",
                                                  Extrapolator      = InDetExtrapolator,
                                                  MagneticFieldTool = InDetMagField)
ToolSvc += InDetLinFactory
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetLinFactory)


# LEVEL 2

if (InDetFlags.doPrimaryVertex3DFinding()):
  from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
  InDetVtxSeedFinder = Trk__CrossDistancesSeedFinder(name = "InDetCrossDistancesSeedFinder"
                                                       #Mode1dFinder = # default, no setting needed
                                                       )
else:
  from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
  InDetVtxSeedFinder = Trk__ZScanSeedFinder(name = "InDetZScanSeedFinder"
                                              #Mode1dFinder = # default, no setting needed
                                              )
    
ToolSvc += InDetVtxSeedFinder
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetVtxSeedFinder)


#
# ------ load new track selector (common for all vertexing algorithms, except for the moment VKalVrt)
#
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                                               pTMin                = 500,
                                                               IPd0Max              = 4,
                                                               IPz0Max              = 1000,
                                                               z0Max                = 1000,
                                                               sigIPd0Max           = 0.35,
                                                               sigIPz0Max           = 2.5,
                                                               d0significanceMax    = -1.,#problem is that you cannot use the significance at all!
                                                               z0significanceMax    = -1.,
                                                               etaMax               = 9999.,
                                                               useTrackSummaryInfo  = True,
                                                               nHitBLayer           = 0,
                                                               nHitPix              = 1,
                                                               nHitBLayerPlusPix    = 0,
                                                               nHitSct              = 5,
                                                               nHitSi               = 7,
                                                               nHitTrt              = 0,
                                                               nHitTrtHighEFractionMax = 1,
                                                               nHitTrtHighEFractionWithOutliersMax = 1,
                                                               useSharedHitInfo     = False,
                                                               useTrackQualityInfo  = True,
                                                               fitChi2OnNdfMax      = 3.5,
                                                               TrtMaxEtaAcceptance  = 1.9,
                                                               TrackSummaryTool     = InDetTrackSummaryTool,
                                                               Extrapolator         = InDetExtrapolator)
ToolSvc += InDetTrackSelectorTool

if InDetFlags.primaryVertexSetup() == 'DefaultFastFinding':
  #
  # load fast Billoir fitter
  #
  from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
  InDetVxFitterTool = Trk__FastVertexFitter(name                   = "InDetFastVertexFitterTool",
                                            LinearizedTrackFactory = InDetLinFactory,
                                            Extrapolator           = InDetExtrapolator)

elif InDetFlags.primaryVertexSetup() == 'DefaultFullFinding':
  #
  # load full billoir fitter
  #
  from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FullVertexFitter
  InDetVxFitterTool = Trk__FullVertexFitter(name                    = "InDetFullVertexFitterTool",
                                            LinearizedTrackFactory  = InDetLinFactory,
                                            Extrapolator            = InDetExtrapolator)

elif InDetFlags.primaryVertexSetup() == 'DefaultKalmanFinding':
  #
  # case default finding with Kalman filter requested 
  #
  
  from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
  InDetVertexSmoother = Trk__SequentialVertexSmoother()
  ToolSvc += InDetVertexSmoother
  if InDetFlags.doPrintConfigurables:
    printfunc (     InDetVertexSmoother)
    
  from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
  InDetVxFitterTool = Trk__SequentialVertexFitter(name                   = "InDetSequentialVxFitterTool",
                                                  LinearizedTrackFactory = InDetLinFactory,
                                                  VertexSmoother         = InDetVertexSmoother
                                                  #VertexUpdator   = # no setting required 
                                                  )
  
elif InDetFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
  #
  # load configured adaptive vertex fitter
  #
  from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
  InDetVxFitterTool = Trk__AdaptiveVertexFitter(name                         = "InDetAdaptiveVxFitterTool",
                                                SeedFinder                   = InDetVtxSeedFinder,
                                                LinearizedTrackFactory       = InDetLinFactory,
                                                ImpactPoint3dAtaPlaneFactory = InDetIp3dAtaPointFactory,
                                                AnnealingMaker               = InDetAnnealingMaker)
  
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
  #
  #load adaptive multi vertex fitter
  #
  from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveMultiVertexFitter
  InDetVxFitterTool = Trk__AdaptiveMultiVertexFitter(name                         = "InDetAdaptiveMultiVertexFitter",
                                                     LinearizedTrackFactory       = InDetLinFactory,
                                                     ImpactPoint3dAtaPlaneFactory = InDetIp3dAtaPointFactory,
                                                     AnnealingMaker               = InDetAnnealingMaker,
                                                     DoSmoothing                  = True) # false is default
                                                     
elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
  #
  #load vkal fitter
  #
  from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
  InDetVxFitterTool = Trk__TrkVKalVrtFitter(name = "InDetVKalVrtFitter")
                                                     
ToolSvc += InDetVxFitterTool
if InDetFlags.doPrintConfigurables:
  printfunc (     InDetVxFitterTool)
#
# ----- load vertex finder tool
#

# LEVEL 1

if not (InDetFlags.primaryVertexSetup() == 'AdaptiveFinding') and not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding'):
  #
  # load primary vertex finder tool
  #
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
  InDetPriVxFinderTool = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool",
                                                     VertexFitterTool  = InDetVxFitterTool,
                                                     TrackSelector     = InDetTrackSelectorTool,
                                                     useBeamConstraint = InDetFlags.useBeamConstraint())
  if jobproperties.Beam.zeroLuminosity():
    InDetPriVxFinderTool.enableMultipleVertices = 0;
  else:
    InDetPriVxFinderTool.enableMultipleVertices = 1;
                                                          
elif InDetFlags.primaryVertexSetup() == 'AdaptiveFinding':
  #
  # load adaptive primary vertex finder
  #

  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptivePriVxFinderTool
  InDetPriVxFinderTool = InDet__InDetAdaptivePriVxFinderTool(name             = "InDetAdaptivePriVxFinderTool",
                                                             VertexFitterTool = InDetVxFitterTool,
                                                             TrackSelector    = InDetTrackSelectorTool
                                                             )
                                                          
elif InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding':
  #
  # load adaptive multi primary vertex finder
  #
  from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
  InDetPriVxFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool",
                                                                  SeedFinder        = InDetVtxSeedFinder,
                                                                  VertexFitterTool  = InDetVxFitterTool,
                                                                  TrackSelector     = InDetTrackSelectorTool,
                                                                  useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                  selectiontype     = 0,
                                                                  do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())


ToolSvc += InDetPriVxFinderTool
if InDetFlags.doPrintConfigurables:
  printfunc (InDetPriVxFinderTool)
#
# load primary vertex finding algorithm and configure it 
#

# LEVEL 0

from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
InDetPriVxFinders = {}
for i in range(1,4):
  for type in ("even", "odd", "all"):
    InDetPriVxFinders[type+str(i)] = InDet__InDetPriVxFinder(name                   = "InDetPriVxFinder_"+type+"_"+str(i),
                                                             VertexFinderTool       = InDetPriVxFinderTool,
                                                             TracksName             = type+"_"+str(i)+"_TrackParticleCandidate",
                                                             VxCandidatesOutputName = type+"_"+str(i)+"_VxPrimaryCandidate")
    topSequence += InDetPriVxFinders[type+str(i)]
    if InDetFlags.doPrintConfigurables:
      printfunc (InDetPriVxFinders[type+str(i)])

InDetPriVxFinderParent = InDet__InDetPriVxFinder(name                   = "InDetPriVxFinderParent",
                                                 VertexFinderTool       = InDetPriVxFinderTool,
                                                 TracksName             = "TrackParticleCandidate",
                                                 VxCandidatesOutputName = "parentVxPrimaryCandidate")
topSequence += InDetPriVxFinderParent
if InDetFlags.doPrintConfigurables:
  printfunc (InDetPriVxFinderParent)

# ------------------------------------------------------------
# switch off history service and no dump
# ------------------------------------------------------------

  
from InDetVertexSplitter.InDetVertexSplitterConf import InDet__InDetVertexSplitterHist
InDetVertexSplitterHist = InDet__InDetVertexSplitterHist()
InDetVertexSplitterHist.MaxVertexNumber = jobConfig['nVtxPerEvent']
InDetVertexSplitterHist.doTruth = jobConfig['doTruth']
InDetVertexSplitterHist.doAxis = jobConfig['doAxis']

InDetVertexSplitterHist.OutputLevel = OutputLevel

topSequence += InDetVertexSplitterHist
printfunc (         InDetVertexSplitterHist)

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+jobConfig['histfile']+"' OPT='RECREATE'"] 



# ------------------------------------------------------------
# switch off history service and no dump
# ------------------------------------------------------------

ServiceMgr.StoreGateSvc.ActivateHistory = False
ServiceMgr.StoreGateSvc.Dump            = False


#--------------------------------------------------------------
# Read alignment constants, if desired
#--------------------------------------------------------------

#--------------------------------------------------------------
# TrackSelector defaults for beamspot work
#--------------------------------------------------------------

ToolSvc = Service("ToolSvc")
ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 25
ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 200
ToolSvc.InDetDetailedTrackSelectorTool.nHitBLayer = 0
ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 0
ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 4
printfunc (ToolSvc.InDetDetailedTrackSelectorTool)

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
IOVDbSvc=theApp.service('IOVDbSvc')
theApp.EvtMax  = jobConfig['evtmax']

ServiceMgr.EventSelector.InputCollections = jobConfig['inputfiles']
