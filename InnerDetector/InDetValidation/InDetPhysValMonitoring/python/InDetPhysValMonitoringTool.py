# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
from __future__ import print_function

from InDetPhysValMonitoring.ConfigUtils import serviceFactory,toolFactory
from InDetRecExample.TrackingCommon import setDefaults

import InDetPhysValMonitoring.InDetPhysValMonitoringConf

def removePhysValExample() :
   print ('DEBUG no AntiKt4EMTopoJets in input file.')
   from InDetPhysValMonitoring.InDetPhysValDecoration import findMonMan
   mon_index = findMonMan()
   if mon_index is not None :
     import re
     pattern=re.compile('.*PhysValExample')

     from AthenaCommon.AlgSequence import AlgSequence
     topSequence = AlgSequence()
     mon_manager = topSequence.getChildren()[mon_index]
     print ('DEBUG Found  mon_manager %s with the following tools: %s ' % (mon_manager.getName(), mon_manager.AthenaMonTools.toStringProperty()))
     for idx in range(0,len(mon_manager.AthenaMonTools)) :
       tool_name = mon_manager.AthenaMonTools[idx].getName()
       # print ('DEBUG %s AthenaMonTools %s' % (mon_manager.getName(),tool_name))
       if pattern.match(tool_name) is not None :
         print ('DEBUG removing %s from %s .' % (tool_name, mon_manager.getName()))
         del mon_manager.AthenaMonTools[idx]
         break
   else :
       for child in topSequence.getChildren() :
           print ('DEBUG top sequence has %s' % (child.getName()))

def getInDetPhysValMonitoringTool(**kwargs) :
   kwargs=setDefaults(kwargs,
                      useTrackSelection = False,
                      EnableLumi        = False)

   # create the HistogramDefinitionSvc
   # at the moment there can only be one HistogramDefinitionSvc
   from InDetPhysValMonitoring.HistogramDefinitionSvc import getHistogramDefinitionSvc
   #self.HistogramDefinitionSvc =
   serviceFactory(getHistogramDefinitionSvc)

   from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC,InDetPhysValFlags
   if isMC() :
      from InDetPhysValMonitoring.InDetPhysValDecoration import getInDetRttTruthSelectionTool
      kwargs=setDefaults(kwargs, TruthParticleContainerName = "TruthParticles")
      if 'TruthSelectionTool' not in kwargs :
         kwargs=setDefaults(kwargs, TruthSelectionTool = getInDetRttTruthSelectionTool() )
      if InDetPhysValFlags.doValidateTracksInJets() :
         jets_name='AntiKt4LCTopoJets'
         kwargs=setDefaults(kwargs,
                            JetContainerName    = jets_name,
                            FillTrackInJetPlots = True)
         from InDetPhysValMonitoring.addTruthJets import addTruthJetsIfNotExising
         addTruthJetsIfNotExising(jets_name)
         if InDetPhysValFlags.doValidateTracksInBJets():
            kwargs=setDefaults(kwargs,
                              FillTrackInBJetPlots = True)

      else :
         kwargs=setDefaults(kwargs,
                            JetContainerName    ='' ,
                            FillTrackInJetPlots = False)
      
      #adding the VeretxTruthMatchingTool
      from InDetTruthVertexValidation.InDetTruthVertexValidationConf import InDetVertexTruthMatchTool
      kwargs=setDefaults(kwargs, 
                        useVertexTruthMatchTool = True,
         		VertexTruthMatchTool = toolFactory(InDetVertexTruthMatchTool) )
      
      #Options for Truth Strategy : Requires full pile-up truth containers for some
      if InDetPhysValFlags.setTruthStrategy() == 'All' or InDetPhysValFlags.setTruthStrategy() == 'PileUp' :
        from RecExConfig.AutoConfiguration import IsInInputFile
        if IsInInputFile('xAOD::TruthPileupEventContainer','TruthPileupEvents') :
            kwargs=setDefaults(kwargs,
                               PileupSwitch = InDetPhysValFlags.setTruthStrategy())
        else :
            print ('WARNING Truth Strategy for InDetPhysValMonitoring set to %s but TruthPileupEvents are missing in the input; resetting to HardScatter only' % (InDetPhysValFlags.setTruthStrategy()))
      elif InDetPhysValFlags.setTruthStrategy() != 'HardScatter' :
        print ('WARNING Truth Strategy for for InDetPhysValMonitoring set to invalid option %s; valid flags are ["HardScatter", "All", "PileUp"]' %  (InDetPhysValFlags.setTruthStrategy()))
         

      


   else :
      # disable truth monitoring for data
      kwargs=setDefaults(kwargs,
                         TruthParticleContainerName = '',
                         TruthVertexContainerName   = '',
                         TruthEvents                = '',
                         TruthPileupEvents          = '',
                         TruthSelectionTool         = '',
                         # the jet container is actually meant to be a truth jet container
                         JetContainerName           ='',
                         FillTrackInJetPlots        = False,
                         FillTrackInBJetPlots       = False)

   # Control the number of output histograms
   if InDetPhysValFlags.doPhysValOutput() :
      kwargs=setDefaults(kwargs,
                         SkillLevel = 100)

   elif InDetPhysValFlags.doExpertOutput() :
      kwargs=setDefaults(kwargs,
                         SkillLevel = 200)

   # hack to remove example physval monitor
   from RecExConfig.AutoConfiguration import IsInInputFile
   if not IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets') :
      add_remover=True
      from RecExConfig.RecFlags import rec
      try:
        for elm in rec.UserExecs :
           if elm.find('removePhysValExample')>0 :
              add_remover=False
              break
      except:
        pass
      if add_remover :
         rec.UserExecs += ['from InDetPhysValMonitoring.InDetPhysValMonitoringTool import removePhysValExample;removePhysValExample();']

   return InDetPhysValMonitoring.InDetPhysValMonitoringConf.InDetPhysValMonitoringTool(**kwargs)


