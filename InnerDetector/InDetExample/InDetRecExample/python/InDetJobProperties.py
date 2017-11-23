# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



#
## @file InDetRecExample/python/InDetJobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" InDetJobProperties
    Python module to hold common flags to configure InDet JobOptions.

"""

__author__ = "A. Wildauer"
__version__= "$Revision: 1.104 $"
__doc__    = "InDetJobProperties"

__all__    = [ "InDetJobProperties" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

##-----------------------------------------------------------------------------
## 0th step: define infrastructure JPs DO NOT MODIFY THEM!!!

class Enabled(JobProperty):
   """ jobproperty to disable/enable algorithms in the same context
   (container) in one go
   """
   statusOn=True
   allowedTypes=['bool']
   StoredValue=True

   def set_On_NoDoAction(self, obj):
      """ Sets statusOn equals to True w/o _do_action.
      """
      if not(obj._locked):
         obj.__dict__['statusOn']=True
      else:
         obj._log.info('The JobProperty %s is blocked' % obj.__name__)

   def set_Off_NoDoAction(self, obj):
      """ Sets statusOn equals to False w/o _undo_action.
      """
      if not(obj._locked):
         obj.__dict__['statusOn']=False
      else:
         obj._log.info('The JobProperty %s is blocked' % obj.__name__)

   def _do_action(self):
      for obj in self._the_same_context_objects():
         self.set_On_NoDoAction(obj)
         #obj.set_On()

   def _undo_action(self):
      for obj in self._the_same_context_objects():
         self.set_Off_NoDoAction(obj)
         #obj.set_Off()

   def _the_same_context_objects(self):
      context = self._context_name.partition('.'+self.__name__)
      objects = list()
      
      for i in self.__class__._nInstancesContextDict.keys():
         oname = self.__class__._nInstancesContextDict[i].__name__
         ocontext = self.__class__._nInstancesContextDict[i]._context_name.partition('.'+oname)
         if context !=  ocontext:
            #print ocontext
               if context[0] == ocontext[0]:
                  objects.append(self.__class__._nInstancesContextDict[i])
      return objects

class InDetFlagsJobProperty(JobProperty):
    """ This class stores if a user changed a property once in the variable setByUser
    """
    setByUser = False

    def _do_action (self):
       self.setByUser = True

    def _undo_action (self):
       self.setByUser = True

    def get_Value(self):
       if (self.allowedTypes[0] == 'bool'):
          return self.statusOn and self.StoredValue
       else:
          return self.StoredValue

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class doDBMstandalone(InDetFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doDBM(InDetFlagsJobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doPrintConfigurables(InDetFlagsJobProperty):
    """if this is on the all the print InDetXYZ lines are activated"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doNewTracking(InDetFlagsJobProperty):
    """Turn running of newTracking on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doPseudoTracking(InDetFlagsJobProperty):
    """Turn running of the truth seeded pseudo tracking on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False 
    
class doSplitReco(InDetFlagsJobProperty):
    """Turn running of the truth seeded pseudo tracking only for pileup on and off. 
       Only makes sense to run on RDO file where SplitDigi was used!"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    	
	
class doxKalman(InDetFlagsJobProperty):
    """Turn running of xKalman on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doiPatRec(InDetFlagsJobProperty):
    """Turn running of iPatRec on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class preProcessing(InDetFlagsJobProperty):
    """Turn running of pre processing on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doPRDFormation(InDetFlagsJobProperty):
    """Turn running of PRD formation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doPixelPRDFormation(InDetFlagsJobProperty):
    """Turn running of pixel PRD formation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSCT_PRDFormation(InDetFlagsJobProperty):
    """Turn running of SCT PRD formation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doTRT_PRDFormation(InDetFlagsJobProperty):
    """Turn running of TRT PRD formation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSpacePointFormation(InDetFlagsJobProperty):
    """Turn running of space point formation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doRefit(InDetFlagsJobProperty):
    """Turn running of refitting on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSLHC(InDetFlagsJobProperty):
    """Turn running of SLHC reconstruction on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doIBL(InDetFlagsJobProperty):
    """Turn running of IBL reconstruction on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doHighPileup(InDetFlagsJobProperty):
    """Turn running of high pile-up reconstruction on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doVtxLumi(InDetFlagsJobProperty):
    """Turn running of vertex lumi reconstruction on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doVtxBeamSpot(InDetFlagsJobProperty):
   """Turn running of vertex BeamSpot reconstruction on and off"""
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = False

class doMinimalReco(InDetFlagsJobProperty):
    """Turn running of minimal reconstruction on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doDVRetracking(InDetFlagsJobProperty):
    """Turn running of large-d0 retracking mode on and off.
    This flag assumes that the processing is done from a (D)ESD file"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue   = False

class postProcessing(InDetFlagsJobProperty):
    """Turn running of post processing on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doTruth(InDetFlagsJobProperty):
    """Turn running of truth matching on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class loadTools(InDetFlagsJobProperty):
    """Turn loading of tools on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doBackTracking(InDetFlagsJobProperty):
    """Turn running of backtracking on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doLowPt(InDetFlagsJobProperty):
    """Turn running of doLowPt second pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
                  
class doVeryLowPt(InDetFlagsJobProperty):
    """Turn running of doVeryLowPt thrid pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSLHCConversionFinding(InDetFlagsJobProperty):
    """Turn running of doSLHCConversionFinding second pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doForwardTracks(InDetFlagsJobProperty):
    """Turn running of doForwardTracks pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doLowPtLargeD0(InDetFlagsJobProperty):
    """Turn running of doLargeD0 second pass down to 100 MeV on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue   = False

class doLargeD0(InDetFlagsJobProperty):
    """Turn running of doLargeD0 second pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue   = False

class useExistingTracksAsInput(InDetFlagsJobProperty):
    """Use already processed Track from a (D)ESD input file.
    This flag is related with ProcessedESDTracks InDetKey """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue   = False
                  
class cutLevel(InDetFlagsJobProperty):
    """Control cuts and settings for different lumi to limit CPU and disk space
    """
    statusOn     = True
    allowedTypes = ['int']
    allowedValues= [1,2,3,4,5,6,7,8,9,10,11,12,13,14]
    StoredValue  = 14

class doBremRecovery(InDetFlagsJobProperty):
    """Turn on running of Brem Recover in tracking"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
                  
class doCaloSeededBrem(InDetFlagsJobProperty):
    """Brem Recover in tracking restricted to Calo ROIs"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHadCaloSeededSSS(InDetFlagsJobProperty):
    """Use Recover SSS to Calo ROIs"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doBeamGas(InDetFlagsJobProperty):
    """Turn running of BeamGas second pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
                  
class doBeamHalo(InDetFlagsJobProperty):
    """Turn running of BeamHalo second pass on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doCosmics(InDetFlagsJobProperty):
    """Turn running of Cosmics on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doHeavyIon(InDetFlagsJobProperty):
    """Turn running of HeavyIons on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doParticleCreation(InDetFlagsJobProperty):
    """Turn running of particle creation on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class KeepParameters(InDetFlagsJobProperty):
    """Keep extra parameters on slimmed tracks"""
    statusOn     = True
    allowedTypes = ['bool']
    #False to drop them
    StoredValue  = True

class KeepFirstParameters(InDetFlagsJobProperty):
    """Keep the first set of track parameters in addition to the defining ones for TrackParticles."""
    statusOn     = True
    allowedTypes = ['bool']
    #False to drop them
    StoredValue  = False

class doTrackSegmentsPixel(InDetFlagsJobProperty):
    """Turn running of track segment creation in pixel on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrackSegmentsSCT(InDetFlagsJobProperty):
    """Turn running of track segment creation in SCT on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrackSegmentsTRT(InDetFlagsJobProperty):
    """Turn running of track segment creation in TRT on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringGlobal(InDetFlagsJobProperty):
    """ Use to turn on global monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringPrimaryVertexingEnhanced(InDetFlagsJobProperty):
    """ Use to turn on enhanced primary vertex monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringPixel(InDetFlagsJobProperty):
    """ Use to turn on Pixel monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringSCT(InDetFlagsJobProperty):
    """ Use to turn on SCT monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringTRT(InDetFlagsJobProperty):
    """ Use to turn on TRT monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMonitoringAlignment(InDetFlagsJobProperty):
    """ Use to turn on alignment monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class useDynamicAlignFolders(InDetFlagsJobProperty):
    """ Use to turn on dynamic alignment constants folder scheme (new development for 2016) """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doPerfMon(InDetFlagsJobProperty):
    """ Use to turn on PerfMon """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class AODall(InDetFlagsJobProperty):
    """ """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class useBeamConstraint(InDetFlagsJobProperty):
    """ use beam spot service in new tracking """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class kalmanUpdator(InDetFlagsJobProperty):
    """ control which updator to load for KalmanFitter ("None"/"fast"/"smatrix"/"weight"/"amg") """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['None','fast','weight','smatrix','amg']
    StoredValue  = 'smatrix'

class magField(InDetFlagsJobProperty):
    """ control which field tool to use ("None"/"fast") """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['None','fast']
    StoredValue  = 'None'

class propagatorType(InDetFlagsJobProperty):
    """ control which propagator to use ('RungeKutta'/'STEP') """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['RungeKutta','STEP']
    StoredValue  = 'RungeKutta'

class trackFitterType(InDetFlagsJobProperty):
    """ control which fitter to be used: 'KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['KalmanFitter', 'KalmanDNAFitter', 'DistributedKalmanFilter', 'GlobalChi2Fitter', 'GaussianSumFilter', 'ReferenceKalmanFitter']
    StoredValue  = 'GlobalChi2Fitter'

class doHolesOnTrack(InDetFlagsJobProperty):
    """ do holes search from now on in summry tool """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class useZvertexTool(InDetFlagsJobProperty):
    """ start with Zvertex finding """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSiSPSeededTrackFinder(InDetFlagsJobProperty):
    """ use track finding in silicon """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTRTExtensionNew(InDetFlagsJobProperty):
    """ turn on / off TRT extensions  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class trtExtensionType(InDetFlagsJobProperty):
    """ which extension type ("xk"/"DAF") """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['xk','DAF']
    StoredValue  = 'xk'
      
class redoTRT_LR(InDetFlagsJobProperty):
    """ use smart TRT LR/tube hit creator and redo ROTs """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    
class doTRTPhaseCalculation(InDetFlagsJobProperty):
    """ control to run TRT phase calculation """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    

class doTrtSegments(InDetFlagsJobProperty):
    """ control to run TRT Segment finding (do it always after new tracking!) """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doTRTSeededTrackFinder(InDetFlagsJobProperty):
    """ control running the back tracking """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class loadTRTSeededSPFinder(InDetFlagsJobProperty):
    """ control which SP finder is used """
    # exclusive with loadSimpleTRTSeededSPFinder
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class loadSimpleTRTSeededSPFinder(InDetFlagsJobProperty):
    """ control which SP finder is used """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
      
class doResolveBackTracks(InDetFlagsJobProperty):
    """ control running the ambi on back tracking """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
      
class doTRTStandalone(InDetFlagsJobProperty):
    """ control TRT Standalone """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
      
class refitROT(InDetFlagsJobProperty):
    """ control if refit is done from PRD or ROT """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSlimming(InDetFlagsJobProperty):
    """ turn track slimming on/off """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSlimPoolTrack(InDetFlagsJobProperty):
    """ Slimming at converter level rather than creating a slim track collections; requires slimming to be on. """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doWriteTracksToESD(InDetFlagsJobProperty):
    """ turn track slimming on/off """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doVertexFinding(InDetFlagsJobProperty):        
    """ Turn on the primary vertex reconstruction """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class primaryVertexSetup(InDetFlagsJobProperty):
    """ string to store the type of finder/fitter for pri vertexing, possible types: 'AdaptiveMultiFinding', 'IterativeFinding', 'AdaptiveFinding', 'DefaultFastFinding', 'DefaultFullFinding', 'DefaultKalmanFinding', 'DefaultAdaptiveFinding', 'DefaultVKalVrtFinding' 'MedImgMultiFinding' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= [ 'AdaptiveMultiFinding', 'IterativeFinding', 'AdaptiveFinding', 'DefaultFastFinding', 'DefaultFullFinding', 'DefaultKalmanFinding', 'DefaultAdaptiveFinding', 'DefaultVKalVrtFinding', 'DummyVxFinder', 'MedImgMultiFinding']
    StoredValue  = 'IterativeFinding'

