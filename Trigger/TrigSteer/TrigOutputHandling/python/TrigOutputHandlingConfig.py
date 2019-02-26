# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def HLTResultMTMakerCfg():
   from TrigOutputHandlingConf import HLTResultMTMaker
   from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

   m = HLTResultMTMaker()
   m.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTResultMTMaker')
   m.MonTool.Histograms = [ defineHistogram( 'TIME_build', path='EXPERT', type='TH1F', title='Time of result construction in;[micro seccond]',
                                             xbins=100, xmin=0, xmax=1000 ),
                            defineHistogram( 'nstreams', path='EXPERT', type='TH1F', title='number of streams',
                                             xbins=60, xmin=0, xmax=60 ),
                            defineHistogram( 'nfrags', path='EXPERT', type='TH1F', title='number of HLT results',
                                             xbins=10, xmin=0, xmax=10 ),
                            defineHistogram( 'sizeMain', path='EXPERT', type='TH1F', title='Main (physics) HLT Result size;4B words',
                                             xbins=100, xmin=-1, xmax=999 ) ] # 1000 k span
   return m

def TriggerEDMSerialiserToolCfg(name):
   # Configuration helper methods
   def fullResultID(self):
      return 0

   def addCollection(self, typeNameAux, moduleIds):
      self.CollectionsToSerialize[typeNameAux] = moduleIds

   def addCollectionToMainResult(self, typeNameAux):
      self.addCollection(typeNameAux,moduleIds=[self.fullResultID()])

   def addCollectionListToResults(self, typeNameAuxList, moduleIds):
      for typeNameAux in typeNameAuxList:
         self.addCollection(typeNameAux, moduleIds)

   def addCollectionListToMainResult(self, typeNameAuxList):
      self.addCollectionListToResults(typeNameAuxList,moduleIds=[self.fullResultID()])

   # Add the helper methods to the TriggerEDMSerialiserTool python class
   from TrigOutputHandlingConf import TriggerEDMSerialiserTool
   TriggerEDMSerialiserTool.fullResultID = fullResultID
   TriggerEDMSerialiserTool.addCollection = addCollection
   TriggerEDMSerialiserTool.addCollectionToMainResult = addCollectionToMainResult
   TriggerEDMSerialiserTool.addCollectionListToResults = addCollectionListToResults
   TriggerEDMSerialiserTool.addCollectionListToMainResult = addCollectionListToMainResult

   # Create and return a serialiser tool object
   serialiser = TriggerEDMSerialiserTool(name)
   serialiser.CollectionsToSerialize = {}
   return serialiser