def getInDetPhysValMonitoringToolLoose(**kwargs) :
   if 'TrackSelectionTool' not in kwargs :
      from InDetPhysValMonitoring.TrackSelectionTool import getInDetTrackSelectionToolLoose
      kwargs=setDefaults(kwargs, TrackSelectionTool = toolFactory(getInDetTrackSelectionToolLoose))

   kwargs=setDefaults(kwargs,
                      name              = 'InDetPhysValMonitoringToolLoose',
                      SubFolder         = 'Loose/',
                      useTrackSelection = True)

   return getInDetPhysValMonitoringTool(**kwargs)

def getInDetPhysValMonitoringToolTightPrimary(**kwargs) :
   if 'TrackSelectionTool' not in kwargs :
      from InDetPhysValMonitoring.TrackSelectionTool import getInDetTrackSelectionToolTightPrimary
      kwargs=setDefaults(kwargs, TrackSelectionTool = toolFactory(getInDetTrackSelectionToolTightPrimary))

   kwargs=setDefaults(kwargs,
                      name              = 'InDetPhysValMonitoringToolTightPrimary',
                      SubFolder         = 'TightPrimary/',
                      useTrackSelection = True)

   return getInDetPhysValMonitoringTool(**kwargs)

def getInDetPhysValMonitoringToolGSF(**kwargs) :
   kwargs=setDefaults(kwargs,
                      name                       = 'InDetPhysValMonitoringToolGSF',
                      SubFolder                  = 'GSF/',
                      TrackParticleContainerName = 'GSFTrackParticles',
                      useTrackSelection          = True)

   return getInDetPhysValMonitoringTool(**kwargs)

def getInDetPhysValMonitoringToolDBM(**kwargs) :
   from InDetRecExample.InDetKeys import InDetKeys
   kwargs=setDefaults(kwargs,
                      name                       = 'InDetPhysValMonitoringToolDBM',
                      SubFolder                  = 'DBM/',
                      TrackParticleContainerName = InDetKeys.DBMTracks(),
                      useTrackSelection          = True)

   return getInDetPhysValMonitoringTool(**kwargs)
