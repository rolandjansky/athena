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

def findMonMan() :
  '''
  Find the monitoring manager and return its index in the top sequence or None
  '''

  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

  count=0
  mon_man_index=None
  for child in topSequence.getChildren() :   
     if child.name() == monManName() :
         mon_man_index = count
     count += 1

  return mon_man_index


def getMetaData() :
   '''
   Try to determine from the meta data whether the decoration has been performed already.  
   '''
   from RecExConfig.RecFlags import rec
   if not rec.readRDO():
      from RecExConfig.InputFilePeeker import inputFileSummary
      try:
          return inputFileSummary['metadata']['/TagInfo'][metaDataKey()]
      except Exception :
          pass
   return ''

def setMetaData() :
   '''
   Write the input source on which the InDet decoration algorithm was running to the meta data
   '''

   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from RecExConfig.RecFlags import rec
   str=''
   if rec.readRDO() :
       str='fromRDO'
   elif rec.readESD() :
       str='fromESD'
   elif rec.readESD() :
       str='fromAOD'

   svcMgr.TagInfoMgr.ExtraTagValuePairs += [metaDataKey(), str]


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
          from AthenaCommon.AppMgr import ToolSvc
          super(InDetHoleSearchTool.PhysValMonInDetHoleSearchTool,self).__init__(**_args( kwargs,
                                                                                                 name         = self.__class__.__name__,
                                                                                                 Extrapolator = ToolSvc.InDetExtrapolator,
                                                                                                 usePixel     = True,
                                                                                                 useSCT       = True,
                                                                                                 OutputLevel  = 1,
                                                                                                 CountDeadModulesAfterLastHit = True) )


import InDetPhysValMonitoring.InDetPhysValMonitoringConf
class InDetPhysHitDecoratorTool(object) :
  '''
  Namespace for inner detector hole search tools
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
          self.InDetTrackHoleSearchTool = toolFactory(InDetHoleSearchTool.PhysValMonInDetHoleSearchTool)

# Debugging
# import trace
# tracer = trace.Trace()
# tracer.runfunc( )

def addDecorator() :
  '''
  Add the track particle decoration algorithm to the top sequence. 
  The algorithm is to be run on RAW/RDO since it depends on full hit information
  which is generally not available at later stages. The decorations added by this
  algorithm are used by InDetPhysValMonitoring tool.
  '''
  # Access the algorithm sequence:
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()

  # print ToolSvc
  # import sys
  # sys.exit(1)
  # return _indet_hole_search_tool
  # print InDetHoleSearchTool

  # from AthenaCommon.AppMgr import ToolSvc
  # import trace
  # tracer = trace.Trace()
  # tool = tracer.runfunc(InDetPhysHitDecoratorTool)
  # ToolSvc += tool

  from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg
  _hit_decorator = toolFactory(InDetPhysHitDecoratorTool.InDetPhysHitDecoratorTool)  
  decorators = InDetPhysValDecoratorAlg()
  decorators.InDetPhysHitDecoratorTool = _hit_decorator

  # if there is a monitoring manager add decorator before
  mon_man_index=findMonMan()
  if mon_man_index == None :
     topSequence += decorators
  else :
      print 'DEBUG insert decorator at position %i' % mon_man_index
      topSequence.insert(mon_man_index,decorators)

  setMetaData()


def addDecoratorIfNeeded() :
   '''
   Run the InDet decoration algorithm if it has not been ran yet.
   '''
   meta_data = getMetaData()
   if len(meta_data) == 0 :
      # decoration has not been ran
      addDecorator()




