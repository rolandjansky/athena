# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigT2CaloCosmic.TrigT2CaloCosmicConf import CosmicSamp2Fex
from TrigT2CaloCosmic.TrigT2CaloCosmicConf import CosmicHadEnFex
from TrigT2CaloCosmic.TrigT2CaloCosmicConf import T2CaloCosmic

#from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
#from TrigMonitorBase.TrigGenericMonitoringToolConfig import TrigGenericMonitoringToolConfig

#from AthenaCommon.Constants import VERBOSE,DEBUG

class CosmicHadEnFex1(CosmicHadEnFex):
      __slots__ = []
      def __init__ (self, name="CosmicHadEnFex1"):
        super(CosmicHadEnFex1,self).__init__(name)
        self.THR_1=200

class CosmicHadEnFex2(CosmicHadEnFex):
      __slots__ = []
      def __init__ (self, name="CosmicHadEnFex2"):
        super(CosmicHadEnFex2,self).__init__(name)
        self.THR_1=200

class CosmicSamp2Fex1(CosmicSamp2Fex):
      __slots__ = []
      def __init__ (self, name="CosmicSamp2Fex1"):
        super(CosmicSamp2Fex1,self).__init__(name)
        self.THR_1=420

class CosmicSamp2Fex2(CosmicSamp2Fex):
      __slots__ = []
      def __init__ (self, name="CosmicSamp2Fex2"):
        super(CosmicSamp2Fex2,self).__init__(name)
        self.THR_1=420


class T2CaloCosmic_Cosmic (T2CaloCosmic):
   __slots__ = []
   def __init__ (self, name="T2CaloCosmic_Cosmic"): 
       super(T2CaloCosmic_Cosmic, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [CosmicHadEnFex1(),
                           CosmicHadEnFex2(),
                           CosmicSamp2Fex1(),
                           CosmicSamp2Fex2()]
       #self.EtaMin=[0.2,0,0.2,-1.0]
       #self.EtaMax=[ 0.6, 1.0, 0.6, 0]
       self.EtaMin=[1.0, 0.1, 0.1, -0.1]
       self.EtaMax=[ 2.4, 2.4, 2.4, 2.4 ]
       self.PhiMin=[1.3, -1.8, 1.3, -1.8]
       self.PhiMax=[1.8, -1.3, 1.8, -1.3]
       self.EtaWidth = 0.5
       self.PhiWidth = 0.5
       self.TimerNtuple="T2CaloCosmic.T2CaEgtTot"
       self.TrigEMClusterKey="T2CaloCosmic"
#       self.Monitoring=False
#       self.OutputLevel = DEBUG
#       self.AthenaMonTools = [ TrigT2CaloCosmicValMonitoring(),
#			 TrigT2CaloCosmicOnMonitoring() ]

class T2CaloCosmic_Cosmic_NoHad (T2CaloCosmic):
   __slots__ = []
   def __init__ (self, name="T2CaloCosmic_Cosmic_NoHad"): 
       super(T2CaloCosmic_Cosmic_NoHad, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [CosmicSamp2Fex()]
       self.EtaWidth = 0.5
       self.PhiWidth = 0.5
       self.TimerNtuple="T2CaloCosmic.T2CaEgtTot"
       self.TrigEMClusterKey="T2CaloTrigEMCluster"
#       self.Monitoring=False
#       self.OutputLevel = DEBUG
#       self.AthenaMonTools = [ TrigT2CaloCosmicValMonitoring(),
#			 TrigT2CaloCosmicOnMonitoring() ]

class T2CaloCosmic_Cosmic_NoEm (T2CaloCosmic):
   __slots__ = []
   def __init__ (self, name="T2CaloCosmic_Cosmic_NoEm"): 
       super(T2CaloCosmic_Cosmic_NoEm, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [CosmicHadEnFex()]
       self.EtaWidth = 0.5
       self.PhiWidth = 0.5
       self.TimerNtuple="T2CaloCosmic.T2CaEgtTot"
       self.TrigEMClusterKey="T2CaloTrigEMCluster"
#       self.Monitoring=False
#       self.OutputLevel = DEBUG
#       self.AthenaMonTools = [ TrigT2CaloCosmicValMonitoring(),
#			 TrigT2CaloCosmicOnMonitoring() ]

