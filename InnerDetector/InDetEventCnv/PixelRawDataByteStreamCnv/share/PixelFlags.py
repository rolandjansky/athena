###################################################################################
# ConfiguredInDetFlags
###################################################################################
# Python Setup Class for InnerDetector reconstruction
#
# Author: Markus.Elsing@cern.ch
#
###################################################################################

class ConfiguredInDetFlags :

  def __init__ (self):
    
    # -------------------------------------------------------------------
    # ESD/AOD output options
    # -------------------------------------------------------------------
    self.__restrictedESD = False
    self.__AODall        = False
    
    if self.__AODall:
      InDetJobProperties.newTracking = False
    #
    # --------------------------------------------------------------------
    # ---- Loading of Services
    # --------------------------------------------------------------------
    #
    # load TrackingGeometry
    self.__loadTrackingGeometry    = True
    #
    # use beam spot service in new tracking
    self.__useBeamConstraint       = False
    #
    # --------------------------------------------------------------------
    # ---- Loading of Tools
    # --------------------------------------------------------------------
    #
    # load RIO_OnTrackCreator
    self.__loadRotCreator     = False
    #
    # configure updator for new tracking
    self.__loadUpdator        = False
    # ---> control which updator to load for KalmanFitter (None/"fast"/weight")
    self.__kalmanUpdator      = None    # use default (is KalmanUpdator with gain formalism)
    # self.__kalmanUpdator      = "fast"  # this is the fast KalmanUpdator_xk
    # self.__kalmanUpdator      = "weight"  # KalmanUpdator with weight formalism
    #
    # configure extrapolator for new tracking
    self.__loadExtrapolator        = False
    # ---> control which field tool to use (None/"fast")
    self.__magField           = None    # use default field tool
    # self.__magField           = "fast"  # this uses the FieldTool_xk
    # ---> control which propagator to use ('RungeKutta'/'STEP')
    self.__propagatorType     = 'RungeKutta'
    # self.__propagatorType     = 'STEP'
    #
    # configure fitter for new tracking
    self.__loadFitter         = False
    # ---> control which fitter to be used
    self.__trackFitterType    = 'KalmanFitter'
    # self.__trackFitterType    = 'KalmanDNAFitter'
    # self.__trackFitterType    = 'DistributedKalmanFilter'
    # self.__trackFitterType    = 'GlobalChi2Fitter'
    # self.__trackFitterType    = 'GaussianSumFilter'
    #
    # control if to load dynamic layer creator
    if (self.__trackFitterType is 'GlobalChi2Fitter') : # or iPatRec:
      self.__loadDynamicLayerCreator = False
    else:
      self.__loadDynamicLayerCreator = False
    #
    #
    # configure summary for new tracking
    self.__loadAssoTool       = False
    #    
    # configure summary for new tracking
    self.__loadSummaryTool    = False
    # ---> do holes search from now on in summry tool !
    self.__doHolesOnTrack     = False
    #
    # --------------------------------------------------------------------
    # ---- enable Truth
    # --------------------------------------------------------------------
    #
    # control whether to do truth association
    self.__doTruth            = False
    #
    # --------------------------------------------------------------------
    # ---- preProcessing algorithms
    # --------------------------------------------------------------------
    #
    # Controls whether to do PrepRawData/RIO Formation (default=true)
    self.__doPRDFormation          = True
    # Control whether to do SpacePointFormation (default=true)
    self.__doSpacePointFormation   = False
    #
    # --------------------------------------------------------------------
    # ---- legacy pattern algorithms plus conversions
    # --------------------------------------------------------------------
    #
    # control if legacy conversion is needed
    self.__doLegacyConversion      = False
    #
    # --------------------------------------------------------------------
    # ---- NewTracking algorithms
    # --------------------------------------------------------------------
    #
    # control NewTracking itself
    self.__doNewTracking           = False
    #
    # control whether to run SiSPSeededTrackFinder
    self.__doSiSPSeededTrackFinder = False
    # ---> start with Zvertex finding
    self.__useZvertexTool          = False
    #
    # run ambiguity solution
    self.__doAmbiSolving           = False
    #
    # control to TRT extension
    self.__doTRTExtension          = False
    # ---> which extension type ("xk"/"DAF")
    self.__trtExtensionType        = "xk"
    # use smart TRT LR/tube hit creator and redo ROTs
    self.__redoTRT_LR              = False
    #
    # control to run TrackExtensionProcessor
    self.__doExtensionProcessor    = False
    #
    # --------------------------------------------------------------------
    # --- 2nd interation, outside in tracking
    # --------------------------------------------------------------------
    #
    # control back tracking
    self.__doBackTracking         = False
    #
    # control to run TRT Segment finding (do it always after new tracking!)
    self.__doTrtSegments          = False
    #
    # control running the back tracking
    self.__doTRTSeededTrackFinder = False
    # control which SP finder is used
    self.__loadTRTSeededSPFinder        = False
    self.__loadSimpleTRTSeededSPFinder  = False
    #
    # control running the ambi on back tracking
    self.__doResolveBackTracks    = False
    #
    # control TRT Standalone
    self.__doTRTStandalone        = False
    #
    # control TRT Standalone
    self.__doAddCollections       = False
    #
    # --------------------------------------------------------------------
    # ---- Refit of tracks
    # --------------------------------------------------------------------
    #
    # control if a refit is done
    self.__doRefit                 = False
    # ---> control if refit is done from PRD or ROT
    self.__refitROT                = False
    #
    # --------------------------------------------------------------------
    # ---- slimming or copy into "Tracks" container
    # --------------------------------------------------------------------
    #
    self.__doSlimming              = False
    #
    # --------------------------------------------------------------------
    # ---- PostProcessing algorithms
    # --------------------------------------------------------------------
    #
    # control if run vertexing
    self.__doVertexFinding          = False
    # ---> options related to the vertex finding:
    #      selecting one concrete strategy and fitter implementation
    #      adaptive multi vertex finder (new default)
    self.__doAdaptiveMultiFinding   = False  # default
    self.__doAdaptiveFinding        = False
    self.__doDefaultFastFinding     = False
    self.__doDefaultFullFinding     = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doDefaultVKalVrtFinding  = False
    #
    # control whether to create TrackParticles (default=true)
    self.__doParticleCreation       = False
    # ---> control if the shared hits are recorded in TrackPatricles
    self.__doSharedHits             = False
    #
    # control to run InDetV0Finder
    self.__doV0Finder               = False
    # control to run InDetV0Finder
    self.__doConversions            = False
    # control to run InDetBremFitAlg
    self.__doBremFitAlg             = False
    #
    # --------------------------------------------------------------------
    # ---- Statistics
    # --------------------------------------------------------------------
    #
    # control whether to do statistics package
    self.__doStatistics            = False
    # ---> some setting
    self.__doStatNtuple            = False
    self.__StatNtupleName          = "InDetRecStatistics.root"  
    #
    # --------------------------------------------------------------------
    # --- some configuration of options for the new tracking
    # --------------------------------------------------------------------
    #
    #
    # control material effects in extrapolator/fitter (True/False) - needs fixing
    self.__materialInteractions  = False # use Pion mass
    #self.__materialInteractions  = False
    
