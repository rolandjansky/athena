# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import setDefaults,serviceFactory,toolFactory

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
         jets_name='AntiKt4TruthJets'
         kwargs=setDefaults(kwargs,
                            jetContainerName    = jets_name,
                            FillTrackInJetPlots = True)
         from InDetPhysValMonitoring.addTruthJets import addTruthJetsIfNotExising
         addTruthJetsIfNotExising(jets_name)
      else :
         kwargs=setDefaults(kwargs,
                            jetContainerName    ='' ,
                            FillTrackInJetPlots = False)

   else :
      # disable truth monitoring for data
      kwargs=setDefaults(kwargs,
                         TruthParticleContainerName = '',
                         #                   TruthVertexContainerName   = '',
                         #                   TruthEventKey              = '',
                         #                   TruthPileupEventKey        = '',
                         #                   TruthSelectionTool         = '',
                         # the jet container is actually meant to be a truth jet container
                         jetContainerName           ='',
                         FillTrackInJetPlots        = False)

   # hack to remove example phyval monitor
   from RecExConfig.AutoConfiguration import IsInInputFile
   if not IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets') :
      add_remover=True;
      from RecExConfig.RecFlags import rec
      for elm in rec.UserExecs :
         if elm.find('removePhysValExample')>0 :
            add_remover=False
            break
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
