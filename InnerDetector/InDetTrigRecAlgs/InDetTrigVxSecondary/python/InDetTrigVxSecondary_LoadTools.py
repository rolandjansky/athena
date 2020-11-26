# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from InDetTrigVxSecondary.InDetTrigVxSecondaryConf import InDet__TrigVxSecondary
from InDetTrigVxSecondary.InDetTrigVxSecondaryConf import InDet__TrigVxSecondaryCombo

class TrigVxSecondary_EF( InDet__TrigVxSecondary ):
   __slots__ = []
   def __init__(self, name="TrigVxSecondary_Bjet_EF", type="bjet"):
      super( InDet__TrigVxSecondary, self ).__init__( name )
      
      from AthenaCommon.AppMgr import ToolSvc

      # Start of offline vertex finder tool configuration
      
      from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
      InDetVKalVxInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetEFVKalVxInJetTool",
                                                         getNegativeTail  = False, # True for SV0 ; False for SV1
                                                         CutChi2          = 999999, #remove this cut as in Fex, default was 3   
                                                         CutSctHits       = 0,
                                                         CutPixelHits     = 2,
                                                         CutSiHits        = 7, #was 4
                                                         CutBLayHits      = 1
                                                         #OutputLevel      = DEBUG
                                                         )
      #Run on/off monitoring
      InDetVKalVxInJetTool.FillHist=False
      
      ToolSvc += InDetVKalVxInJetTool
      self.SecVtxFinderList+= [ InDetVKalVxInJetTool ]
      
      # End of offline vertex finder tool configuration
      
      self.sortSecVxContainer   = True  # Sorting of vertices in order of increasing chi2/NDoF provided in case it is needed
      self.T2PrmVtxAtEF         = True
      self.T2PrmVtxAtEFAlgoId   = 11    # SiTrack=1, IDScan=2, EFID=11
      self.UseBeamSpotFlag      = False
      
      ## HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3; Unset = -1
      self.UseJetDirection = 1
      
      from InDetTrigVxSecondary.InDetTrigVxSecondaryMonitoring import InDetTrigVxSecondaryValidationMonitoring, InDetTrigVxSecondaryOnlineMonitoring
      validation = InDetTrigVxSecondaryValidationMonitoring()
      online     = InDetTrigVxSecondaryOnlineMonitoring()
      
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      time = TrigTimeHistToolConfig("TimeHistogramForInDetTrigVxSecondary")
      time.TimerHistLimits = [0,20]
      
      self.AthenaMonTools = [ time, validation, online ]

#===============================================================

class TrigVxSecondaryCombo_EF( InDet__TrigVxSecondaryCombo ):
   __slots__ = []
   def __init__(self, name="TrigVxSecondaryCombo_Bjet_EF", type="bjet"):
      super( InDet__TrigVxSecondaryCombo, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc

      # Start of offline vertex finder tool configuration

      from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
      InDetVKalVxInJetTool = InDet__InDetVKalVxInJetTool(name             = "InDetEFVKalVxInJetTool",
                                                         getNegativeTail  = False, # True for SV0 ; False for SV1
                                                         CutChi2          = 999999, #remove this cut as in Fex, default was 3   
                                                         CutSctHits       = 0, 
                                                         CutPixelHits     = 2,
                                                         CutSiHits        = 7, #was 4
                                                         CutBLayHits      = 1
                                                         #OutputLevel      = DEBUG
                                                         )
      #Run on/off monitoring
      InDetVKalVxInJetTool.FillHist=False

      ToolSvc += InDetVKalVxInJetTool
      self.SecVtxFinderList+= [ InDetVKalVxInJetTool ]
      
      # End of offline vertex finder tool configuration
 
      self.sortSecVxContainer   = True  # Sorting of vertices in order of increasing chi2/NDoF provided in case it is needed
      self.T2PrmVtxAtEF         = True
      self.T2PrmVtxAtEFAlgoId   = 11    # SiTrack=1, IDScan=2, EFID=11
      self.UseBeamSpotFlag      = False

      ## HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3; Unset = -1
      self.UseJetDirection = 1
      
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      time = TrigTimeHistToolConfig("TimeHistogramForInDetTrigVxSecondary")
      time.TimerHistLimits = [0,20]

      self.AthenaMonTools = [ time ]

      
#===============================================================