# --------------------------------------------------------------------
# ---- some methods to deal with different fitters
# --------------------------------------------------------------------

  def __isValidTrackFitter ( self, fitterType ):
    if        (self.__trackFitterType is 'KalmanFitter') \
           or (self.__trackFitterType is 'KalmanDNAFitter') \
           or (self.__trackFitterType is 'DistributedKalmanFilter') \
           or (self.__trackFitterType is 'GlobalChi2Fitter' ) \
           or (self.__trackFitterType is 'GaussianSumFilter') :
      return fitterType
    else:
      print 'ConfiguredInDetFlags.py       WARNING unregistered or invalid track fitter type given.'
      print '                                      --> re-setting to TrkKalmanFitter.'
      return 'KalmanFitter'

  # change default track fitter used for all ID fits
  def setTrackFitterType ( self, trackFitterType ) :
    self.__trackFitterType = self.__isValidTrackFitter(trackFitterType)
    if self.__trackFitterType is 'GlobalChi2Fitter':
      self.__loadDynamicLayerCreator = True
    if not self.__doNewTracking :
      print 'ConfiguredInDetFlags.py       WARNING changing the track fitter has no effect'
      print '                                      because the new tracking IS NOT SWITCHED ON!'

# ----------------------------------------------------------------------------
# --- ESD all kludge
# ----------------------------------------------------------------------------

  def setAODall (self) :
    self.__AODall = True
    if not self.__doNewTracking:
      print "ERROR - inconsistent AOD all setup - NT needed"
      raise None
    
# ----------------------------------------------------------------------------
# --- ESD/AOD output options
# ----------------------------------------------------------------------------

  def setRestrictedESD (self) :
    self.__restrictedESD = True

  def restrictedESD (self) :
    return self.__restrictedESD

  def AODall (self) :
    return self.__AODall

