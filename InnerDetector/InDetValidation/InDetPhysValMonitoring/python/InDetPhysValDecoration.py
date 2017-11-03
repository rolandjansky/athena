# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# block('InDetPhysValMonitoring/InDetPhysValDecoration.py')
import ConfigUtils

# ---- definitions
def metaDataKey() :
    '''
    Meta data key to store the file source on which the InDet decoration alg has been running.
    '''
    return 'InDetPhysValDecoration'

def monManName() :
    '''
    Name of the monitoring manager
    '''
    return 'PhysValMonManager'

def findAlg(alg_name) :
  '''
     Find the algorithm alg_name in the top sequence and return its index.
     if no algorithm of the given name is found the index just before the AhtenaOutputStream is returned.
     if all that fails None is returned.
  '''
  if not isinstance(alg_name, (list, tuple)) :
      print >> sys.stderr,'ERROR logic error findAlg called with a non list argument %s / %s' %(alg_name,type(alg_name))
      import sys
      sys.exit(1)

  from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
  topSequence = AlgSequence()
  count=0
  mon_man_index=None
  for child in topSequence.getChildren() :
     # if child.getName() in alg_name :
     if child.getName() in alg_name :
         # print 'DEBUG findAlg %s =?=  %s ' %(child.getName(),alg_name)
         mon_man_index = count
         if len(alg_name) == 1 :
           break
     count += 1

  if mon_man_index == None :
     count=0
     exclude_streams=['StreamBS']
     for child in topSequence.getChildren() :
         if child.getType() == 'AthenaOutputStream' and child.getName() not in exclude_streams:
             # print 'DEBUG found %s at postion %i/%i' % (child.getFullName(),count,len(topSequence.getChildren() ) )
             mon_man_index = count
             break
         count += 1
  return mon_man_index

def findMonMan() :
  '''
  Find the monitoring manager and return its index in the top sequence or None
  '''

  return findAlg( [monManName()] )


def getMetaData() :
   '''
   Try to determine from the meta data whether the decoration has been performed already.  
   '''
   from RecExConfig.RecFlags import rec
   if not rec.readRDO():
      from RecExConfig.InputFilePeeker import inputFileSummary
      try:
          # print 'DEBUG InDetPhysValMonitoring getMetaData %s' % inputFileSummary['metadata']
          return inputFileSummary['metadata']['/TagInfo'][metaDataKey()]
      except Exception :
          pass
   return ''

def setMetaData() :
   '''
   Write the input source on which the InDet decoration algorithm was running to the meta data
   '''

   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   if metaDataKey() in svcMgr.TagInfoMgr.ExtraTagValuePairs  :
      return

   from RecExConfig.RecFlags import rec
   str=''
   if rec.readRDO() :
       str='fromRDO'
   elif rec.readESD() :
       str='fromESD'
   elif rec.readAOD() :
       str='fromAOD'

   if metaDataKey() not in svcMgr.TagInfoMgr.ExtraTagValuePairs :
      svcMgr.TagInfoMgr.ExtraTagValuePairs += [metaDataKey(), str]
   # print 'DEBUG add meta data %s.' % svcMgr.TagInfoMgr.ExtraTagValuePairs


from ConfigUtils import _args
from ConfigUtils import injectNameArgument
from ConfigUtils import dumpConfigurables
from ConfigUtils import checkKWArgs
from ConfigUtils import toolFactory

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
class InDetHoleSearchTool(object) :
  '''
  Namespace for inner detector hole search tools
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class PhysValMonInDetHoleSearchTool(InDet__InDetTrackHoleSearchTool) :
      '''
      default inner detector hole search tool
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          # if cls.__name__ in cls.configurables :
          #     print 'WARNING configurable %s exists already ' % cls.__name__
          return InDet__InDetTrackHoleSearchTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
          # If InDetSCT_ConditionsSummarySvc instance configured by InDetRecConditionsAccess.py is available, use it.
          # Otherwise, the default SCT_ConditionsSummarySvc instance is used.
          # @TODO find a better to solution to get the correct service for the current job.
          SctSummarySvc = "InDetSCT_ConditionsSummarySvc"
          if not hasattr(ServiceMgr, SctSummarySvc):
              SctSummarySvc = "SCT_ConditionsSummarySvc"
          super(InDetHoleSearchTool.PhysValMonInDetHoleSearchTool,self).__init__(**_args( kwargs,
                                                                                                 name         = self.__class__.__name__,
                                                                                                 Extrapolator = ToolSvc.InDetExtrapolator,
                                                                                                 SctSummarySvc = SctSummarySvc,
                                                                                                 usePixel     = True,
                                                                                                 useSCT       = True,
                                                                                                 # OutputLevel  = 1,
                                                                                                 CountDeadModulesAfterLastHit = True) )