class primaryVertexCutSetup(InDetFlagsJobProperty):
    """ string to store the type of cuts to be used in PV reconstruction: 'Offline', 'IBL', 'SLHC' 'HeavyIon' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['Offline', 'LowPt', 'IBL', 'SLHC', 'HeavyIon']
    StoredValue  = 'Offline'

class priVtxCutLevel(InDetFlagsJobProperty):
    """Control vertexing cuts and settings for different lumi to limit CPU and disk space
    """
    statusOn     = True
    allowedTypes = ['int']
    allowedValues= [1,2,3]
    StoredValue  = 3 

class vertexSeedFinder(InDetFlagsJobProperty):
    """ string to store the type of seed finder, possible types: 'SlidingWindowMultiSeedFinder', 'HistogrammingMultiSeedFinder', 'DivisiveMultiSeedFinder' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['SlidingWindowMultiSeedFinder', 'HistogrammingMultiSeedFinder', 'DivisiveMultiSeedFinder']
    StoredValue  = 'SlidingWindowMultiSeedFinder'
    
class primaryVertexSortingSetup(InDetFlagsJobProperty):
    """ string to store the type of sorting algorithm to separate signal and pile-up vertices, possible types: 'NoReSorting','SumPt2Sorting','VxProbSorting','NNSorting' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['NoReSorting','SumPt2Sorting','VxProbSorting','NNSorting']
    StoredValue  = 'SumPt2Sorting'
    
class doPrimaryVertex3DFinding(InDetFlagsJobProperty):
   """ control if to use 3d seeding for primary vertex finding (useful in case of poor / no knowledge of the beam spot """
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = True       # will be set to false automatically if beam constraint ON, otherwise true

class doVertexFindingForMonitoring(InDetFlagsJobProperty):        
    """ Turn on the primary vertex reconstruction needed to run the enhanced vertex monitoring, this runs the iterative PV with no beam constraint """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSplitVertexFindingForMonitoring(InDetFlagsJobProperty):        
    """ Turn on the primary vertex reconstruction needed to run the enhanced vertex monitoring, this runs iterative PV in split mode """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class perigeeExpression(InDetFlagsJobProperty):
    """Express track parameters wrt. to : 'BeamLine','BeamSpot','Vertex' (first primary vertex) """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['BeamLine','BeamSpot','Vertex']
    StoredValue  = 'BeamLine'   

class secondaryVertexCutSetup(InDetFlagsJobProperty):
    """ string to store the type of cuts to be used in PV reconstruction: 'StartUp', 'PileUp' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['StartUp','PileUp']
    StoredValue  = 'PileUp'

class conversionVertexCutSetup(InDetFlagsJobProperty):
    """ string to store the type of cuts to be used in conversion reconstruction: 'ConversionStartUp', 'ConversionPileUp' """
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['ConversionStartUp','ConversionPileUp']
    StoredValue  = 'ConversionPileUp'

class doSharedHits(InDetFlagsJobProperty):
    """ control if the shared hits are recorded in TrackPatricles """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doV0Finder(InDetFlagsJobProperty):  
    """ switch on/off V0 finder  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSimpleV0Finder(InDetFlagsJobProperty):  
    """ switch on/off simple V0 finder  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class useV0Fitter(InDetFlagsJobProperty):  
    """ use V0 Fitter (alternative is VKalVrt) """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSecVertexFinder(InDetFlagsJobProperty):  
    """ switch on/off conversion finder fitting V0s  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doConversions(InDetFlagsJobProperty):
    """ switch on/off conversion finder """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doStatistics(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doStandardPlots(InDetFlagsJobProperty):
    """ Use to turn on creating the Standard Plots of tracking performance """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class doPhysValMon(InDetFlagsJobProperty):
    """ Use to turn on Physics Validation Monitoring """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class materialInteractions(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class materialInteractionsType(InDetFlagsJobProperty):
    """Control which type of particle hypothesis to use for the material interactions
       0=non-interacting,1=electron,2=muon,3=pion,4=kaon,5=proton.  See ParticleHypothesis.h
       for full definition.
    """
    statusOn     = True
    allowedTypes = ['int']
    allowedValues= [0,1,2,3,4,5]
    StoredValue  = 3

class doPixelClusterNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doSctClusterNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrtDriftCircleNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrkNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doPixelTrkNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doSctTrkNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrtTrkNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doVtxNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doConvVtxNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doV0VtxNtuple(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doTrkD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doPixelTrkD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doSctTrkD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doTrtTrkD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doVtxD3PD(InDetFlagsJobProperty):
    """ fills the D3PD part of the default primary vertex finder (which is in ESD/AOD or produced during reco, normally WITH beam constraint) """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doVtxMonitoringD3PD(InDetFlagsJobProperty):
    """ fills the D3PD parts for the unconstrained PV and the split vtx, works only with iterative finder """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doConvVtxD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doV0VtxD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class doTriggerD3PD(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class removeTRTNoise(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class noTRTTiming(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class InDet25nsec(InDetFlagsJobProperty): 
    """ """ 
    statusOn     = True 
    allowedTypes = ['bool'] 
    StoredValue  = False 

class selectSCTIntimeHits(InDetFlagsJobProperty):
    """ defines if the X1X mode is used for the offline or not """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
        
class cutSCTOccupancy(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class useDCS(InDetFlagsJobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class truthMatchStrategy(InDetFlagsJobProperty):
    """defines how truth matching is done. possible values TruthMatchRatio (old style) or TruthMatchTanimoto (new style)"""
    statusOn     = True
    allowedTypes = ['str']
    allowedValues= ['TruthMatchRatio','TruthMatchTanimoto']
    StoredValue  = 'TruthMatchRatio'

class doFatras(InDetFlagsJobProperty):
    """ Switch for FATRAS running """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doSGDeletion(InDetFlagsJobProperty):
    """ Drop contianers from SG ones finished with them """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doLowBetaFinder(InDetFlagsJobProperty):
    """ Switch for running Low-Beta finer """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class useHVForSctDCS(InDetFlagsJobProperty):
    """ Temporary switch for using 20V HV limit for SCT DCS """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class disableTracking(InDetFlagsJobProperty):
    """ Disable all tracking algorithms """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class disableInDetReco(InDetFlagsJobProperty):
    """
    Disable all ID reconstruction: pre-processing,tracking, post-processing etc.
    Still does the configuration: job porperties, cuts, loaign of tools and conditions
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doPixelClusterSplitting (InDetFlagsJobProperty):
   """ Try to split pixel clusters """
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = True
   
class pixelClusterSplittingType(InDetFlagsJobProperty):
   """ choose splitter type: NeuralNet or AnalogClus"""
   statusOn     = True
   allowedTypes = ['str']
   allowedValues= ['NeuralNet','AnalogClus']
   StoredValue  = 'NeuralNet'

class pixelClusterSplitProb1 (InDetFlagsJobProperty):
   """ Cut value for splitting clusters into two parts """
   statusOn = True
   allowedTypes = ['float']
   StoredValue = 0.6

class pixelClusterSplitProb2 (InDetFlagsJobProperty):
   """ Cut value for splitting clusters into three parts """
   statusOn = True
   allowedTypes = ['float']
   StoredValue = 0.2

class pixelClusterSplitProb1_run1 (InDetFlagsJobProperty):
   """ Cut value for splitting clusters into two parts """
   statusOn = True
   allowedTypes = ['float']
   StoredValue = 0.5

class pixelClusterSplitProb2_run1 (InDetFlagsJobProperty):
   """ Cut value for splitting clusters into three parts """
   statusOn = True
   allowedTypes = ['float']
   StoredValue = 0.5

class pixelClusterSplitMinPt (InDetFlagsJobProperty):
   """ Min pt for tracks to try and split hits """
   statusOn = True
   allowedTypes = ['float']
   StoredValue = 1000

class pixelClusterBadClusterID (InDetFlagsJobProperty) :
   """ Select the mode to identify suspicous pixel clusteri """
   statusOn = True
   allowedTypes = ['int']
   allowedValues = [0,1,2,3,4]
   StoredValue = 3

class useBroadClusterErrors(InDetFlagsJobProperty):
    """ Use broad cluster errors for Pixel/SCT """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class useBroadPixClusterErrors(InDetFlagsJobProperty):
    """ Use broad cluster errors for Pixel"""
    statusOn     = True
    allowedTypes = ['NoneType','bool']
    StoredValue  = None

class useBroadSCTClusterErrors(InDetFlagsJobProperty):
    """ Use broad cluster errors for SCT"""
    statusOn     = True
    allowedTypes = ['NoneType','bool']
    StoredValue  = None

class writeRDOs(InDetFlagsJobProperty):
    """ Write RDOs into ESD """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class writePRDs(InDetFlagsJobProperty):
    """ Write PRDs into ESD """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doMinBias(InDetFlagsJobProperty):
    """ Switch for running MinBias settings """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doLowMuRunSetup(InDetFlagsJobProperty):
    """ Switch for running MinBias runs at low lumi settings """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doRobustReco(InDetFlagsJobProperty):
    """ Switch for running Robust settings """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class doSingleCollisionVertexReco(InDetFlagsJobProperty):
    """ Switch for running veretx reconstruction in single collision mode """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class useMBTSTimeDiff(InDetFlagsJobProperty): 
    """ Switch for skipping background events based on MBTS time info """ 
    statusOn     = True  
    allowedTypes = ['bool'] 
    StoredValue  = False 

class useNewSiSPSeededTF(InDetFlagsJobProperty): 
    """ Switch for using new SiSPSeededTrackFinder strategy""" 
    statusOn     = True  
    allowedTypes = ['bool'] 
    StoredValue  = False 

class doxAOD(InDetFlagsJobProperty):
    """ Switch for running AOD to xAOD conversion algs """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doCaloSeededTRTSegments(InDetFlagsJobProperty):
    """ Switch for running AOD to xAOD conversion algs """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doInnerDetectorCommissioning(InDetFlagsJobProperty):
  """ Switch for running looser settings in ID for commissioning """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doTIDE_Ambi(InDetFlagsJobProperty):
  """ Switch for running TIDE Ambi """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True
  
class doTIDE_RescalePixelCovariances(InDetFlagsJobProperty):
  """ Switch for running TIDE pixel cluster covariance rescaling """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doRefitInvalidCov(InDetFlagsJobProperty):
  """ Try Kalman fitter if the track fit in the ambiguity processor produces non positive definitematrices."""
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doRejectInvalidCov(InDetFlagsJobProperty):
  """ Reject all tracks which have a non positive definite covariance matrix after the refit."""
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doSSSfilter(InDetFlagsJobProperty):
  """ Switch for running SSS filter"""
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = True

class pT_SSScut(InDetFlagsJobProperty):
  """ Pt cut for SSS filter [GeV]"""
  statusOn     = True
  allowedTypes = ['float']
  StoredValue  = -1
  
class doTIDE_AmbiTrackMonitoring(InDetFlagsJobProperty):
  """ Switch for monitoring track canidates in TIDE Ambi process """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class ForceCoraCool(InDetFlagsJobProperty):
  """ Use old (non CoolVectorPayload) SCT Conditions """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class ForceCoolVectorPayload(InDetFlagsJobProperty):
  """ Use new (CoolVectorPayload) SCT Conditions """
  statusOn     = True
  allowedTypes = ['bool']
  StoredValue  = False

class doTrackSegmentsPixelPrdAssociation(InDetFlagsJobProperty):
    """Turn running of track segment creation in pixel after NewTracking, and with PRD association, on and off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doSLHCVeryForward(InDetFlagsJobProperty): 
  """Turn running of SLHC reconstruction for Very Forward extension on and off""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False 

class doTRTGlobalOccupancy(InDetFlagsJobProperty): 
  """Turn running of Event Info TRT Occupancy Filling Alg on and off (also whether it is used in TRT PID calculation)""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class doNNToTCalibration(InDetFlagsJobProperty): 
  """USe ToT calibration for NN clustering rather than Charge""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class keepAdditionalHitsOnTrackParticle(InDetFlagsJobProperty): 
  """Do not drop first/last hits on track (only for special cases - will blow up TrackParticle szie!!!)""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class doSCTModuleVeto(InDetFlagsJobProperty): 
  """Turn on SCT_ModuleVetoSvc, allowing it to be configured later""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class doParticleConversion(InDetFlagsJobProperty): 
  """In case anyone still wants to do Rec->xAOD TrackParticle Conversion""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class doHIP300(InDetFlagsJobProperty):
   """ Switch for running MinBias settings with a 300 MeV pT cut (for Heavy Ion Proton)"""
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = False

class doStoreTrackSeeds(InDetFlagsJobProperty): 
  """Turn on to save the Track Seeds in a xAOD track collecting for development studies""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = False

class checkDeadElementsOnTrack(InDetFlagsJobProperty): 
  """Enable check for dead modules and FEs""" 
  statusOn     = True 
  allowedTypes = ['bool']
  StoredValue  = True


##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class InDetJobProperties(JobPropertyContainer):
  """Container for the InDet flags
  """

  def __init__(self, context=''):
    #allow specifying context (used by the trigger) 
    JobPropertyContainer.__init__(self,context)
  
  def checkThenSet (self, jp, value):
    # checks if a variable has been changed by the user before
    if not jp.setByUser:
      jp.set_Value (value)

  def setupDefaults(self):
    #do not add anything to this method if you are not 100% sure what you are doing!
    #if in doubt contact release coordination
    #-------------------------------------------------------------------
    #disable things depending on global beam setup
    #-------------------------------------------------------------------
    from RecExConfig.RecFlags import rec
    if rec.doMonitoring():
      self.checkThenSet(self.doMonitoringGlobal   , True)
      self.checkThenSet(self.doMonitoringPrimaryVertexingEnhanced   , True)
      self.checkThenSet(self.doMonitoringPixel    , True)
      self.checkThenSet(self.doMonitoringSCT      , True)
      self.checkThenSet(self.doMonitoringTRT      , True)
      self.checkThenSet(self.doMonitoringAlignment, True)

    from AthenaCommon.DetFlags    import DetFlags

    from AthenaCommon.BeamFlags import jobproperties
    print "InDetJobProperties::setupDefaults():  jobproperties.Beam.beamType() is "+jobproperties.Beam.beamType()+" bunch spacing is "+str(jobproperties.Beam.bunchSpacing()) 

    if ( jobproperties.Beam.beamType()=="collisions" and jobproperties.Beam.bunchSpacing() <= 25): 
       self.checkThenSet(self.InDet25nsec            , True)     

    if self.doHIP300 :
       self.checkThenSet(self.doRejectInvalidCov      ,True)

    if self.doSLHCVeryForward():
       self.checkThenSet(self.doSLHC            , True)
       self.checkThenSet(self.doForwardTracks   , True)

    if (jobproperties.Beam.beamType()=="singlebeam"):
       self.checkThenSet(self.useHVForSctDCS         , True)    
       self.checkThenSet(self.doNewTracking          , False)
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBeamGas              , True )
       self.checkThenSet(self.doBeamHalo             , True )
       self.checkThenSet(self.doParticleCreation     , True )
       self.checkThenSet(self.doTRTPhaseCalculation  , True )
       self.checkThenSet(self.doTRTStandalone        , True)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doVertexFinding        , False)
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)
       self.checkThenSet(self.doSecVertexFinder      , False)
       self.checkThenSet(self.doConversions          , False)
       self.checkThenSet(self.noTRTTiming            , True )
       self.checkThenSet(self.materialInteractions   , False)
       self.checkThenSet(self.doSlimming             , False)
       self.checkThenSet(self.doLowBetaFinder        , False)

    elif (jobproperties.Beam.beamType()=="cosmics"):
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doTRTStandalone        , True ) # *ME* this is different than 15.4.0 and before
       self.checkThenSet(self.doTRTPhaseCalculation  , True )
       self.checkThenSet(self.doParticleCreation     , True )
       self.checkThenSet(self.doVertexFinding        , False)
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)
       self.checkThenSet(self.doSecVertexFinder      , False )
       self.checkThenSet(self.doConversions          , False )
       #self.checkThenSet(self.doTrackSegmentsPixel   , False)
       #self.checkThenSet(self.doTrackSegmentsSCT     , False)
       #self.checkThenSet(self.doTrackSegmentsTRT     , False)
       self.checkThenSet(self.doLowBetaFinder        , False)
       self.checkThenSet(self.doCosmics              , True )
       self.checkThenSet(self.preProcessing          , True )
       self.checkThenSet(self.doSlimming             , False)
       self.checkThenSet(self.materialInteractionsType   , 2)      
       self.checkThenSet(self.selectSCTIntimeHits    , False)
       self.checkThenSet(self.cutLevel               , 8    )
       self.checkThenSet(self.doInnerDetectorCommissioning, True)

    # --- special case heavy ion
    elif (rec.doHeavyIon()):
       self.checkThenSet(self.doHeavyIon             , True )
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.useZvertexTool         , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doTRTStandalone        , False)
       self.checkThenSet(self.doTrackSegmentsPixel   , False)
       self.checkThenSet(self.doTrackSegmentsSCT     , False)
       self.checkThenSet(self.doTrackSegmentsTRT     , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doVertexFinding        , True)
       self.checkThenSet(self.primaryVertexSetup     , "DefaultFastFinding")
       self.checkThenSet(self.primaryVertexCutSetup  , "HeavyIon") 
       self.checkThenSet(self.doSingleCollisionVertexReco, True )
       self.checkThenSet(self.useBeamConstraint      , True )
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)
       self.checkThenSet(self.doConversions          , False)
       self.checkThenSet(self.doStatistics           , False)
       self.checkThenSet(self.doSlimming             , True )
       self.checkThenSet(self.writeRDOs              , False)
       self.checkThenSet(self.useMBTSTimeDiff        , True )
       self.checkThenSet(self.cutLevel               , 2    )
       self.checkThenSet(self.priVtxCutLevel         , 1    )
       self.checkThenSet(self.doTrackSegmentsPixelPrdAssociation, False)
       self.checkThenSet(self.perigeeExpression      , 'Vertex')
       self.checkThenSet(self.doRefitInvalidCov      ,True)

    # --- special case SLHC
    elif (self.doSLHC()):
       print "----> InDetJobProperties for SLHC"
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, True )
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doTRTStandalone        , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doVertexFinding        , True)
       self.checkThenSet(self.primaryVertexSetup     , "IterativeFinding")
       self.checkThenSet(self.primaryVertexCutSetup  , "SLHC")
       self.checkThenSet(self.secondaryVertexCutSetup, "PileUp") 
       self.checkThenSet(self.vertexSeedFinder       , "SlidingWindowMultiSeedFinder")
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)
       self.checkThenSet(self.doConversions          , False)
       self.checkThenSet(self.doStatistics           , False)
       self.checkThenSet(self.doTrackSegmentsPixel   , False)
       self.checkThenSet(self.doTrackSegmentsSCT     , False)
       self.checkThenSet(self.doTrackSegmentsTRT     , False)
       self.checkThenSet(self.doSlimming             , False)
       self.checkThenSet(self.doSGDeletion           , True )
       self.checkThenSet(self.doTIDE_RescalePixelCovariances, False)
       # TEMPORARY FIX TO STOP SEG FAULT
       self.checkThenSet(self.doPixelClusterSplitting, False)
       self.checkThenSet(self.doTIDE_Ambi, False)
       self.checkThenSet(self.doTrackSegmentsPixelPrdAssociation, False)

    elif (self.doIBL()):
       print "----> InDetJobProperties for IBL"
       print "----> DEPRECATED! This should now be the default settings"
       #self.checkThenSet(self.doNewTracking          , True )
       #self.checkThenSet(self.doLowPt                , False)
       #self.checkThenSet(self.doVeryLowPt            , False)
       #self.checkThenSet(self.doBeamGas              , False)
       #self.checkThenSet(self.doBeamHalo             , False)
       #self.checkThenSet(self.doxKalman              , False)
       #self.checkThenSet(self.doiPatRec              , False)
       #self.checkThenSet(self.doBackTracking         , False)
       #self.checkThenSet(self.doTRTStandalone        , False)
       #self.checkThenSet(self.doForwardTracks        , False)
       #self.checkThenSet(self.doVertexFinding        , True)
       #self.checkThenSet(self.primaryVertexSetup     , "IterativeFinding") 
       #self.checkThenSet(self.primaryVertexCutSetup  , "IBL") 
       #self.checkThenSet(self.secondaryVertexCutSetup, "PileUp") 
       #self.checkThenSet(self.vertexSeedFinder       , "SlidingWindowMultiSeedFinder")
       #self.checkThenSet(self.doV0Finder             , False)
       #self.checkThenSet(self.doSimpleV0Finder       , False)      
       #self.checkThenSet(self.doConversions          , False )        
       #self.checkThenSet(self.doStatistics           , False) 
       #self.checkThenSet(self.doTrackSegmentsPixel   , False )
       #self.checkThenSet(self.doTrackSegmentsSCT     , False )
       #self.checkThenSet(self.doTrackSegmentsTRT     , False )
       #self.checkThenSet(self.doSlimming             , False )
       #self.checkThenSet(self.doSGDeletion           , True )
 
    elif (self.doHighPileup()):
       print "----> InDetJobProperties for high pilep"
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doTRTStandalone        , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doVertexFinding        , True)
       self.checkThenSet(self.primaryVertexSetup     , "IterativeFinding") 
       self.checkThenSet(self.primaryVertexCutSetup  , "Offline") 
       self.checkThenSet(self.secondaryVertexCutSetup, "PileUp") 
       self.checkThenSet(self.vertexSeedFinder       , "SlidingWindowMultiSeedFinder")
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)      
       self.checkThenSet(self.doConversions          , True )        
       self.checkThenSet(self.doStatistics           , False) 
       self.checkThenSet(self.doTrackSegmentsPixel   , False )
       self.checkThenSet(self.doTrackSegmentsSCT     , False )
       self.checkThenSet(self.doTrackSegmentsTRT     , False )
       self.checkThenSet(self.doSlimming             , False )
       self.checkThenSet(self.doSGDeletion           , True )

    # --- special setup for vtxlumi stream processing   
    elif (self.doVtxLumi()):
       print "----> InDetJobProperties for vertex lumi"
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doTRTStandalone        , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doVertexFinding        , True)
       self.checkThenSet(self.primaryVertexSetup     , "IterativeFinding") 
       self.checkThenSet(self.primaryVertexCutSetup  , "Offline") 
       self.checkThenSet(self.secondaryVertexCutSetup, "PileUp") 
       self.checkThenSet(self.vertexSeedFinder       , "SlidingWindowMultiSeedFinder")
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)      
       self.checkThenSet(self.doConversions          , False )        
       self.checkThenSet(self.doStatistics           , False) 
       self.checkThenSet(self.doTrackSegmentsPixel   , False )
       self.checkThenSet(self.doTrackSegmentsSCT     , False )
       self.checkThenSet(self.doTrackSegmentsTRT     , False )
       self.checkThenSet(self.doSlimming             , False )
       self.checkThenSet(self.useBeamConstraint      , False )
       self.checkThenSet(self.selectSCTIntimeHits    , True )
       # --- turn off brem
       self.checkThenSet(self.doBremRecovery         , False)
       self.checkThenSet(self.doCaloSeededBrem       , False)
       self.checkThenSet(self.doHadCaloSeededSSS     , False)
       # --- turn off TRT
       DetFlags.makeRIO.TRT_setOff()
       DetFlags.TRT_setOff()
       DetFlags.detdescr.TRT_setOn()
       DetFlags.dcs.TRT_setOff()

