# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import injectNameArgument,checkKWArgs,_args,serviceFactory,toolFactory

import InDetPhysValMonitoring.InDetPhysValMonitoringConf

def removePhysValExample() :
   print 'DEBUG no AntiKt4EMTopoJets in input file.'
   from InDetPhysValDecoration import findMonMan
   mon_index = findMonMan()
   if mon_index != None :
     import re
     pattern=re.compile('.*PhysValExample')

     from AthenaCommon.AlgSequence import AlgSequence,AthSequencer 
     topSequence = AlgSequence()
     mon_manager = topSequence.getChildren()[mon_index]
     print 'DEBUG Found  mon_manager %s with the following tools: %s ' % (mon_manager.getName(), mon_manager.AthenaMonTools.toStringProperty())
     for idx in range(0,len(mon_manager.AthenaMonTools)) :
       tool_name = mon_manager.AthenaMonTools[idx].getName()
       # print 'DEBUG %s AthenaMonTools %s' % (mon_manager.getName(),tool_name)
       if pattern.match(tool_name) != None :
         print 'DEBUG removing %s from %s .' % (tool_name, mon_manager.getName())
         del mon_manager.AthenaMonTools[idx]
         break
   else :
       for child in topSequence.getChildren() :
           print 'DEBUG top sequence has %s' % (child.getName())

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

          from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC,InDetPhysValFlags
          if isMC() :
              self.TruthParticleContainerName = "TruthParticles"
              if InDetPhysValFlags.doValidateTracksInJets() :
                 self.jetContainerName ='AntiKt4TruthJets'
                 self.FillTrackInJetPlots = True
                 from InDetPhysValMonitoring.addTruthJets import addTruthJetsIfNotExising
                 addTruthJetsIfNotExising(self.jetContainerName)
              else :
                 self.jetContainerName =''
                 self.FillTrackInJetPlots = False


          else :
              # disable truth monitoring for data
              self.TruthParticleContainerName = ''
              # the jet container is actually meant to be a truth jet container
              self.jetContainerName =''


          # hack to remove example phyval monitor
          from RecExConfig.AutoConfiguration import IsInInputFile
          if not IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets') :
            from RecExConfig.RecFlags import rec
            rec.UserExecs += ['from InDetPhysValMonitoring.InDetPhysValMonitoringTool import removePhysValExample;removePhysValExample();']


  class InDetPhysValMonitoringToolLoose(InDetPhysValMonitoringTool) :
      '''
      InDetPhysValMonitoringTool for track particles which pass the loose selection
      '''
      #@injectNameArgument
      #def __new__(cls, *args, **kwargs) :
      #    return InDetPhysValMonitoringTool.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringToolLoose,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the InDetPhysValMonitoringTool for monitoring loose tracks
          self.SubFolder='Loose/'
          self.useTrackSelection = True

          from InDetPhysValMonitoring.TrackSelectionTool import InDetTrackSelectionTool
          self.TrackSelectionTool = toolFactory(InDetTrackSelectionTool.InDetTrackSelectionToolLoose)


  class InDetPhysValMonitoringToolTightPrimary(InDetPhysValMonitoringTool) :
      '''
      InDetPhysValMonitoringTool for track particles which pass the tight primary selection
      '''
      #@injectNameArgument
      #def __new__(cls, *args, **kwargs) :
      #    return InDetPhysValMonitoringTool.InDetPhysValMonitoringTool.__new__(cls,*args,**kwargs)

      @checkKWArgs
      def __init__(self, **kwargs) :
          super(InDetPhysValMonitoringTool.InDetPhysValMonitoringToolTightPrimary,self)\
                        .__init__(**_args( kwargs,
                                           name = self.__class__.__name__))

          # special parameters of the InDetPhysValMonitoringTool for monitoring tight primary tracks
          self.SubFolder='TightPrimary/'
          self.useTrackSelection = True

          from InDetPhysValMonitoring.TrackSelectionTool import InDetTrackSelectionTool
          self.TrackSelectionTool = toolFactory(InDetTrackSelectionTool.InDetTrackSelectionToolTightPrimary)

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
