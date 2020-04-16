# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from builtins import str
def HLTResultMTMakerCfg(name="HLTResultMTMaker"):
   from TrigOutputHandling.TrigOutputHandlingConf import HLTResultMTMaker
   from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

   m = HLTResultMTMaker(name)

   # ROBs/SubDets which are enabled but not necessarily part of the ROS-ROB map
   from libpyeformat_helper import SourceIdentifier,SubDetector
   subdets = [
      SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI,
      SubDetector.TDAQ_CALO_JET_PROC_ROI,
      SubDetector.TDAQ_HLT,
      SubDetector.TDAQ_FTK,
      SubDetector.TDAQ_CALO_TOPO_PROC,
      SubDetector.TDAQ_CALO_DIGITAL_PROC,
      SubDetector.TDAQ_CALO_FEAT_EXTRACT_ROI,
   ]
   m.ExtraEnabledSubDets = []
   for subdetId in subdets:
      m.ExtraEnabledSubDets.append( int(subdetId) )

   def addROBs(dest,subdet,modules):
      for moduleId in modules:
         dest.append(SourceIdentifier(subdet,moduleId).code())

   m.ExtraEnabledROBs = []
   addROBs(m.ExtraEnabledROBs, SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI, [0xa8, 0xa9, 0xaa, 0xab])
   addROBs(m.ExtraEnabledROBs, SubDetector.TDAQ_CALO_JET_PROC_ROI,     [0xac, 0xad])
   addROBs(m.ExtraEnabledROBs, SubDetector.TDAQ_MUON_CTP_INTERFACE,    [0x01])
   addROBs(m.ExtraEnabledROBs, SubDetector.TDAQ_CTP,                   [0x01])
   addROBs(m.ExtraEnabledROBs, SubDetector.TDAQ_CALO_TOPO_PROC,        [0x81, 0x91, 0x82, 0x92])

   # Configure HLT result monitoring histograms
   m.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
   m.MonTool.Histograms = [ defineHistogram( 'TIME_build', path='EXPERT', type='TH1F', title='Time of result making;Time [us];Events',
                                             xbins=100, xmin=0, xmax=1000 ),
                            defineHistogram( 'nstreams', path='EXPERT', type='TH1F', title='Number of streams;Number of streams;Events',
                                             xbins=60, xmin=0, xmax=60 ),
                            defineHistogram( 'nfrags', path='EXPERT', type='TH1F', title='Number of HLT results (module IDs);Number of results;Events',
                                             xbins=10, xmin=0, xmax=10 ),
                            defineHistogram( 'sizeMain', path='EXPERT', type='TH1F', title='Main (physics) HLT result size;Size [kB];Events',
                                             xbins=101, xmin=-10, xmax=1000 ) ]
   
   
   return m

def TriggerEDMSerialiserToolCfg(name="TriggerEDMSerialiserTool"):
   from TrigEDMConfig.DataScoutingInfo import getFullHLTResultID

   # Configuration helper methods
   def addCollection(self, typeNameAux, moduleIds):
      self.CollectionsToSerialize[typeNameAux] = moduleIds

   def addCollectionToMainResult(self, typeNameAux):
      self.addCollection(typeNameAux,moduleIds=[getFullHLTResultID()])

   def addCollectionListToResults(self, typeNameAuxList, moduleIds):
      for typeNameAux in typeNameAuxList:
         self.addCollection(typeNameAux, moduleIds)

   def addCollectionListToMainResult(self, typeNameAuxList):
      self.addCollectionListToResults(typeNameAuxList,moduleIds=[getFullHLTResultID()])

   # Add the helper methods to the TriggerEDMSerialiserTool python class
   from .TrigOutputHandlingConf import TriggerEDMSerialiserTool
   TriggerEDMSerialiserTool.addCollection = addCollection
   TriggerEDMSerialiserTool.addCollectionToMainResult = addCollectionToMainResult
   TriggerEDMSerialiserTool.addCollectionListToResults = addCollectionListToResults
   TriggerEDMSerialiserTool.addCollectionListToMainResult = addCollectionListToMainResult

   # Create and return a serialiser tool object
   serialiser = TriggerEDMSerialiserTool(name)
   from collections import OrderedDict
   class OD(OrderedDict):
      """Purpose of this class is to present map (ordered by insertion order) interface on python side, 
      whereas the property to look like vector of such strings
      "type#key;id0,id1"
      when it gets to setting the serialiser property
      """
      def __repr__(self):
         return '[' +','.join( ['"'+str(typekey)+';'+','.join([str(_) for _ in ids] )+'"'  for typekey,ids in self.items()] ) + ']'
      def __str__(self):
         return self.__repr__()

   serialiser.CollectionsToSerialize = OD()

   from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
   from TrigEDMConfig.TriggerEDMRun3 import tpMap
   tpTool = TrigSerTPTool()
   tpTool.TPMap = tpMap()
   serialiser.TPTool = tpTool

   from TrigEDMConfig.DataScoutingInfo import TruncationThresholds as truncThresholds
   serialiser.TruncationThresholds = truncThresholds

   # Configure monitoring histograms
   from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
   serialiser.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
   serialiser.MonTool.defineHistogram('Truncation_ModuleId', path='EXPERT', type='TH1F',
                                      title='Module IDs of truncated HLT results;Module ID;Num of truncated results',
                                      xbins=20, xmin=0, xmax=20)
   serialiser.MonTool.defineHistogram('Truncation_TotalSize', path='EXPERT', type='TH1F',
                                      title='Total size of truncated HLT result;Size [kB];Num of truncated results',
                                      xbins=200, xmin=0, xmax=5000)
   serialiser.MonTool.defineHistogram('Truncation_LargestName', path='EXPERT', type='TH1F',
                                      title='Name of the largest collection;;Num of truncated results',
                                      xbins=1, xmin=0, xmax=1)
   serialiser.MonTool.defineHistogram('Truncation_LargestSize', path='EXPERT', type='TH1F',
                                      title='Size of the largest collection;Size [kB];Num of truncated results',
                                      xbins=200, xmin=0, xmax=5000)

   return serialiser

def StreamTagMakerToolCfg(name="StreamTagMakerTool"):
   from .TrigOutputHandlingConf import StreamTagMakerTool

   stmaker = StreamTagMakerTool(name)
   # Extra configuration may come here

   return stmaker


def TriggerBitsMakerToolCfg(name="TriggerBitsMakerTool"):
   from .TrigOutputHandlingConf import TriggerBitsMakerTool
   from TriggerJobOpts.TriggerFlags import TriggerFlags

   bitsmaker = TriggerBitsMakerTool(name)
   # Extra configuration may come here

   return bitsmaker