# --- special setup for vtxbeamspot stream processing   
    elif (self.doVtxBeamSpot()):
       print "----> InDetJobProperties for vertex lumi"
       self.checkThenSet(self.doNewTracking          , True )
       self.checkThenSet(self.doLowPt                , False)
       self.checkThenSet(self.doVeryLowPt            , False)
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doBeamGas              , False)
       self.checkThenSet(self.doBeamHalo             , False)
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       self.checkThenSet(self.doBackTracking         , False)
       self.checkThenSet(self.doTRTStandalone        , False)
       self.checkThenSet(self.doForwardTracks        , False)
       self.checkThenSet(self.doVertexFinding        , True)
       self.checkThenSet(self.primaryVertexSetup     , "IterativeFinding")
       self.checkThenSet(self.primaryVertexCutSetup  , "Offline")
       self.checkThenSet(self.secondaryVertexCutSetup, "PileUp")
       self.checkThenSet(self.vertexSeedFinder       , "SlidingWindowMultiSeedFinder")
       self.checkThenSet(self.doV0Finder             , False)
       self.checkThenSet(self.doSimpleV0Finder       , False)     
       self.checkThenSet(self.doConversions          , False )       
       self.checkThenSet(self.doStatistics           , False)
       self.checkThenSet(self.doTrackSegmentsPixel   , False )
       self.checkThenSet(self.doTrackSegmentsSCT     , False )
       self.checkThenSet(self.doTrackSegmentsTRT     , False )
       self.checkThenSet(self.doSlimming             , False )
       self.checkThenSet(self.useBeamConstraint      , False )
       self.checkThenSet(self.selectSCTIntimeHits    , False )
       # --- turn off brem
       self.checkThenSet(self.doBremRecovery         , False)
       self.checkThenSet(self.doCaloSeededBrem       , False)
       self.checkThenSet(self.doHadCaloSeededSSS     , False)
       # --- turn off TRT
       DetFlags.TRT_setOff()

    # --- special case minimal reconstruction setup
    elif (self.doMinimalReco()):
       print "----> InDetJobProperties for minimal reconstruction"
       self.checkThenSet(self.preProcessing          , True )
       self.checkThenSet(self.doPRDFormation         , True )
       self.checkThenSet(self.doSpacePointFormation  , True )
       self.checkThenSet(self.doNewTracking          , False )
       self.checkThenSet(self.doLowPt                , False )
       self.checkThenSet(self.doVeryLowPt            , False )
       self.checkThenSet(self.doSLHCConversionFinding, False)
       self.checkThenSet(self.doForwardTracks        , False )
       self.checkThenSet(self.doBeamGas              , False )
       self.checkThenSet(self.doBeamHalo             , False )
       self.checkThenSet(self.doxKalman              , False )
       self.checkThenSet(self.doiPatRec              , False )
       self.checkThenSet(self.doBackTracking         , False )
       self.checkThenSet(self.doTRTStandalone        , False )
       self.checkThenSet(self.postProcessing         , False )
       self.checkThenSet(self.doVertexFinding        , False )
       self.checkThenSet(self.doV0Finder             , False )
       self.checkThenSet(self.doSimpleV0Finder       , False )
       self.checkThenSet(self.doConversions          , False )
       self.checkThenSet(self.doParticleCreation     , False )
       self.checkThenSet(self.doStatistics           , False )
       self.checkThenSet(self.doTrackSegmentsPixel   , False )
       self.checkThenSet(self.doTrackSegmentsSCT     , False )
       self.checkThenSet(self.doTrackSegmentsTRT     , False )
       self.checkThenSet(self.doSlimming             , False )
       self.checkThenSet(self.doSGDeletion           , True  )
       self.checkThenSet(self.doTrkNtuple            , False )
       self.checkThenSet(self.doMonitoringGlobal     , False )
       self.checkThenSet(self.doMonitoringPrimaryVertexingEnhanced     , False )
       self.checkThenSet(self.doMonitoringPixel      , False )
       self.checkThenSet(self.doMonitoringSCT        , False )
       self.checkThenSet(self.doMonitoringTRT        , False )
       self.checkThenSet(self.doMonitoringAlignment  , False )
       self.checkThenSet(self.doBremRecovery         , False)
       self.checkThenSet(self.doCaloSeededBrem       , False)
       self.checkThenSet(self.doHadCaloSeededSSS     , False)
    # --- new setup for LargeD0 retracking -- what the user
    # --- is allowed to change
    elif self.doDVRetracking():
       print "----> InDetJobProperties for high-d0 tracks reconstruction"
       # see setDVRetracking method which overrides some of the flags
       self.checkThenSet(self.doLargeD0               , True              )
       self.checkThenSet(self.useExistingTracksAsInput, True              )
       self.checkThenSet(self.preProcessing           , True              )
       self.checkThenSet(self.doSpacePointFormation   , True              )
       self.checkThenSet(self.postProcessing          , False             )
       # --- enable prim/sec vertexing ?? Probably yes, but a DV-vertexing
       # --- instead primary vertex setup 
       self.checkThenSet(self.doVertexFinding         , False             )
       self.checkThenSet(self.primaryVertexSetup      , "IterativeFinding")
       self.checkThenSet(self.primaryVertexCutSetup   , "Offline"         )          
       self.checkThenSet(self.priVtxCutLevel          , 3                 )   
       # --- sec vertexing setup
       self.checkThenSet(self.secondaryVertexCutSetup , "PileUp"          ) 
       self.checkThenSet(self.conversionVertexCutSetup, "ConversionPileUp")
       self.checkThenSet(self.doV0Finder              , False             )
       self.checkThenSet(self.doSimpleV0Finder        , False             )
       self.checkThenSet(self.doConversions           , False             )
       self.checkThenSet(self.doParticleCreation      , False             )
       self.checkThenSet(self.doStatistics            , False             )
       self.checkThenSet(self.doTrackSegmentsPixel    , False             )
       self.checkThenSet(self.doTrackSegmentsSCT      , False             )
       self.checkThenSet(self.doTrackSegmentsTRT      , False             )
       self.checkThenSet(self.doSlimming              , False             )
       self.checkThenSet(self.doSGDeletion            , False             )
       self.checkThenSet(self.doTrkNtuple             , False             )
       self.checkThenSet(self.doMonitoringGlobal      , False             )
       self.checkThenSet(self.doMonitoringPrimaryVertexingEnhanced, False )
       self.checkThenSet(self.doMonitoringPixel       , False             )
       self.checkThenSet(self.doMonitoringSCT         , False             )
       self.checkThenSet(self.doMonitoringTRT         , False             )
       self.checkThenSet(self.doMonitoringAlignment   , False             )
       self.checkThenSet(self.doBremRecovery          , True              )
       self.checkThenSet(self.doCaloSeededBrem        , True              )
       self.checkThenSet(self.doHadCaloSeededSSS      , False             )
       self.checkThenSet(self.doTrackSegmentsPixelPrdAssociation, False   )
       # --- Output
       self.checkThenSet(self.AODall                  , False             )
       self.checkThenSet(self.doxAOD                  , True              )


    # --- collisions, this is the main one !
    elif (jobproperties.Beam.beamType()=="collisions"):
       # --- set default track configuration
       self.checkThenSet(self.doxKalman              , False)
       self.checkThenSet(self.doiPatRec              , False)
       # --- enable phase for collisions by default
       self.checkThenSet(self.doTRTPhaseCalculation  , True )       

       # --- new setup for MinBias tracking
       if self.doMinBias():
          # --- run TRT only tracking over the entire detector
          self.checkThenSet(self.cutLevel     , 12)
          # --- disable forward tracklets
          self.checkThenSet(self.doForwardTracks     , False )
          # --- run tracklets
          self.checkThenSet(self.doTrackSegmentsPixelPrdAssociation, False)
          self.checkThenSet(self.doTrackSegmentsPixel, True )
          #self.checkThenSet(self.doTrackSegmentsSCT  , False )
          #self.checkThenSet(self.doTrackSegmentsTRT  , False )
          # --- turn off brem
          #self.checkThenSet(self.doBremRecovery  , False)
          #self.checkThenSet(self.doCaloSeededBrem, False)
          self.checkThenSet(self.doHadCaloSeededSSS     , False)

          # --- primary vertex setup
          self.checkThenSet(self.primaryVertexSetup      , "IterativeFinding")
          self.checkThenSet(self.primaryVertexCutSetup   , "Offline")          
          self.checkThenSet(self.priVtxCutLevel          , 1)   
          # --- sec vertexing setup
          self.checkThenSet(self.secondaryVertexCutSetup , "StartUp") 
          self.checkThenSet(self.conversionVertexCutSetup, "ConversionStartUp")
          # --- enable sec vertexing
          self.checkThenSet(self.doV0Finder,       True) 
          self.checkThenSet(self.doSimpleV0Finder, True)
          self.checkThenSet(self.doConversions,    True)
          # --- keep 1st and last hit information
          self.checkThenSet(self.keepAdditionalHitsOnTrackParticle, True)
         

       # --- new setup for d0LowLMuRunSetup tracking
       elif self.doLowMuRunSetup():
          # --- run soft tracking, but not low-pt
          self.checkThenSet(self.doLowPt             , True )    
          self.checkThenSet(self.doVeryLowPt         , False )
          # --- disable forward tracklets
          self.checkThenSet(self.doForwardTracks     , False )
          # --- run tracklets
          self.checkThenSet(self.doTrackSegmentsPixel, True )
          self.checkThenSet(self.doTrackSegmentsSCT  , True )
          self.checkThenSet(self.doTrackSegmentsTRT  , True )
          # --- turn off brem
          self.checkThenSet(self.doBremRecovery  , False)
          self.checkThenSet(self.doCaloSeededBrem, False)
          self.checkThenSet(self.doHadCaloSeededSSS     , False)

          # --- primary vertex setup
          self.checkThenSet(self.primaryVertexSetup   , "IterativeFinding")
          self.checkThenSet(self.primaryVertexCutSetup, "LowPt")
          # --- sec vertexing setup
          self.checkThenSet(self.secondaryVertexCutSetup , "StartUp") 
          self.checkThenSet(self.conversionVertexCutSetup, "ConversionStartUp") 
          # --- set cut levels back to old values
          self.checkThenSet(self.cutLevel               , 3    )
          self.checkThenSet(self.priVtxCutLevel         , 1    )
       else:
          # --- vertexing setup is the pileup one now (if not set already above)
          self.checkThenSet(self.primaryVertexSetup   , "IterativeFinding")
          self.checkThenSet(self.primaryVertexCutSetup, "Offline")

    # --- turn off brem reco for non collision reconstruction
    if not jobproperties.Beam.beamType()=="collisions":
       self.checkThenSet(self.doBremRecovery  , False)
       self.checkThenSet(self.doCaloSeededBrem, False)
       self.checkThenSet(self.doHadCaloSeededSSS     , False)

    # --- comissioning steering independent of cosmics/collisions/...
    #if (rec.Commissioning()):
    #    self.checkThenSet(self.doInnerDetectorCommissioning, True)

    if (self.doInnerDetectorCommissioning()):
        if not (self.doCosmics()):
           self.checkThenSet(self.useBroadClusterErrors  , True)
        self.checkThenSet(self.doSlimming             , False)
        self.checkThenSet(self.doTrackSegmentsPixel, True )
        self.checkThenSet(self.doTrackSegmentsSCT  , True )
        self.checkThenSet(self.doTrackSegmentsTRT  , True )
        self.checkThenSet(self.doPixelClusterSplitting, False)
        self.checkThenSet(self.doTIDE_Ambi, False)
        self.checkThenSet(self.doTIDE_RescalePixelCovariances, False)
        self.checkThenSet(self.doTrackSegmentsPixelPrdAssociation, False)

    if rec.doExpressProcessing() :
       self.checkThenSet(self.useBeamConstraint,False)

  def init(self):
    #Method to do the final setup of the flags according to user input before.
    #This method MUST ONLY BE CALLED once in InDetRecExample/InDetRec_jobOptions.py!!
    if not self.Enabled:
      print 'InDetFlags.init(): ID flags are disabled. Locking container and not doing anything else.'
    else:
      print 'Initializing InDetJobProperties with DetFlags, GlobalFlags and other high level flags.'

      # THIS METHOD MUST BE THE FIRST TO BE CALLED. DO NOT MOVE IT OR ADD THINGS IN FRONT
      self.setupDefaults()

      from AthenaCommon.GlobalFlags import globalflags
      from AthenaCommon.DetFlags    import DetFlags
      from RecExConfig.RecFlags     import rec

      # -------------------------------------------------------------------
      # Cosmics TRT extension, NewTracking needs full tracking geometry 
      # -------------------------------------------------------------------
      if self.doCosmics() and DetFlags.haveRIO.TRT_on():
         from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
         TrkDetFlags.TRT_BuildStrawLayers.set_Value_and_Lock(True)

      # -------------------------------------------------------------------
      # ESD/AOD output options
      # -------------------------------------------------------------------
      if self.AODall() and jobproperties.Beam.beamType()=="collisions":
        self.doiPatRec     = True
        self.doNewTracking = True

      # --------------------------------------------------------------------
      # ---- Disable tracking/reco
      # --------------------------------------------------------------------      
      if self.disableInDetReco(): self.setInDetRecoOff()
      if self.disableTracking():  self.setTrackingOff()

      # --------------------------------------------------------------------
      # ---- Robust reco
      # --------------------------------------------------------------------      
      if self.doRobustReco(): self.setRobustReco()

      # --------------------------------------------------------------------
      # ---- Large-d0 re-tracking setup
      # --------------------------------------------------------------------      
      # no Large radius tracking if pixel or sct off (new tracking = inside out only)
      self.doLargeD0 = self.doLargeD0() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
      self.doLowPtLargeD0 = self.doLowPtLargeD0() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
      if self.doDVRetracking():
          self.setDVRetracking()
      
      # --------------------------------------------------------------------
      # ---- PreProcessing algorithms
      # --------------------------------------------------------------------
      self.doSpacePointFormation = self.preProcessing() and self.doSpacePointFormation() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
      self.doPRDFormation        = self.preProcessing() and self.doPRDFormation()        and (DetFlags.makeRIO.pixel_on() or DetFlags.makeRIO.SCT_on() or DetFlags.makeRIO.TRT_on())
      
      # --------------------------------------------------------------------
      # ---- TIDE Pixel cluster covariance rescaling
      # --------------------------------------------------------------------
      self.doTIDE_RescalePixelCovariances = self.doTIDE_RescalePixelCovariances() and self.doPixelClusterSplitting() and self.pixelClusterSplittingType() == 'NeuralNet' and self.doTIDE_Ambi()

      # --------------------------------------------------------------------
      # --- 1st iteration, inside out tracking
      # --------------------------------------------------------------------
      #
      # no new tracking if pixel or sct off (new tracking = inside out only)
      self.doNewTracking = self.doNewTracking() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
	  # always use truth tracking for SplitReco and never use it if pixel or sct off
      self.doPseudoTracking = (self.doPseudoTracking() or self.doSplitReco()) and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
      #
      # no low pt tracking if no new tracking before or if pixels are off (since low-pt tracking is pixel seeded)!  Explicitly veto for cosmics to aid T0
      self.doLowPt       = self.doLowPt() and self.doNewTracking() and ( DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on() ) and not self.doCosmics()
      # no low pt tracking if no new tracking before or if pixels are off (since low-pt tracking is pixel seeded)!      
      self.doVeryLowPt   = self.doVeryLowPt() and self.doLowPt()
      #
      self.doSLHCConversionFinding = self.doSLHCConversionFinding() and self.doSLHC() and self.doNewTracking() and ( DetFlags.haveRIO.pixel_on() and DetFlags.haveRIO.SCT_on() ) and not self.doCosmics()
      # new forward tracklets
      self.doForwardTracks = self.doForwardTracks() and self.doNewTracking()
      #
      # no Large radius tracking if pixel or sct off (new tracking = inside out only)
      self.doLargeD0 = self.doLargeD0() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
      # no BeamGas tracking if no new tracking before (but only if beamtype is not single beam!)      
      if (jobproperties.Beam.beamType()!="singlebeam"):
        self.doBeamGas     = self.doBeamGas() and self.doNewTracking()

      #
      # --------------------------------------------------------------------
      # --- 2nd iteration, outside in tracking
      # --------------------------------------------------------------------
      #
      # control whether to run SiSPSeededTrackFinder
      self.doSiSPSeededTrackFinder = (self.doNewTracking() or self.doNewTrackingSegments() or \
                                      self.doBeamGas() or self.doLargeD0() or self.doLowPtLargeD0() ) \
                                    and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())      
      # failsafe lines in case requirements are not met to run TRT standalone or back tracking
      self.doTRTStandalone         = self.doTRTStandalone() and DetFlags.haveRIO.TRT_on()
      self.doBackTracking          = self.doBackTracking() and DetFlags.haveRIO.TRT_on() and DetFlags.haveRIO.SCT_on()
      #      
      # control to run steps which are needed for outside in tracking
      self.doTrtSegments           = (self.doTRTStandalone() or self.doBackTracking())
      self.doTRTSeededTrackFinder  = self.doBackTracking()
      self.doResolveBackTracks     = self.doBackTracking()
      #
      # --------------------------------------------------------------------
      # ---- segment finding
      # --------------------------------------------------------------------
      #
      # to be able to turn on the standalone trackings (if detector is on)
      self.doTrackSegmentsPixel  = self.doTrackSegmentsPixel() and DetFlags.haveRIO.pixel_on()
      self.doTrackSegmentsSCT    = self.doTrackSegmentsSCT()   and DetFlags.haveRIO.SCT_on()
      self.doTrackSegmentsTRT    = self.doTrackSegmentsTRT()   and DetFlags.haveRIO.TRT_on()
      
      #
      # --------------------------------------------------------------------
      # ---- Monitoring
      # --------------------------------------------------------------------
      #
      # Turn off the subdetectror monitoring if the detector is off
      self.doMonitoringPixel  = self.doMonitoringPixel() and DetFlags.haveRIO.pixel_on()
      self.doMonitoringSCT    = self.doMonitoringSCT()   and DetFlags.haveRIO.SCT_on()
      self.doMonitoringTRT    = self.doMonitoringTRT()   and DetFlags.haveRIO.TRT_on()

      # 
      # --------------------------------------------------------------------
      # ---- PostProcessing algorithms
      # --------------------------------------------------------------------
      #
      # control if run vertexing
      self.doVertexFinding          = self.postProcessing() and self.doVertexFinding()
      self.doPrimaryVertex3DFinding = self.doPrimaryVertex3DFinding() and (not self.useBeamConstraint())
      #
      # control whether to create TrackParticles (default=true)
      self.doParticleCreation       = self.postProcessing() and self.doParticleCreation()
      #
      # control to run InDetV0Finder
      self.doV0Finder               = self.postProcessing() and self.doV0Finder()
      #
      # control to run InDetSimpleV0Finder
      #self.doSimpleV0Finder         = self.postProcessing() and self.doSimpleV0Finder()
      #
      # control to run alternative InDetV0Finder
      self.doSecVertexFinder        = self.postProcessing() and self.doSecVertexFinder()
      #
      # control to run InDetConversionFinder
      self.doConversions            = self.postProcessing() and self.doConversions()
      # control to run LowBetaFinder
      self.doLowBetaFinder          = self.doLowBetaFinder() and DetFlags.haveRIO.TRT_on()

      #
      # --------------------------------------------------------------------
      # ---- Statistics
      # --------------------------------------------------------------------
      #
      # control whether to do statistics package
      self.doStatistics            = self.postProcessing() and self.doStatistics()
      # control to run Standard Plots
      self.doStandardPlots         = self.postProcessing() and self.doStandardPlots()
      # control to run Standard Plots
      self.doPhysValMon            = self.postProcessing() and self.doPhysValMon()
      #
      # --------------------------------------------------------------------
      # ---- ntuple creation
      # --------------------------------------------------------------------
      #
      self.doPixelTrkNtuple      = self.doPixelTrkNtuple() and DetFlags.haveRIO.pixel_on()
      self.doSctTrkNtuple        = self.doSctTrkNtuple()   and DetFlags.haveRIO.SCT_on()
      self.doTrtTrkNtuple        = self.doTrtTrkNtuple()   and DetFlags.haveRIO.TRT_on()
      #
      # --------------------------------------------------------------------
      # ---- D3PD creation
      # --------------------------------------------------------------------
      #
      self.doPixelTrkD3PD      = self.doPixelTrkD3PD() and DetFlags.haveRIO.pixel_on()
      self.doSctTrkD3PD        = self.doSctTrkD3PD()   and DetFlags.haveRIO.SCT_on()
      self.doTrtTrkD3PD        = self.doTrtTrkD3PD()   and DetFlags.haveRIO.TRT_on()
      #
      # --------------------------------------------------------------------
      # ---- Loading of Tools (depends on settings above)
      # --------------------------------------------------------------------
      #
      # control if to load the tools
      self.loadTools  = self.loadTools() or self.doPattern() or self.doRefit() or self.postProcessing() or \
                        self.doNtupleCreation() or self.doD3PDCreation() or self.doMonitoring()
      #
      # --------------------------------------------------------------------
      # --- some configuration of options for the new tracking
      # --------------------------------------------------------------------
      #

      # -------------------------------------------------------------------
      # switch off material effects for cosmics without field
      # -------------------------------------------------------------------
      #if self.doCosmics() and not self.solenoidOn():
      #   self.materialInteractions = False

      # -------------------------------------------------------------------
      # switch off brem recovery without field
      # -------------------------------------------------------------------
      if  not self.solenoidOn():
         self.doBremRecovery = False

      # -------------------------------------------------------------------
      # some tracking switches
      # -------------------------------------------------------------------
      self.redoTRT_LR = DetFlags.haveRIO.TRT_on()
      if self.trtExtensionType() == 'DAF':
         self.redoTRT_LR            = False
      #
      # --------------------------------------------------------------------
      # ---- Refit of tracks
      # --------------------------------------------------------------------
      #
      if (self.trackFitterType() is not 'KalmanFitter' and self.trackFitterType() is not 'KalmanDNAFitter') :
         self.refitROT = True
      if not self.refitROT() and not self.redoTRT_LR() :
         print 'ConfiguredInDetFlags.py       WARNING refitROT and redoTRT_LR are both False, NOT RECOMMENDED!' 
      #
      # refKF needs a new method in IUpdator, where there is currently only one implementation
      if (self.trackFitterType() is 'ReferenceKalmanFitter'):
          self.kalmanUpdator = 'amg'
      #
      # check if a valid fitter has been used
      if not (    (self.trackFitterType() is 'KalmanFitter')
               or (self.trackFitterType() is 'KalmanDNAFitter')
               or (self.trackFitterType() is 'ReferenceKalmanFitter')
               or (self.trackFitterType() is 'DistributedKalmanFilter')
               or (self.trackFitterType() is 'GlobalChi2Fitter' )
               or (self.trackFitterType() is 'GaussianSumFilter') ):
         print 'InDetJobProperties.py       WARNING unregistered or invalid track fitter setup.'
         print '                                      --> re-setting to TrkKalmanFitter.'
         self.trackFitterType = 'KalmanFitter'

      #
      # --------------------------------------------------------------------
      # ---- TRT Phase
      # --------------------------------------------------------------------
      #
      # Collision phase is done on segments so can't run if TRT segements not run. Also don't run when running standalone pseudo tracking.
      if not self.doCosmics(): self.doTRTPhaseCalculation = self.doTRTPhaseCalculation() and DetFlags.haveRIO.TRT_on() and self.doPRDFormation() and not (self.doPseudoTracking and not self.doNewTracking)

      #
      # --------------------------------------------------------------------
      # ---- Track slimming
      # --------------------------------------------------------------------
      #
      # Turn off slimming if the refit is enabled (since this maybe done to unlsim
      # the tracks) unless explicitly set by the user
      if self.doRefit(): self.checkThenSet(self.doSlimming, False)      

      #
      # --------------------------------------------------------------------
      # ---- SG Deletion
      # --------------------------------------------------------------------
      #
      # Turn of SG deletion if we are producing ntuples or standard plots.
      self.doSGDeletion = self.doSGDeletion() and not self.doD3PDCreation()
      #
      # turn off enhanced vertex monitoring if primaryVertexSetup is not IterativeFinder
      self.doMonitoringPrimaryVertexingEnhanced = self.doMonitoringPrimaryVertexingEnhanced() and (self.primaryVertexSetup() == 'IterativeFinding')
      self.doVtxMonitoringD3PD                  = self.doVtxMonitoringD3PD() and (self.primaryVertexSetup() == 'IterativeFinding')
      self.doVertexFindingForMonitoring         = self.doVertexFindingForMonitoring() or (self.doMonitoringPrimaryVertexingEnhanced() or self.doVtxMonitoringD3PD()) and (self.primaryVertexSetup() == 'IterativeFinding')
      self.doSplitVertexFindingForMonitoring    = self.doSplitVertexFindingForMonitoring() or (self.doMonitoringPrimaryVertexingEnhanced() or self.doVtxMonitoringD3PD()) and (self.primaryVertexSetup() == 'IterativeFinding')

      #-----PRD Formation------------------------------------------------
      if not self.doPRDFormation() :
       self.checkThenSet(self.doPixelPRDFormation, False)
       self.checkThenSet(self.doSCT_PRDFormation,  False)
       self.checkThenSet(self.doTRT_PRDFormation,  False)
      
    # do this also if Enabled == False
    print "Initialization of InDetFlags finished - locking container!"
    self.lock_JobProperties()

  def loadTrackingGeometry(self):
    return self.loadTools()
  
  def doAmbiSolving(self):
    from AthenaCommon.DetFlags import DetFlags
    return (self.doNewTracking() or self.doBeamGas() or self.doTrackSegmentsPixel() \
            or self.doTrackSegmentsSCT() or self.doLargeD0() or self.doLowPtLargeD0() ) \
           and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on())
  
  def loadRotCreator(self):
    return self.loadTools()
  
  def loadUpdator(self):
    return self.loadTools()
  
  def loadExtrapolator(self):
    return self.loadTools()
  
  def loadFitter(self):
    return self.loadTools()
  
  def loadAssoTool(self):
    return self.loadTools()
    
  def loadSummaryTool(self):
    return self.loadTools()

  def doNewTrackingPattern(self):
    return self.doNewTracking() or self.doBackTracking() or self.doBeamGas() \
           or self.doLowPt() or self.doVeryLowPt() or self.doTRTStandalone() \
           or self.doForwardTracks() or self.doLargeD0() or self.doLowPtLargeD0()

  def doNewTrackingSegments(self):
    return self.doTrackSegmentsPixel() or self.doTrackSegmentsSCT() or self.doTrackSegmentsTRT()
 
  def doPattern(self):
    return self.doNewTrackingPattern() or self.doxKalman() or self.doiPatRec() or self.doNewTrackingSegments()
  
  def doTRTExtension(self):
    from AthenaCommon.DetFlags import DetFlags
    return ((self.doNewTracking() or self.doBeamGas() or self.doLargeD0() or self.doLowPtLargeD0()) \
             and DetFlags.haveRIO.TRT_on() ) and self.doTRTExtensionNew()
  
  def doExtensionProcessor(self):
    from AthenaCommon.DetFlags    import DetFlags
    return (self.doNewTracking() or self.doBeamGas() or self.doLargeD0() or self.doLowPtLargeD0()) \
            and DetFlags.haveRIO.TRT_on()
 
  def solenoidOn(self):
    from AthenaCommon.BFieldFlags import jobproperties
    return jobproperties.BField.solenoidOn()

  def usePixelDCS(self):
    from AthenaCommon.DetFlags    import DetFlags
    return (self.useDCS() and DetFlags.dcs.pixel_on())

  def useSctDCS(self):
    from AthenaCommon.DetFlags    import DetFlags 
    return (self.useDCS() and DetFlags.dcs.SCT_on())

  def useTrtDCS(self):
    from AthenaCommon.DetFlags    import DetFlags
    return (self.useDCS() and DetFlags.dcs.TRT_on())  

  def doNtupleCreation(self):
    return (self.doPixelClusterNtuple() or self.doSctClusterNtuple() or self.doTrtDriftCircleNtuple() or
	    self.doTrkNtuple() or self.doPixelTrkNtuple() or self.doSctTrkNtuple() or
            self.doTrtTrkNtuple() or self.doVtxNtuple() or self.doConvVtxNtuple() or
            self.doV0VtxNtuple())

  def doD3PDCreation(self):
    return (self.doTrkD3PD() or self.doPixelTrkD3PD() or self.doSctTrkD3PD() or
            self.doTrtTrkD3PD() or self.doVtxD3PD() or self.doVtxMonitoringD3PD() or self.doConvVtxD3PD() or
            self.doV0VtxD3PD() or self.doTriggerD3PD())
  
  def doMonitoring(self):
    return (self.doMonitoringGlobal() or self.doMonitoringPrimaryVertexingEnhanced() or self.doMonitoringPixel() or self.doMonitoringSCT() or
            self.doMonitoringTRT() or self.doMonitoringAlignment())

  def setTrackingOff(self):
    "Disable all tracking algorithms"
     
    if self.disableTracking():
       self.doCaloSeededBrem          = False # disables ROI creation
       self.doNewTracking             = False
       self.doBackTracking            = False
       self.doTRTStandalone           = False
       self.doTrtSegments             = False
       self.doLowPt                   = False  
       self.doVeryLowPt               = False  
       self.doForwardTracks           = False
       self.doLargeD0                 = False
       self.doLowPtLargeD0            = False
       self.doHadCaloSeededSSS        = False

       self.doxKalman                 = False
       self.doiPatRec                 = False
       
       self.doTrackSegmentsPixel      = False
       self.doTrackSegmentsSCT        = False
       self.doTrackSegmentsTRT        = False
       
       self.doBeamGas                 = False
       self.doBeamHalo                = False
       
       self.doLowBetaFinder           = False  # couple to post processing? 
    return
 
  def setInDetRecoOff(self):
    "Disable all ID reco: pre-processing, tracking, post-processing ..."
    if self.disableInDetReco():
       self.doCaloSeededBrem          = False # disables ROI creation
       self.preProcessing             = False
       self.doHadCaloSeededSSS        = False
       #self.doPRDFormation            = False
       #self.doSpacePointFormation     = False
       self.disableTracking           = True
       self.postProcessing            = False
       #self.doParticleCreation        = False
       self.doSlimming                = False
       self.doStatistics              = False  
    return

  def setRobustReco(self):
     "Robust reco settings, regardless of what set before"

     if self.doRobustReco() and jobproperties.Beam.beamType()=="collisions":
        self.doLowPt                = False
        self.useHVForSctDCS         = True
        self.primaryVertexSetup     = "DefaultFullFinding"
        self.primaryVertexCutSetup  = "StartUp"
        self.useBeamConstraint      = False
        self.useBroadClusterErrors  = True

     return

  def setDVRetracking(self):
     "Set the High-d0 re-track mode settings, regardless of what was set before"
     # --- FIXME:: Decide what could be changed by the user

     # --- an extra check
     if not self.doDVRetracking():
         return

     # [XXX JDC: Checked needed flags
     self.doLargeD0                = True
     self.useExistingTracksAsInput = True
     self.doSpacePointFormation    = True  # Sure?? I think yes, to use the previously removed hits?
     #self.doSiSPSeededTrackFinder  = True
     self.doNewTracking            = False
     self.doPRDFormation           = False
     #self.preProcessing            = True  ?? Not found where is used
     # [ switch off the other modes
     self.doLowPt                  = False
     self.doVeryLowPt              = False
     self.doForwardTracks          = False
     self.doBeamGas                = False
     self.doBeamHalo               = False
     self.doxKalman                = False
     self.doiPatRec                = False
     self.doBackTracking           = False
     self.doTRTStandalone          = False
     self.postProcessing           = True
     # --- enable prim/sec vertexing ?? Probably yes, but a DV-vertexing
     # --- instead
     # --- primary vertex setup 
     self.doVertexFinding         = False             
     self.primaryVertexSetup      = "IterativeFinding"
     self.primaryVertexCutSetup   = "Offline"                   
     self.priVtxCutLevel          = 3                    
     # --- sec vertexing setup
     self.secondaryVertexCutSetup = "PileUp"           
     self.conversionVertexCutSetup= "ConversionPileUp"
     self.doV0Finder              = False             
     self.doSimpleV0Finder        = False             
     self.doConversions           = False             
     self.doParticleCreation      = True
     self.doTrackSegmentsPixel    = False             
     self.doTrackSegmentsSCT      = False             
     self.doTrackSegmentsTRT      = False          
     #self.doSlimming              = False          
     #self.doSGDeletion            = False             
     #self.doTrkNtuple             = False             
     #self.doMonitoringGlobal      = False             
     #self.doMonitoringPrimaryVertexingEnhanced = False 
     #self.doMonitoringPixel       = False             
     #self.doMonitoringSCT         = False             
     #self.doMonitoringTRT         = False             
     #self.doMonitoringAlignment   = False             
     #self.doBremRecovery          = True              
     #self.doCaloSeededBrem        = True              
     #self.doHadCaloSeededSSS      = False             
     # --- Output
     self.keepAdditionalHitsOnTrackParticle = True
     #self.AODall                  = False             
     #self.doxAOD                  = True              
     
     return

