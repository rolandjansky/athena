# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import injectNameArgument,checkKWArgs,_args,serviceFactory

import InDetPhysValMonitoring.InDetPhysValMonitoringConf

class InDetPhysValMonitoringTool(object) :
  '''
  Namespace for inner detector hole search tools
  '''
  def __init__(self) :
     raise('must not be instantiated. Only child classes should be instantiated.')

  class InDetPhysValMonitoringTool(InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValMonitoringTool) :
      '''
      Default InDetPhysValMonitoringTool for InDetTrackParticles
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

          # create the HistogramDefinitionSvc
          # at the moment there can only be one HistogramDefinitionSvc
          from InDetPhysValMonitoring.HistogramDefinitionSvc import HistogramDefinitionSvc
          #self.HistogramDefinitionSvc = 
          serviceFactory(HistogramDefinitionSvc.HistogramDefinitionSvc)  

          from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC
          if isMC() :
              self.TruthParticleContainerName = "TruthParticles"
              self.jetContainerName ='AntiKt4TruthJets'
              from InDetPhysValMonitoring.addTruthJets import addTruthJetsIfNotExising
              addTruthJetsIfNotExising(self.jetContainerName)

          else :
              # disable truth monitoring for data
              self.TruthParticleContainerName = ''
              # the jet container is actually meant to be a truth jet container
              self.jetContainerName =''


          # hack to remove example phyval monitor
          from RecExConfig.AutoConfiguration import IsInInputFile
          if not IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets') :
            from InDetPhysValDecoration import findMonMan
            mon_index = findMonMan()
            if mon_index != None :
              import re
              pattern=re.compile('.*PhysValExample')

              from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
              topSequence = AlgSequence()
              mon_manager = topSequence.getChildren()[mon_index]
              del_idx=None
              for tool in mon_manager.AthenaMonTools :
                print 'DEBUG %s AthenaMonTools %s' % (mon_manager.getName(),tool.getName())
                if pattern.match(tool.getName()) != None :
                  mon_manager.AthenaMonTools.remove(tool)
                  break


  class InDetPhysValMonitoringToolGSF(InDetPhysValMonitoringTool) :
      '''
      InDetPhysValMonitoringTool for GSF track particles
      '''
      #@injectNameArgument
      #def __new__(cls, *args, **kwargs) :
      #    return InDetPhysValMonitoringTool.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringToolGSF,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the GSF InDetPhysValMonitoringTool
          self.TrackParticleContainerName = 'GSFTrackParticles'
          self.SubFolder='GSF/'


  class InDetPhysValMonitoringToolDBM(InDetPhysValMonitoringTool) :
      '''
      InDetPhysValMonitoringTool for DBM track particles
      '''
      #@injectNameArgument
      #def __new__(cls, *args, **kwargs) :
      #    return InDetPhysValMonitoringTool.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringToolDBM,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the GSF InDetPhysValMonitoringTool
          from InDetRecExample.InDetKeys import InDetKeys
          self.TrackParticleContainerName=InDetKeys.DBMTracks()
          self.SubFolder='DBM/'
