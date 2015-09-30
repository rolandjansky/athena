# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import injectNameArgument,checkKWArgs,_args
import InDetPhysValMonitoring.InDetPhysValMonitoringConf

class InDetPhysValMonitoringTool(object) :
  '''
  Namespace for inner detector hole search tools
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class InDetPhysValMonitoringTool(InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValMonitoringTool) :
      '''
      Default InDetPhysValMonitoringTool 
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          print "DEBUG InDetPhysValMonitoringTool.py InDetPhysValMonitoringTool.InDetPhysValMonitoringTool %s" %kwargs['name']
          return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringTool,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the default InDetPhysValMonitoringTool
          self.useTrackSelection = False
          self.EnableLumi = False
          self.TruthParticleContainerName = "TruthParticles"

  class InDetPhysValMonitoringToolGSF(InDetPhysValMonitoringTool) :
      '''
      InDetPhysValMonitoringTool for GSF track particles
      '''
      @injectNameArgument
      def __new__(cls, *args, **kwargs) :
          return InDetPhysValMonitoringTool.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringToolGSF,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the GSF InDetPhysValMonitoringTool
          self.TrackParticleContainerName = 'GSFTrackParticles'
          self.SubFolder='GSF/'