# ----------------------------------------------------------------------------
# --- Printout of settings
# ----------------------------------------------------------------------------

  def printInfo(self) :
    print '****** Inner Detector Flags ********************************************************'
    if self.AODall() :
       print '*'
       print '* --------------------> write AOD classes for all trackers!'
       print '*'
    if self.doSLHC() :
       print '*'
       print '* --------------------> Special reconstruction for SLHC !'
       if self.doSLHCVeryForward():
          print '* --------------------> Including Very Forward Extension !' 
       print '*'
    if self.doIBL() :
       print '*'
       print '* --------------------> Special reconstruction for IBL !'
       print '*'
    if self.doDBM() :
       print '*'
       print '* --------------------> Special reconstruction for DBM !'
       print '*'
    if self.doDBMstandalone() :
       print '*'
       print '* --------------------> Standalone reconstruction for DBM !'
       print '*'
    if self.doHighPileup() :
       print '*'
       print '* --------------------> Special reconstruction for high pile-up !'
       print '*'
    if self.doVtxLumi() :
       print '*'
       print '* --------------------> Special reconstruction for vertex lumi measurement !'
       print '*'
    if self.doVtxBeamSpot() :
       print '*'
       print '* --------------------> Special reconstruction for vertex beamspot measurement !'
       print '*'
    if self.doCosmics() :
       print '*'
       print '* --------------------> Special reconstruction for cosmics !'
       print '*'
    if self.doHeavyIon() :
       print '*'
       print '* --------------------> Special reconstruction for heavy ions !'
       print '*'
    if self.doMinimalReco() :
       print '*'
       print '* --------------------> Minimal Reconstruction !'
       print '*'
    if self.doDVRetracking() :
       print '*'
       print '* --------------------> Special reconstruction for high-d0 tracks !'
       print '*'
    if self.disableInDetReco():
       print '*'
       print '* --------------------> Disabling Reconstruction !'
       print '*'
    if self.disableTracking() and not self.disableInDetReco():
       print '*'
       print '* --------------------> Disabling all tracking !'
       print '*'
    if self.doMinBias():
       print '*'
       print '* --------------------> old Min Bias Mode (no longer supported)'
       print '*'
    if self.doLowMuRunSetup():
       print '*'
       print '* --------------------> Low Mu Run Setup for Min Bias studies'
       print '*'
    if self.doRobustReco():
       print '*'
       print '* --------------------> Robust Reco Cuts'
       print '*'
    if self.doSingleCollisionVertexReco():
       print '*'
       print '* --------------------> Single collision vertex reco'
       print '*'   
    if self.doInnerDetectorCommissioning:
       print '*'
       print '* --------------------> Loose setting for Commissioning'
       print '*'   

    
    # -----------------------------------------
    print '*'
    print '* PreProcessing:'
    print '* =============='
    print '*'
    if self.InDet25nsec(): 
       print '* -----> 25 nsec setup for SCT/TRT' 
    else:
       print '* -----> 50 nsec setup for SCT/TRT' 
    print '*'
    if self.doPRDFormation() :
       print '* run PrepRawDataFormation'
       if self.doPixelClusterSplitting():
          if self.doTIDE_Ambi(): 
            print '* - run TIDE ambi with pixel cluster splitting' 
            print '*   splitting technique:                 ', self.pixelClusterSplittingType()
            print '*   split prob1 cut:                     ', self.pixelClusterSplitProb1()
            print '*   split prob2 cut:                     ', self.pixelClusterSplitProb2()
            print '*   Min split   pt: [MeV]                ', self.pixelClusterSplitMinPt() 
            if self.doTIDE_RescalePixelCovariances():
                print '*   rescaling pixel cluster covariances: ', self.doTIDE_RescalePixelCovariances() 
          else:
            print '* - run new Pixel clustering with splitting using analog information'
            print '*   splitting technique: ', self.pixelClusterSplittingType()
       if self.selectSCTIntimeHits():
           if self.InDet25nsec(): 
               print '* - use 01X masking for SCT readout in reconstruction' 
           else:  
               print '* - use X1X masking for SCT readout in reconstruction' 
       if self.cutSCTOccupancy():
           print '* - cut on SCT occupancy for masking noisy modules'
       if self.removeTRTNoise():
           print '* - remove flagged TRT noise' 
       if self.noTRTTiming():
           print '* - ignore TRT timing information !'
    if self.doSpacePointFormation() :
       print '* run SpacePointFormation'
    # -----------------------------------------
    print '*'
    print '* TrackFinding:'
    print '* ============='
    # -----------------------------------------
    if self.doNewTracking() :
       print '*'
       print '* NewTracking is ON:'
    if self.doSiSPSeededTrackFinder() :
       print '* - run SiSPSeededTrackFinder'
       if self.useZvertexTool() :
          print '*   and use ZvertexTool'
    if self.doAmbiSolving() :
       print '* - run AmbiguitySolver'
    if self.doTRTExtension() :
       print '* - run TRT_TrackExtension'
    if self.doExtensionProcessor() :
       print '* - run TrackExtensionProcessor'
       if self.redoTRT_LR() :
          print '*   and redo LR and tube hits in fit for TRT !!!'
    # -----------------------------------------
    if self.doBackTracking() :
       print '*'
       print '* BackTracking is ON:'
    if self.doTrtSegments() :
       print '* - run TRT Segment finding'
    if self.doTRTSeededTrackFinder() :
       print '* - run TRT seeded track finding'
       if self.loadTRTSeededSPFinder() :
          print '*   and load TRT_SeededSpacePointFinder'
       if self.loadSimpleTRTSeededSPFinder() :
          print '*   and load SimpleTRT_SeededSpacePointFinder'
    if self.doResolveBackTracks() :
       print '* - run ambi on TRT seeded tracks'
    if self.doTRTStandalone() :
       print '* - create TRT standalone tracks'
    # -----------------------------------------
    if self.doNewTrackingSegments():
       print '*'
       print '* Create separate track segments for:'
       standAloneTracking = '*   - '
       if self.doTrackSegmentsPixel():
          standAloneTracking += 'Pixel '
       if self.doTrackSegmentsSCT():
          standAloneTracking += 'SCT '
       if self.doTrackSegmentsTRT():
          standAloneTracking += 'TRT'
       print standAloneTracking
    # -----------------------------------------
    if self.doLargeD0() or self.doLowPtLargeD0() :
       print '*'
       print '* LargeD0 Tracking is ON'
       if self.doSiSPSeededTrackFinder() :
          print '* - run SiSPSeededTrackFinder'
       if self.useZvertexTool() :
          print '*   and use ZvertexTool'
       if self.doAmbiSolving() :
          print '* - run AmbiguitySolver'
       if self.doTRTExtension() :
          print '* - run TRT_TrackExtension'
       if self.doExtensionProcessor() :
          print '* - run TrackExtensionProcessor'
          if self.redoTRT_LR() :
              print '*   and redo LR and tube hits in fit for TRT !!!'
    # -----------------------------------------
    if self.useExistingTracksAsInput() :
       print '*'
       print '* Use (D)ESD tracks as input'
    # -----------------------------------------
    if self.doLowPt() :
       print '*'
       print '* LowPtTracking is ON'
       if self.doVeryLowPt() :
          print '* and VeryLowPtTracking is ON'
    # -----------------------------------------
    if self.doSLHCConversionFinding() :
       print '*'
       print '* SLHCConversionFinding is ON'
    # -----------------------------------------
    if self.doForwardTracks():
       print '*'
       print '* Forward Tracklets are ON'
    # -----------------------------------------
    print '*'
    print '* Cut level for Tracking is set to : ',self.cutLevel()
    if not self.doHeavyIon():
       print '*    (1) - 2010 settings'
       print '*    (2) - start of 2011 settings'
       print '*    (3) - tighter version of (2)'
       print '*    (4) - max d0 cut on SSS seeds'
       print '*    (5) - cut level 4 + seed level 2'
       print '*    (6) - TRTonly cuts for pileup' 
       print '*    (7) - BackTracking cuts for pileup'
       print '*    (8) - slightly tighter hole cuts for NewTracking'
       print '*    (9) - slightly tighter IP cuts'
       print '*    (10)- use Z boundary seeding'
       print '*    (11)- TRT only uses eta depending hit cuts'
       print '*    (12)- Tighter X2 cuts on both track and hits being accepted'
       print '*    (13)- TRT segment making in RoI guided model'
    else:
       print '*    (1) - 2010 heavy ion settings'
       print '*    (2) - 2011 heavy ion settings with seed level 2'
       print '*    (3) - 2011 heavy ion settings with seed level 2 and pT cut at 0.3 GeV'
    # -----------------------------------------
    if self.doBremRecovery():
       print '* run Brem Recovery in tracking'
       if self.doCaloSeededBrem():
          print '* - restrict Brem Recovery to Calo ROIs'
    # -----------------------------------------
    if self.doxKalman() or self.doiPatRec() or self.doFatras():
       print '*'
       print '* Alternative trackings:'
       if self.doxKalman() :
          print '*   - run xKalman'
       if self.doiPatRec() :
          print '*   - run iPatRec'
       if self.doFatras() :
          print '*   - run FATRAS'
    # -----------------------------------------
    if self.doRefit() :
       print '*'
       print '* do a refit of all tracks'
    if self.doSlimming() :
       print '*'
       if not self.doSlimPoolTrack() :
          print '* slim down the tracks for output on ESD'
       else :
          print '* persistify slim tracks '
    # -----------------------------------------
    print '*'
    print '* PostProcessing:'
    print '* ==============='
    print '*'
    if self.doVertexFinding() :
       print '* run primary vertex finding with : ',self.primaryVertexSetup()
       print '* - using sorting based on        : ',self.primaryVertexSortingSetup()
       print '* - primary vertexing cut setup   : ',self.primaryVertexCutSetup()
       if self.doPrimaryVertex3DFinding() :
          print '* - use 3D seed finding'
       print '* - privtx cut level : ', self.priVtxCutLevel()
    if self.doParticleCreation() :
       print '* create TrackParticles'
       if self.doSharedHits() :
          print '* - and do shared hits search'
       if self.KeepFirstParameters() :
          print '* - keep first parameters on track'
       elif self.KeepParameters() :
          print '* - keep extra parameters on track'
    if self.doV0Finder() :
       print '* run V0 finder'
       if self.useV0Fitter() :
          print '* - use V0 fitter'
       else:
          print '* - use VkalVrt'
          pass
       if self.doSimpleV0Finder() :
          print '* - running with simple V0Finder cuts'
       else:
          print '* - running with full V0Finder cuts'
          pass
       pass
    #if self.doSimpleV0Finder() :
    #   print '* run simple V0 finder'
    #   if self.useV0Fitter() :
    #      print '* - use V0 fitter'
    #   else:
    #      print '* - use VkalVrt'
    #      pass
    #   pass
    if self.doSecVertexFinder() :
       print '* run V0 search using conversion finder with vertexing cut setup : ',self.secondaryVertexCutSetup()
    if self.doConversions() :
       print '* run conversion finder with conversion vertexing cut setup      : ',self.conversionVertexCutSetup()
    # -----------------------------------------
    if self.doStatistics() :
       print '* run statistics packages'
    if self.doStandardPlots() :
       print '* run Standard Plots package'
    if self.doPhysValMon() :
       print '* run Physics Validation Monitoring'
    if self.doNtupleCreation():
       ntupleString = '* Ntuple cluster/drift circle trees activated:'
       if self.doPixelClusterNtuple():
          ntupleString += ' Pixel'
       if self.doSctClusterNtuple():
          ntupleString += ' SCT'
       if self.doTrtDriftCircleNtuple():
          ntupleString += ' TRT'
       if self.doPixelClusterNtuple() or self.doSctClusterNtuple() or self.doTrtDriftCircleNtuple():
          print ntupleString

       ntupleString = '* Ntuple track trees activated:'
       if self.doTrkNtuple():
          ntupleString += ' \"Basic Track Tree\"'
       if self.doPixelTrkNtuple():
          ntupleString += ' \"Pixel Track Extension\"'
       if self.doSctTrkNtuple():
          ntupleString += ' \"SCT Track Extension\"'
       if self.doTrtTrkNtuple():
          ntupleString += ' \"TRT Track Extension\"'
       if self.doTrkNtuple() or self.doPixelTrkNtuple() or self.doSctTrkNtuple() or self.doTrtTrkNtuple():
          print ntupleString

       ntupleString = '* Ntuple vertex trees activated:'
       if self.doVtxNtuple():
          ntupleString += ' PrimaryVertexing'
       if self.doConvVtxNtuple():
          ntupleString += ' Conversions'
       if self.doV0VtxNtuple():
          ntupleString += ' V0s'
       if self.doVtxNtuple() or self.doConvVtxNtuple() or self.doV0VtxNtuple():
          print ntupleString

    if self.doD3PDCreation():
       ntupleString = '* D3PD track trees activated:'
       if self.doTrkD3PD():
          ntupleString += ' \"Basic Track Tree\"'
       if self.doPixelTrkD3PD():
          ntupleString += ' \"Pixel Tracklets\"'
       if self.doSctTrkD3PD():
          ntupleString += ' \"SCT Tracklets\"'
       if self.doTrtTrkD3PD():
          ntupleString += ' \"TRT Tracklets\"'
       if self.doTrkD3PD() or self.doPixelTrkD3PD() or self.doSctTrkD3PD() or self.doTrtTrkD3PD():
          print ntupleString

       ntupleString = '* D3PD vertex trees activated:'
       if self.doVtxD3PD():
          ntupleString += ' PrimaryVertexing'
       if self.doVtxMonitoringD3PD():
          ntupleString += ' EnhPriVtxMonitoring'
       if self.doConvVtxD3PD():
          ntupleString += ' Conversions'
       if self.doV0VtxD3PD():
          ntupleString += ' V0s'
       if self.doVtxD3PD() or self.doVtxMonitoringD3PD() or self.doConvVtxD3PD() or self.doV0VtxD3PD():
          print ntupleString

       if self.doTriggerD3PD():
          print '* D3PD trigger tree activated'

    # -----------------------------------------
    if (self.doMonitoringGlobal() or self.doMonitoringPrimaryVertexingEnhanced() or self.doMonitoringPixel() or self.doMonitoringSCT() or self.doMonitoringTRT() or self.doMonitoringAlignment()):
       print '*'
       myString = '* Run Monitoring on for: '
       if self.doMonitoringGlobal():
         myString += ' Global'
       if self.doMonitoringPrimaryVertexingEnhanced():
         myString += ' Enhanced Primary Vertexing'
       if self.doMonitoringPixel():
         myString += ' Pixel'
       if self.doMonitoringSCT():
         myString += ' SCT'
       if self.doMonitoringTRT():
         myString += ' TRT'
       if self.doMonitoringAlignment():
         myString += ' Alignment'
       print myString
       print '*'
    # -----------------------------------------
    print '*'
    print '* Other parameters'
    print '* ================='
    print '*'
    if self.doTruth() :
       print '* run truth association: '+self.truthMatchStrategy()
    if self.solenoidOn() :
       print '* solenoid field is ON'
    if self.usePixelDCS():
       print '* use Pixel DCS'
    if self.useSctDCS():
       if not self.useHVForSctDCS():
          print '* use SCT DCS'
       else:
          print '* use non-standard SCT DCS based on ~20V HV cut'          
    if self.useTrtDCS():
       print '* use TRT DCS'
    if self.useDynamicAlignFolders():
       print '* use of Dynamic alignment folder scheme enabled'

    if not self.doPRDFormation():
       print '* PRD Formation is off for all technologies'
    if not self.doPixelPRDFormation():
       print '* Pixel PRD Formation is off'
    if not self.doSCT_PRDFormation():
       print '* SCT PRD Formation is off'
    if not self.doTRT_PRDFormation():
       print '* TRT PRD Formation is off'

    # -----------------------------------------
    print '*'
    print '* Configurable Services loaded:'
    print '* ============================='
    print '*'
    if self.loadTrackingGeometry() :
      print '* load tracking geometry for Inner Detector'
    if self.useBeamConstraint() :
      print '* use Beam Spot Constraint in reconstruction/vertexing'
    # -----------------------------------------
    print '*'
    print '* Configurable Tools loaded:'
    print '* =========================='
    print '*'
    if self.loadRotCreator() :
      print '* load ROT_Creator for Inner Detector'
      if self.useBroadClusterErrors():
        print '* - (commissioning) use broad cluster errors !'
    if self.loadTrackingGeometry():
      print '* load TrackingGeometry'
    if self.loadExtrapolator() :
      print '* load Extrapolator:'
      if self.propagatorType() is 'RungeKutta' :
        print '* - load Runge Kutta propagator'
      elif self.propagatorType() is 'STEP' :
        print '* - load STEP propagator'
      if self.materialInteractions() :
        print '* - use material corrections of type %s in extrapolation and fit'% self.materialInteractionsType()
    if self.loadUpdator() :
      if self.kalmanUpdator() is "fast" :
        print '* load MeasurementUpdator_xk'
      else:
        print '* load MeasurementUpdator'
    if self.loadFitter() :
      print '* load track fitter of type ', self.trackFitterType()
      if self.refitROT() :
        print '* - refit from ROT'
      else:
        print '* - refit from PRD, redo the ROTs'
    if self.loadAssoTool() :
      print '* load PRD_AssociationToolGangedPixels'
    if self.loadSummaryTool() :
      print '* load TrackSummaryTool'
      if self.doHolesOnTrack() :
        print '* - and do holes on track search'
      print '*'  
    print '************************************************************************************'
    