# ----------------------------------------------------------------------------
# --- set methods for less often used switches
# ----------------------------------------------------------------------------

  # decide if the SiSpacePoint track finder should look for primary vertices
  # in z and use this constraint during pattern recognition
  def usePrimVertexZcoordinate ( self, usezvertex ) :
    self.__useZvertexTool = usezvertex
    if not self.__doNewTracking :
      print 'ConfiguredInDetFlags.py       WARNING toggling the z-vertex constraint has no effect'
      print '                                      because the new tracking IS NOT SWITCHED ON!'

  def enableBackTracking(self):
    self.__doBackTracking         = True
    self.__doTrtSegments          = True
    self.__doTRTSeededTrackFinder = True 
    self.__doResolveBackTracks    = True
    self.__doTRTStandalone        = True
    self.__doAddCollections       = True

  def enableV0Finder (self) :
    self.__doV0Finder   = True

  def enableConversions (self) :
    self.__doConversions = True

  def enableBremFitAlg (self) :
    self.__doBremFitAlg = True

  # control the validation & ntuple output based on InDetRecStatistics
  def enableStatistics ( self, doStatNtuple = True, StatNtupleName = "InDetRecStatistics.root" ) :
    self.__doStatistics            = True
    self.__doStatNtuple            = doStatNtuple
    self.__StatNtupleName          = StatNtupleName     

  # control Track slimming
  def enableTrackSlimming (self, useslimming) :
    self.__doSlimming = useslimming

# ----------------------------------------------------------------------------
# --- set different vertexing options
# ----------------------------------------------------------------------------

  def  enableAdaptiveFinding(self) :
    self.__doAdaptiveFinding  = True
    self.__doDefaultFastFinding  = False
    self.__doDefaultFullFinding  = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = False
    
  def  enableAdaptiveMultiFinding(self) :
    self.__doAdaptiveFinding  = False
    self.__doDefaultFastFinding  = False
    self.__doDefaultFullFinding  = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = True
    self.__doDefaultVKalVrtFinding = False

  def  enableDefaultFullFinding(self) :
    self.__doAdaptiveFinding  = False
    self.__doDefaultFastFinding  = False
    self.__doDefaultFullFinding  = True
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = False

  def  enableDefaultFastFinding(self) :
    self.__doAdaptiveFinding  = False
    self.__doDefaultFastFinding  = True
    self.__doDefaultFullFinding  = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = False
    
  def  enableDefaultKalmanFinding(self) :
    self.__doAdaptiveFinding       = False
    self.__doDefaultFastFinding    = False
    self.__doDefaultFullFinding    = False
    self.__doDefaultKalmanFinding   = True
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = False

  def  enableDefaultAdaptiveFinding(self) :
    self.__doAdaptiveFinding       = False
    self.__doDefaultFastFinding    = False
    self.__doDefaultFullFinding    = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = True
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = False
  
  def  enableDefaultVKalVrtFinding(self) :
    self.__doAdaptiveFinding       = False
    self.__doDefaultFastFinding    = False
    self.__doDefaultFullFinding    = False
    self.__doDefaultKalmanFinding   = False
    self.__doDefaultAdaptiveFinding = False
    self.__doAdaptiveMultiFinding = False
    self.__doDefaultVKalVrtFinding = True