import InDetPhysValMonitoring.InDetPhysValMonitoringConf
class InDetPhysHitDecoratorTool(object) :
  '''
  Namespace for hit decoration tool
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class InDetPhysHitDecoratorTool(InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysHitDecoratorTool) :
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysHitDecoratorTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysHitDecoratorTool.InDetPhysHitDecoratorTool,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # custom configuration here:
          self.InDetTrackHoleSearchTool = toolFactory(InDetHoleSearchTool.PhysValMonInDetHoleSearchTool)


# Configuration of InDetPhysValDecoratorAlg algorithms
# 
import InDetPhysValMonitoring.InDetPhysValMonitoringConf
class InDetPhysValDecoratorAlg(object) :
  '''
  Namespace for track aprticle decoration algorithms
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class InDetPhysValDecoratorAlg(InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValDecoratorAlg) :
      '''
      Default track particle decoration algorithm.
      This algorithm will decorate the InDetTrackParticles.
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValDecoratorAlg.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlg,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # custom configurations below:
          self.InDetPhysHitDecoratorTool = toolFactory(InDetPhysHitDecoratorTool.InDetPhysHitDecoratorTool)  
          # self.InDetPhysHitDecoratorTool.OutputLevel=1
          from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC
          if not isMC() :
              # disable truth monitoring for data
              self.TruthParticleContainerName = ''


class InDetPhysValKeys :
    '''
    Keys to decorate and monitor GSF Tracks/TrackParticles
    '''
    # the name of unslimmed GSF Tracks defined here
    GSFTracksUnslimmed = 'GSFTracksUnslimmed'

    # the name of GSF Tracks given by egamma
    GSFTracks = 'GSFTracks'

    # the name of GSF TrackParticles given by egamma
    # @todo should be used also in InDetPhysValMonitoring.py
    GSFTrackParticles = 'GSFTrackParticles'

class InDetPhysValDecoratorAlg(InDetPhysValDecoratorAlg) :

  class InDetPhysValDecoratorAlgGSF(InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlg) :
      '''
      Algorithm to decorate GSF track particles.
      '''
      def __init__(self, **kwargs) :
          super(InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlgGSF,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))
          # custom configuration below:
          self.TrackParticleContainerName=InDetPhysValKeys.GSFTrackParticles


  class InDetPhysValDecoratorAlgDBM(InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlg) :
      '''
      Algorithm to decorate DBM Track particles.
      '''
      def __init__(self, **kwargs) :
          super(InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlgDBM,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))
          # custom configuration below:
          from InDetRecExample.InDetKeys import InDetKeys
          self.TrackParticleContainerName=InDetKeys.DBMTrackParticles()


# Debugging
# import trace
# tracer = trace.Trace()
# tracer.runfunc( )

def _addDecorators(decorator_alg_list, add_after=None) :
  '''
  Add the given decorator algorithms to the top sequence. 
  The algorithm is to be run on RAW/RDO since it depends on full hit information
  which is generally not available at later stages. The decorations added by this
  algorithm are used by InDetPhysValMonitoring tool.
  '''

  if add_after != None and not isinstance(add_after, (list, tuple)) :
      print >> sys.stderr,'ERROR logic error findAlg called with a non list argument %s / %s' %(alg_name,type(alg_name))
      import sys
      sys.exit(1)

  # Access the algorithm sequence:
  from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
  topSequence = AlgSequence()
  # print 'DEBUG add _addDecorators add after %s ' % (add_after)

  # if there is a monitoring manager add decorator before
  mon_man_index=findMonMan()
  if mon_man_index == None and add_after != None :
      alg_index = findAlg(add_after )
      if alg_index != None :
          # add after the found algorithm
          mon_man_index =alg_index + 1
  # print 'DEBUG _addDecorators after this %s ' % (mon_man_index)

  if mon_man_index == None :
     for decorator_alg in decorator_alg_list :
        if findAlg([decorator_alg.getName()]) != None :
            print 'DEBUG decorator %s already in sequence. Not adding again.' % (decorator_alg.getFullName())
            continue
        print 'DEBUG add decorator %s at end of top sequence:' % (decorator_alg.getFullName())
        topSequence += decorator_alg

  else :
      for decorator_alg in decorator_alg_list :
         if findAlg([decorator_alg.getName()]) != None :
            print 'DEBUG decorator %s already in sequence. Not inserting again.' % (decorator_alg.getFullName())
            continue
         print 'DEBUG insert decorator %s at position %i' % (decorator_alg.getFullName(),mon_man_index)
         topSequence.insert(mon_man_index,decorator_alg)
         mon_man_index += 1
  setMetaData()

def addGSFTrackDecoratorAlg() :
   '''
   Search egamma algorithm and add the GSF TrackParticle decorator after the it.
   '''
   from InDetPhysValMonitoring.InDetPhysValDecoration    import _addDecorators
   from InDetPhysValMonitoring.InDetPhysValDecoration    import InDetPhysValDecoratorAlg

   from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
   if InDetPhysValFlags.doValidateGSFTracks() :
      # print 'DEBUG add addGSFTrackDecoratorAlg'
      decorators=[InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlgGSF() ]
      # add the InDetPhysValDecoratorAlgGSF after the egamma algorithms ran
      # they build the GSF track particles.
      _addDecorators( decorators, ['egamma','egammaTruthAssociationAlg'] )

      # To allow for proper decoration of GSF TrackParticles
      #  - have to switch of slimming for GSF Tracks at time of creation
      #  - must slim GSF Tracks after decoration since the unslimmed GSF Tracks cannot be persistified

      from AthenaCommon.AppMgr import ToolSvc
      # print ToolSvc
      # print 'DEBUG has EMBremCollectionBuilder %s' % hasattr(ToolSvc,'EMBremCollectionBuilder')
      if hasattr(ToolSvc,'EMBremCollectionBuilder') :
          decor_index = findAlg([decorators[0].getName()])
          if decor_index != None :
              from TrkTrackSlimmer.TrkTrackSlimmerConf import Trk__TrackSlimmer as ConfigurableTrackSlimmer
              slimmer = ConfigurableTrackSlimmer(name                 = "RealGSFTrackSlimmer",
                                                 TrackLocation        = [ InDetPhysValKeys.GSFTracksUnslimmed ],
                                                 SlimmedTrackLocation = [ InDetPhysValKeys.GSFTracks ],
                                                 TrackSlimmingTool    = ToolSvc.EMBremCollectionBuilder.TrackSlimmingTool )

              from InDetPhysValMonitoring.InDetPhysValMonitoringConf import DummyTrackSlimmingTool
              slimming_tool = DummyTrackSlimmingTool()
              ToolSvc += slimming_tool
              ToolSvc.EMBremCollectionBuilder.TrackSlimmingTool = slimming_tool
              ToolSvc.EMBremCollectionBuilder.OutputTrackContainerName=InDetPhysValKeys.GSFTracksUnslimmed
              # ToolSvc.ResidualPullCalculator.OutputLevel = 1

              from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
              topSequence = AlgSequence()
              topSequence.insert(decor_index+1,slimmer)
      # import sys
      # sys.exit(1)


def addDecorator() :
  '''
  Add the track particle decoration algorithm to the top sequence. 
  The algorithm is to be run on RAW/RDO since it depends on full hit information
  which is generally not available at later stages. The decorations added by this
  algorithm are used by InDetPhysValMonitoring tool.
  '''

  decorators=[]

  decorators.append( InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlg() )

  from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
  InDetPhysValFlags.init()
  if InDetPhysValFlags.doValidateGSFTracks() :
    # cannot add the decorator for GSF tracks at this moment because the egamma algorithm has not been
    # constructed yet.
    # so this does not work: decorators.append( InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlgGSF() )
    # Can only schedule a user algorithm :
    from RecExConfig.RecFlags import rec
    rec.UserExecs += ['from InDetPhysValMonitoring.InDetPhysValDecoration import addGSFTrackDecoratorAlg;addGSFTrackDecoratorAlg();']

  # from  InDetRecExample.InDetJobProperties import InDetFlags
  from InDetRecExample.InDetKeys import InDetKeys
  # for backward compatibility check whether DBM has been added already
  if InDetPhysValFlags.doValidateDBMTracks() and hasattr(InDetKeys,'DBMTrackParticles') :
    # and InDetFlags.doDBM()
    decorators.append( InDetPhysValDecoratorAlg.InDetPhysValDecoratorAlgDBM() )

  _addDecorators( decorators )
 
def addExtraMonitoring() :
  '''
  IF monitoring is wished for GSF or DBM TrackParticles find the monitoring manager and 
  add the corresponding monitoring tools.
  '''
  # hack to add monitors for DBM and GSF
  # the job option fragment which adds the InDetPhysValMonitoringTool for the default tracks
  # will call this method, so can abuse this method to also add the monitoring tools for DBM and GSF tracks
  try:
   from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
   # flags are at this stage already initialised, so do not need to  InDetPhysValFlags.init()
   if InDetPhysValFlags.doValidateGSFTracks() or InDetPhysValFlags.doValidateDBMTracks() or InDetPhysValFlags.doValidateTightPrimaryTracks() :
       mon_index = findMonMan()
       if mon_index != None :
           from AthenaCommon.AlgSequence import AlgSequence
           topSequence = AlgSequence()
           mon_manager = topSequence.getChildren()[mon_index]
           from InDetPhysValMonitoring.InDetPhysValMonitoringTool import InDetPhysValMonitoringTool

  except ImportError :
    import sys,traceback
    exc_type, exc_value, exc_traceback = sys.exc_info()
    print "*** print_tb:"
    traceback.print_tb(exc_traceback, limit=1, file=sys.stdout)
    print "*** print_exception:"
    traceback.print_exception(exc_type, exc_value, exc_traceback,
                              limit=2, file=sys.stdout)
    print "*** print_exc:"
    traceback.print_exc()
    raise

def canAddDecorator() :
    '''
    check whether the decorator can be added.

    A decorator can be added if a track particle converter alg is in the sequence or
    if ESDs or AODs are read.
    '''
    from RecExConfig.RecFlags import rec
    if not rec.doInDet :
        return False

    if rec.readAOD :
        return True

    if rec.readESD :
        return True

    if rec.readTAG :
        return False

    if rec.readRDO :
      try :
        from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
        topSequence = AlgSequence()
        import re
        pat =re.compile('.*(TrackParticleCnvAlg).*')
        for alg in topSequence.getChildren() :
            if pat.match( alg.getFullName() ) != None :
                return True

      except :
        pass
    
    return False

def addDecoratorIfNeeded() :
   '''
    Run the InDet decoration algorithm if it has not been ran yet.
   '''

   if  not canAddDecorator() :
         print 'DEBUG addDecoratorIfNeeded ? Stage is too early or too late for running the decoration. Needs reconstructed tracks. Try again during next stage ?'
         return

   meta_data = getMetaData()
   if len(meta_data) == 0 :
      # decoration has not been ran
      addDecorator()

   # if DBM or GSF tracks need to be monitored schedule addExtraMonitoring as user algorithm, so that
   # the monitoring manager exists already.
   from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
   InDetPhysValFlags.init()

   # from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   # print 'DEBUG addDecoratorIfNeeded add meta data %s.' % svcMgr.TagInfoMgr.ExtraTagValuePairs