# ----------------------------------------------------------------------------
# --- set methods for less often used switches
# ----------------------------------------------------------------------------
  
  # decide if the SiSpacePoint track finder should look for primary vertices
  # in z and use this constraint during pattern recognition
  def usePrimVertexZcoordinate ( self, usezvertex ) :
    self.useZvertexTool = usezvertex
    if not self.doNewTracking() :
      print 'ConfiguredInDetFlags.py       WARNING toggling the z-vertex constraint has no effect'
      print '                                      because the new tracking IS NOT SWITCHED ON!'

# ----------------------------------------------------------------------------
# --- set different vertexing options
# ----------------------------------------------------------------------------

  #def enableTrackSlimming(self, doSlimming):
    #self.doSlimming = doSlimming

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(InDetJobProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetJobProperties container
_list_InDetJobProperties = [Enabled,
                            doPrintConfigurables,
                            doNewTracking,
                            doPseudoTracking,
                            doSplitReco,
                            doxKalman,
                            doiPatRec,
                            preProcessing,
                            doPRDFormation,
                            doPixelPRDFormation,
                            doSCT_PRDFormation,
                            doTRT_PRDFormation,
                            doSpacePointFormation,
                            doRefit,
                            doSLHC,
                            doIBL,
                            doHighPileup,
                            doMinimalReco,
                            doDVRetracking,
                            postProcessing,
                            doTruth,
                            loadTools,
                            doBackTracking,
                            doLowPt,
                            doVeryLowPt,
                            doSLHCConversionFinding,
                            doForwardTracks,
                            doLowPtLargeD0,
                            doLargeD0,
                            useExistingTracksAsInput,
                            cutLevel,
                            priVtxCutLevel,
                            doBremRecovery,
                            doCaloSeededBrem,
                            doHadCaloSeededSSS,
                            doBeamGas,
                            doBeamHalo,
                            doVtxLumi,
                            doVtxBeamSpot,
                            doCosmics,
                            doHeavyIon,
                            doParticleCreation,
                            KeepParameters,
                            KeepFirstParameters,
                            doTrackSegmentsPixel,
                            doTrackSegmentsSCT,
                            doTrackSegmentsTRT,
                            doMonitoringGlobal,
                            doMonitoringPrimaryVertexingEnhanced,
                            doMonitoringPixel,
                            doMonitoringSCT,
                            doMonitoringTRT,
                            doMonitoringAlignment,
                            useDynamicAlignFolders,
                            doPerfMon,
                            AODall,
                            useBeamConstraint,
                            kalmanUpdator,
                            magField,
                            propagatorType,
                            trackFitterType,
                            doHolesOnTrack,
                            useZvertexTool,
                            doSiSPSeededTrackFinder,
#                            doTRTExtension,
                            doTRTExtensionNew,
                            trtExtensionType,
                            redoTRT_LR,
                            doTrtSegments,
                            doTRTPhaseCalculation,
                            doTRTSeededTrackFinder,
                            loadTRTSeededSPFinder,
                            loadSimpleTRTSeededSPFinder,
                            doResolveBackTracks,
                            doTRTStandalone,
                            refitROT,
                            doSlimming,
                            doSlimPoolTrack,
                            doWriteTracksToESD,
                            doVertexFinding,
                            primaryVertexSetup,
                            primaryVertexCutSetup,
                            vertexSeedFinder,
                            primaryVertexSortingSetup,
                            doPrimaryVertex3DFinding,
                            doVertexFindingForMonitoring,
                            doSplitVertexFindingForMonitoring,
                            perigeeExpression,
                            secondaryVertexCutSetup,
                            conversionVertexCutSetup,
                            doSharedHits,
                            doV0Finder,
                            doSimpleV0Finder,
                            useV0Fitter,
                            doSecVertexFinder,
                            doConversions,
                            doStatistics,
                            doStandardPlots,
                            doPhysValMon,
                            materialInteractions,
                            materialInteractionsType,
                            doPixelClusterNtuple,
                            doSctClusterNtuple,
                            doTrtDriftCircleNtuple,
                            doTrkNtuple,
                            doPixelTrkNtuple,
                            doSctTrkNtuple,
                            doTrtTrkNtuple,
                            doVtxNtuple,
                            doConvVtxNtuple,
                            doV0VtxNtuple,
                            doTrkD3PD,
                            doPixelTrkD3PD,
                            doSctTrkD3PD,
                            doTrtTrkD3PD,
                            doVtxD3PD,
                            doVtxMonitoringD3PD,
                            doConvVtxD3PD,
                            doV0VtxD3PD,
                            doTriggerD3PD,
                            removeTRTNoise,
                            noTRTTiming,
                            InDet25nsec,
                            selectSCTIntimeHits,
                            cutSCTOccupancy,
                            useDCS,
                            truthMatchStrategy,
                            doFatras,
                            doSGDeletion,
                            doLowBetaFinder,
                            useHVForSctDCS,
                            disableTracking,
                            disableInDetReco,
                            doPixelClusterSplitting,
                            pixelClusterSplittingType,
                            pixelClusterSplitProb1,
                            pixelClusterSplitProb2,
                            pixelClusterSplitProb1_run1,
                            pixelClusterSplitProb2_run1,
                            pixelClusterSplitMinPt,
                            pixelClusterBadClusterID,
                            useBroadClusterErrors,
                            useBroadPixClusterErrors,
                            useBroadSCTClusterErrors,
                            writeRDOs,
                            writePRDs,
                            doMinBias,
                            doLowMuRunSetup,
                            doRobustReco,
                            doTIDE_AmbiTrackMonitoring,
                            doSingleCollisionVertexReco,
                            useMBTSTimeDiff,
                            useNewSiSPSeededTF,
                            doxAOD,
                            doCaloSeededTRTSegments,
                            doInnerDetectorCommissioning,
                            doTIDE_Ambi,
                            doRefitInvalidCov,
                            doRejectInvalidCov,
                            doTIDE_RescalePixelCovariances,
                            doSSSfilter,
                            pT_SSScut,
                            ForceCoraCool,
                            ForceCoolVectorPayload,
                            doTrackSegmentsPixelPrdAssociation,
                            doSLHCVeryForward,
                            doTRTGlobalOccupancy,
                            doNNToTCalibration,
                            keepAdditionalHitsOnTrackParticle,
                            doSCTModuleVeto,
                            doDBMstandalone,
                            doDBM,
                            doParticleConversion,
                            doStoreTrackSeeds,
                            doHIP300,
                            checkDeadElementsOnTrack
                           ]
for j in _list_InDetJobProperties: 
    jobproperties.InDetJobProperties.add_JobProperty(j)
#keep the list alive for the trigger
#del _list_InDetJobProperties

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select InDetJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags = jobproperties.InDetJobProperties