# ----------------------------------------------------------------------------
# --- return methods which do the actual steering inside the IndetRec_options
# ----------------------------------------------------------------------------

  # return
  def doPRDFormation(self) :
    return self.__doPRDFormation

  # return
  def doSpacePointFormation(self) : 
    return self.__doSpacePointFormation


  # return
  def doLegacyConversion(self) :
    return self.__doLegacyConversion

  # return
  def doNewTracking(self) :
    return self.__doNewTracking

  # return
  def doSiSPSeededTrackFinder(self) :
    return self.__doSiSPSeededTrackFinder

  # return
  def useZvertexTool(self) :
    return self.__useZvertexTool

  # return
  def doRefit(self) :
    return self.__doRefit

  # return
  def doSlimming(self) :
    return self.__doSlimming

  # return
  def doAmbiSolving(self) :
    return self.__doAmbiSolving

  # return
  def doTRTExtension(self) :
    return self.__doTRTExtension

  # return
  def doExtensionProcessor(self) :
    return self.__doExtensionProcessor

  # return
  def doBackTracking(self) :
    return self.__doBackTracking

  # return
  def doResolveBackTracks(self) :
    return self.__doResolveBackTracks

  # return
  def doTrtSegments(self) :
    return self.__doTrtSegments

  # return
  def doTRTSeededTrackFinder(self):
    return self.__doTRTSeededTrackFinder

  # return
  def doTRTStandalone(self):
    return self.__doTRTStandalone

  # return
  def doAddCollections(self):
    return self.__doAddCollections

  # return
  def loadTRTSeededSPFinder(self):
    return self.__loadTRTSeededSPFinder

  # return
  def loadSimpleTRTSeededSPFinder(self):
    return self.__loadSimpleTRTSeededSPFinder

  # return
  def doV0Finder(self) :
    return self.__doV0Finder
  
  # return
  def doConversions(self) :
    return self.__doConversions
  
  # return
  def doBremFitAlg(self) :
    return self.__doBremFitAlg
  
  # return
  def doVertexFinding(self) :
    return self.__doVertexFinding
  
  # return
  def doAdaptiveMultiFinding(self):
    return self.__doAdaptiveMultiFinding

  # return
  def doAdaptiveFinding(self):
    return self.__doAdaptiveFinding
  
  # return
  def doDefaultFastFinding(self):
    return self.__doDefaultFastFinding
  
  #return
  def doDefaultFullFinding(self):
    return self.__doDefaultFullFinding
  
  # return
  def doDefaultKalmanFinding(self):
    return self.__doDefaultKalmanFinding
  
  # return
  def doDefaultAdaptiveFinding(self):
    return self.__doDefaultAdaptiveFinding
  
  # return
  def doDefaultVKalVrtFinding(self):
    return self.__doDefaultVKalVrtFinding
  
  # return
  def doParticleCreation(self) :
    return self.__doParticleCreation
  
  # return
  def doHolesOnTrack(self) :
    return self.__doHolesOnTrack
  
  # return
  def doSharedHits(self) :
    return self.__doSharedHits

  # return
  def doTruth(self) :
    return self.__doTruth

  # return
  def doStatistics(self) :
    return self.__doStatistics

  # return
  def doStatNtuple(self) :
    return self.__doStatNtuple

  #return
  def StatNtupleName(self) :
    return self.__StatNtupleName
   
  # return
  def loadTrackingGeometry(self) :
    return self.__loadTrackingGeometry

  # return
  def useBeamConstraint(self) :
    return self.__useBeamConstraint

  # return
  def loadRotCreator(self) :
    return self.__loadRotCreator

  # return
  def loadExtrapolator(self) :
    return self.__loadExtrapolator

  # return
  def materialInteractions(self) :
    return self.__materialInteractions

  # return
  def propagatorType(self) :
    return self.__propagatorType

  # return
  def loadUpdator(self) :
    return self.__loadUpdator

  # return
  def loadFitter(self) :
    return self.__loadFitter

  # return
  def trackFitterType(self) :
    return self.__trackFitterType

  def loadDynamicLayerCreator(self) :
    return self.__loadDynamicLayerCreator

  def trtExtensionType(self) :
    return self.__trtExtensionType

  def redoTRT_LR(self) :
    return self.__redoTRT_LR

  # return
  def loadAssoTool(self) :
    return self.__loadAssoTool

  # return
  def loadSummaryTool(self) :
    return self.__loadSummaryTool

  # return
  def refitROT(self) :
    return self.__refitROT
  
  # return
  def kalmanUpdator(self) :
    return self.__kalmanUpdator

  # return
  def magField(self) :
    return self.__magField

