# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigT2CaloTileMon.TrigT2CaloTileMonConf import TileMonHadEnFex
from TrigT2CaloTileMon.TrigT2CaloTileMonConf import T2CaloTileMon

#from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
#from TrigMonitorBase.TrigGenericMonitoringToolConfig import TrigGenericMonitoringToolConfig

#from AthenaCommon.Constants import VERBOSE,DEBUG

class TileMonHadEnFex(TileMonHadEnFex):
      __slots__ = []
      def __init__ (self, name="TileMonHadEnFex"):
        super(TileMonHadEnFex,self).__init__(name)
        self.THR_1=100
        self.HistogramsPath="/EXPERT/TileMonHadEnFex"


class T2CaloCosmic_Cosmic (T2CaloTileMon):
   __slots__ = []
   def __init__ (self, name="T2CaloCosmic_Cosmic"): 
       super(T2CaloTileMon, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [TileMonHadEnFex() ]
       self.IAlgToolList[0].HistogramsPath= "/EXPERT/"
       self.IAlgToolList[0].HistogramsPath+=name
       self.doTiming=True

class T2CaloTileMon_TileMon (T2CaloTileMon):
   __slots__ = []
   def __init__ (self, name="T2CaloTileMon_TileMon"): 
       super(T2CaloTileMon, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [TileMonHadEnFex() ]
       self.doTiming=True
