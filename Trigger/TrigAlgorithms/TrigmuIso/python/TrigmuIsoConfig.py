# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigmuIso.TrigmuIsoConf import *
from TrigmuIso.TrigmuIsoMonitoring import *
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR

# CaloNoiseToolDefault
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool

from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import *
ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()
ToolSvc += MuonBackExtrapolatorForData()

class muIsoConfig (muIso):

   __slots__ = []

   def __new__( cls, *args, **kwargs ):
       newargs = ['%s' % cls.getType()]
       if len(args) == 1:
          newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
       if len(args) == 2:
          newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
       return super( muIsoConfig, cls ).__new__( cls, *newargs, **kwargs )

   def __init__ (self, name,*args,**kwargs): 
       super(muIsoConfig, self).__init__(name)


       # Default backextrapolator is for MC Misaligned Detector
       if ( muonByteStreamFlags.RpcDataType()=='atlas' ):
           self.BackExtrapolatorLUT = MuonBackExtrapolatorForMisalignedDet()
       else:
           self.BackExtrapolatorLUT = MuonBackExtrapolatorForAlignedDet()

       #if globalflags.DataSource != 'data':
       #    self.BackExtrapolatorLUT = MuonBackExtrapolatorForData()

       # muIso Parameters
       self.MuIsoStrategy = 0

       self.MaxDzetaIDMuon = 15.0;
       self.RoutECAL = 0.4;
       self.RoutECALEndCaps = 0.4;
       self.RoutHCAL = 0.4;
       self.RoutHCALEndCaps = 0.4;

       # half width for RoI prepareRob request
       self.EtaWidth = 0.4;
       self.PhiWidth = 0.4;

       self.noiseTool = theCaloNoiseTool
       self.UseAbsNoiseCut = True
       self.ApplyNoiseCut = True
       self.NoiseCutValue = 3.4
       self.ApplyHECQualityCut = True
       self.HECcellQualityThreshold = 0
 
       self.UseECAL = True
       self.UseECALEndCaps = True
       self.UseHCAL = True
       self.UseHCALEndCaps = True
       self.UseID = True

       self.UseExtrapolator = True

       #MC/Data specific configurations
       if globalflags.DataSource != 'data':
           print 'DataSource not data (assuming MC)'

       # ID track collection
       self.IDalgo = "SITRACK"
       try:
           if ( args[1] in ['IDSCAN', 'SITRACK', 'TRTXK', 'TRTSEG', 'STRATEGY_A', 'STRATEGY_B', 'STRATEGY_C', 'STRATEGY_F'] ):
               self.IDalgo=args[1]
       except IndexError:
           pass

       # Conf. specific
       if len(args) == 2:
          if (args[0]=='MSonly'):
            self.UseID = False
       
       self.OutputLevel = INFO

       validation = TrigmuIsoValidationMonitoring()
       online     = TrigmuIsoOnlineMonitoring()
       cosmic     = TrigmuIsoCosmicMonitoring()

       time = TrigTimeHistToolConfig("muIso_Time")
       time.TimerHistLimits = [0,40]

       self.AthenaMonTools = [ validation, online, cosmic, time]

       def setDefaults(cls,handle):
          if hasattr(handle,'BackExtrapolatorLUT'):
             if handle.BackExtrapolatorLUT.name().find("AlignedBackExtrapolator")!=-1:
                print self.name," using BackExtrapolatorLUT for Aligned Detector"
             if handle.BackExtrapolatorLUT.name().find("MisalignedBackExtrapolator")!=-1:
                print self.name," using BackExtrapolatorLUT for Misligned Detector" 
             if handle.BackExtrapolatorLUT.name().find("DataBackExtrapolator")!=-1:
                print self.name," using BackExtrapolatorLUT for Data"
