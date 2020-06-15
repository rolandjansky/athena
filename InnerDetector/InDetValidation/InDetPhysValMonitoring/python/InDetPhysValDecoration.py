# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
          return inputFileSummary['tag_info'][metaDataKey()]
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


from ConfigUtils import setDefaults
from ConfigUtils import dumpConfigurables
from ConfigUtils import toolFactory

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
def getPhysValMonInDetHoleSearchTool(**kwargs) :
    '''
    default inner detector hole search tool
    '''
    from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
    # If InDetSCT_ConditionsSummarySvc instance configured by InDetRecConditionsAccess.py is available, use it.
    # Otherwise, the default SCT_ConditionsSummarySvc instance is used.
    # @TODO find a better to solution to get the correct service for the current job.
    SctSummarySvc = "InDetSCT_ConditionsSummarySvc"
    if not hasattr(ServiceMgr, SctSummarySvc):
        SctSummarySvc = "SCT_ConditionsSummarySvc"
    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs=setDefaults( kwargs,
                        name                         = 'PhysValMonInDetHoleSearchTool',
                        Extrapolator                 = ToolSvc.InDetExtrapolator,
                        SctSummarySvc                = SctSummarySvc,
                        usePixel                     = True,
                        useSCT                       = True,
                        checkBadSCTChip              = InDetFlags.checkDeadElementsOnTrack(),
                        # OutputLevel                = 1,
                        CountDeadModulesAfterLastHit = True)

    return InDet__InDetTrackHoleSearchTool(**kwargs)


def getInDetPhysHitDecoratorTool(**kwargs) :
    kwargs=setDefaults(kwargs,name='InDetPhysHitDecoratorTool')
    if 'InDetTrackHoleSearchTool' not in kwargs :
        kwargs=setDefaults(kwargs,InDetTrackHoleSearchTool = toolFactory(getPhysValMonInDetHoleSearchTool))

    import InDetPhysValMonitoring.InDetPhysValMonitoringConf
    return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysHitDecoratorTool(**kwargs)

def getInDetRttTruthSelectionTool(**kwargs) :
    # should match truth selection of truth decorator
    from AthenaCommon.AppMgr import ToolSvc
    the_name='AthTruthSelectionTool'
    if hasattr(ToolSvc,the_name) :
        return getattr(ToolSvc,the_name)
    from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
    comb_kwargs={}
    comb_kwargs.update(requireStatus1 = True,
                      requireCharged = True,
                      maxBarcode = ( 200*1000 if kwargs.pop("OnlyDressPrimaryTracks",True) else 2**31-1 ),
                      maxProdVertRadius = 110.,
                      maxEta = 2.5,
                      minPt = 400. )
    comb_kwargs.update(kwargs)
    tool=AthTruthSelectionTool(**comb_kwargs)
    ToolSvc += tool
    return tool

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


def getInDetPhysValDecoratorAlg(**kwargs) :
    '''
    Default track particle decoration algorithm.
    This algorithm will decorate the InDetTrackParticles.
    '''
    kwargs=setDefaults(kwargs,
                       name = 'InDetPhysValDecoratorAlg')
    if 'InDetPhysHitDecoratorTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           InDetPhysHitDecoratorTool = toolFactory(getInDetPhysHitDecoratorTool))

    # self.InDetPhysHitDecoratorTool.OutputLevel=1
    from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC
    if not isMC() :
        # disable truth monitoring for data
        kwargs=setDefaults(kwargs, TruthParticleContainerName = '')

    import InDetPhysValMonitoring.InDetPhysValMonitoringConf
    return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValDecoratorAlg(**kwargs)

def getInDetPhysValDecoratorAlgGSF(**kwargs) :
    '''
    Algorithm to decorate GSF track particles.
    '''
    return getInDetPhysValDecoratorAlg( **setDefaults(kwargs,
                                                      name                       = 'InDetPhysValDecoratorAlgGSF',
                                                      TrackParticleContainerName =InDetPhysValKeys.GSFTrackParticles))

def getInDetPhysValDecoratorAlgDBM(**kwargs) :
    '''
    Algorithm to decorate DBM track particles.
    '''
    from InDetRecExample.InDetKeys import InDetKeys
    return getInDetPhysValDecoratorAlg( **setDefaults(kwargs,
                                                      name                       = 'InDetPhysValDecoratorAlgDBM',
                                                      TrackParticleContainerName =InDetKeys.DBMTrackParticles() ))

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
   from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
   if InDetPhysValFlags.doValidateGSFTracks() :
      # print 'DEBUG add addGSFTrackDecoratorAlg'
      decorators=[getInDetPhysValDecoratorAlgGSF()]

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

  decorators.append( getInDetPhysValDecoratorAlg() )

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
    decorators.append( getInDetPhysValDecoratorAlgDBM() )

  from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
  from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
  for col in InDetPhysValFlags.validateExtraTrackCollections() :
      prefix=extractCollectionPrefix(col)
      decorators += [ getInDetPhysValDecoratorAlg(name                       = 'InDetPhysValDecoratorAlg'+prefix,
                                                  TrackParticleContainerName = prefix+"TrackParticles") ]

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