# ----------------------------------------------------------------------------
# --- Printout of settings
# ----------------------------------------------------------------------------

  def printInfo(self) :
    print '****** Inner Detector Flags ********************************************************'
    if self.__restrictedESD :
      print '*'
      print '* --------------------> ESD output list restricted !'
    if self.__AODall :
      print '*'
      print '* --------------------> write AOD classes for alll trackers !'
    print '*'
    print '* PreProcessing:'
    print '* =============='
    print '*'
    if self.__doPRDFormation :
      print '* - run PrepRawDataFormation'
    if self.__doSpacePointFormation :
      print '* - run SpacePointFormation'
    print '*'
    print '* TrackFinding:'
    print '* ============='
    print '*'
    if self.__doLegacyConversion :
      print '* - convert output to new EDM (TrkTrack)'
    print '*'
    if self.__doNewTracking :
      print '* following algorithms from newTracking are executed:'
    if self.__doSiSPSeededTrackFinder :
      print '* - run SiSPSeededTrackFinder (part of RTF tracking)'
      if self.__useZvertexTool :
        print '*   and use ZvertexTool !'
    if self.__doAmbiSolving :
      print '* - run AmbiguitySolver'
    if self.__doTRTExtension :
      print '* - run TRT_TrackExtension'
    if self.__doExtensionProcessor :
      print '* - run TrackExtensionProcessor'
      if self.__redoTRT_LR :
        print '*   and redo LR and tube hits in fit for TRT !!!'
    print '*'
    if self.__doTrtSegments :
      print '* - run TRT Segment finding'
    if self.__doTRTSeededTrackFinder :
      print '* - run TRT seeded track finding'
      if self.__loadTRTSeededSPFinder :
        print '*   and load TRT_SeededSpacePointFinder'
      if self.__loadSimpleTRTSeededSPFinder :
        print '*   and load SimpleTRT_SeededSpacePointFinder'
      if self.__doResolveBackTracks :
        print '* - run ambi on TRT seeded tracks'
    if self.__doTRTStandalone :
      print '* - create TRT standalone tracks'
    if self.__doAddCollections :
      print '* - add forward and back tracking plus TRT only'
    print '*'
    if self.__doRefit :
      print '* - do a refit of all tracks'
      print '*'
    if self.__doSlimming :
      print '* - slim down the tracks for output on ESD'
      print '*'
    print '* PostProcessing:'
    print '* ==============='
    print '*'
    if self.__doVertexFinding :
      print '* - run primary vertex finding'
      if self.__doAdaptiveFinding :
        print '*   adaptive finding requested'
      if self.__doDefaultFastFinding :
        print '*   run default PV finding with fast Billoir vertex fit'
      if self.__doDefaultFullFinding :
        print '*   run default PV finding with full Billoir vertex fit'
      if self.__doDefaultKalmanFinding :
        print '*   run default PV finding with Kalman vertex fit'
      if self.__doDefaultAdaptiveFinding :
        print '*   run default PV finding with Adaptive vertex fit'
      if self.__doDefaultVKalVrtFinding :
        print '*   run default PV finding with VKalVrt vertex fit'
    if self.__doParticleCreation :
      print '* - do Particle creation at end of event reconstruction (AOD)'
      if self.__doSharedHits :
        print '*   and do shared hits search'
    print '*'
    if self.__doV0Finder:
      print '* - run V0 finder'
    if self.__doConversions:
      print '* - run conversion finder'
    if self.__doBremFitAlg:
      print '* - run electron brem fitting prototype'
    print '*'
    if self.__doTruth :
      print '* - run truth association'
    print '*'
    if self.__doStatistics :
      print '* - run statistics packages'
      if self.__doStatNtuple :
        print '*   and create nutple'
    print '*'
    print '* Configurable Services loaded:'
    print '* ============================='
    print '*'
    if self.__loadTrackingGeometry :
      print '* - load tracking geometry for Inner Detector'
    print '*'
    if self.__useBeamConstraint :
      print '* - use Beam Spot Constraint in reconstruction/vertexing'
    print '*'
    print '* Configurable Tools loaded:'
    print '* =========================='
    print '*'
    if self.__loadRotCreator :
      print '* - load ROT_Creator for Inner Detector'
    if self.__loadExtrapolator :
      print '* - load Extrapolator'
      if self.__propagatorType is 'RungeKutta' :
        print '*   load Runge Kutta propagator'
      elif self.__propagatorType is 'STEP' :
        print '*   load STEP propagator'
      if self.__magField is "fast" :
        print '*   load MagFieldTool_xk'
      else :
        print '*   load MagFieldTool'
      if self.__materialInteractions :
        print '*   use material corrections in extrapolation and fit'
    if self.__loadUpdator :
      if self.__kalmanUpdator is "fast" :
        print '* - load MeasurementUpdator_xk'
      else :
        print '* - load MeasurementUpdator'
    if self.__loadFitter :
      print '* - load track fitter of type ', self.__trackFitterType
      if self.__loadDynamicLayerCreator:
        print '*   load dynamic layer creator'
      if self.__refitROT :
        print '*   refit from ROT'
      else :
        print '*   refit from PRD, redo the ROTs'
    if self.__loadAssoTool :
      print '* - load PRD_AssociationToolGangedPixels'
    if self.__loadSummaryTool :
      print '* - load TrackSummaryTool'
      if self.__doHolesOnTrack :
        print '*   and do holes on track search'
    print '*'  
    print '************************************************************************************'
