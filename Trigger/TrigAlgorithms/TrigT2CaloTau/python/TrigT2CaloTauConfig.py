# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------
# T2CaloTau Calibration Options
#------------------------------------------------

from TrigT2CaloTau.TrigT2CaloTauConf import T2CaloTau, TauAllCaloDRFex

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()

from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=theCaloNoiseTool

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

#Make changes to TauAllCaloDRFex parameters here:

class TauAllCaloDRFexConfig (TauAllCaloDRFex):
   __slots__ = []
   def __init__ (self, name="TauAllCaloDRFexConfig",tdRNar=0.1,tdRMed=0.2,tdRWid=0.4,tdefWidth=2):
       super(TauAllCaloDRFexConfig, self).__init__(name)
       # here put your customizations
       self.CaloNoiseTool  = theCaloNoiseTool
       self.applyNoiseCut  = True
       self.noiseNSigmaCut = 2.
       self.hecQualityCut  = 0              
       self.dRSeed         = 0.15
       self.StripEthr      = 200.
       self.defaultWidth   = tdefWidth #Sets which width size is saved for EMEnergy (0:Narrow,1:Medium,2:Wide)
       self.dRConeNarrow   = tdRNar
       self.dRConeMedium   = tdRMed
       self.dRConeWide     = tdRWid


## configurable class
class T2CaloTau_Tau_custom (T2CaloTau):
   __slots__ = []
   def __init__ (self, name="T2CaloTau_Tau_custom"): 
       super(T2CaloTau_Tau_custom, self).__init__(name)
       self.EtaWidth = 0.4
       self.PhiWidth = 0.4
       self.EtaWidthForID = 0.3
       self.PhiWidthForID = 0.3
     
       tauAllCaloDRFex = TauAllCaloDRFexConfig()

       self.IAlgToolList=[tauAllCaloDRFex]
       self.TimerNtuple="T2CaloTau.T2CaTautTot"
       self.TrigTauClusterKey = "T2CaloTrigTauCluster"

#       monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
       from TrigT2CaloTau.TrigT2CaloTauMonitoring import T2CaloTauValidationMonitoring, T2CaloTauOnlineMonitoring, T2CaloTauCosmicMonitoring 
       validation = T2CaloTauValidationMonitoring()        
       online     = T2CaloTauOnlineMonitoring()
       cosmic     = T2CaloTauCosmicMonitoring()

       from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
       time = TrigTimeHistToolConfig("Time")

       self.AthenaMonTools = [ time, validation, online, cosmic ]


## calo monitoring class
class T2CaloTau_cells (T2CaloTau_Tau_custom):
   __slots__ = []
   def __init__ (self, name="T2CaloTau_cells"): 
       super(T2CaloTau_cells, self).__init__(name)
       # here put your customizations
       self.IAlgToolList= [TauAllCaloDRFexConfig('tauAllCaloDRFexCells')]
       # Save cells
       for item in self.IAlgToolList:
         item.SaveCellsInContainer=True
         item.ThresholdKeepCells=-100000.
         item.hecQualityCut=0
         item.CaloNoiseTool=theCaloNoiseTool
         item.applyNoiseCut=False
         item.noiseNSigmaCut=2.

       self.StoreCells=True
       self.EtaWidth = 0.4
       self.PhiWidth = 0.4
       self.TimerNtuple="T2CaloTau.T2CaTautTot"
       self.TrigTauClusterKey = "T2CaloTrigTauCluster"


############### to be imported by the menu ###############

# default class (2011)
class T2CaloTau_Tau (T2CaloTau_Tau_custom):
   __slots__ = []
   #def __init__ (self, name="T2CaloTau_Tau"):
   def __init__ (self, name="T2CaloTau_Tau"):
      T2CaloTau_Tau_custom.__init__(self,name)
      tauAllCaloDRFex = TauAllCaloDRFexConfig(tdRNar=0.1,tdRMed=0.2,tdRWid=0.4,tdefWidth=2) # use Wide (Nor in 2011), cone size 0.4
      self.IAlgToolList=[tauAllCaloDRFex]
      
# class for 2012: uses Medium cone size as default
class T2CaloTau_Tau_Med (T2CaloTau_Tau_custom):
   __slots__ = []
   def __init__ (self, name="T2CaloTau_Tau_Med"):
      T2CaloTau_Tau_custom.__init__(self,name)
      tauAllCaloDRFex = TauAllCaloDRFexConfig(tdRNar=0.1,tdRMed=0.2,tdRWid=0.4,tdefWidth=1) # use Medium cone size (0.2) variables
      self.IAlgToolList=[tauAllCaloDRFex]
