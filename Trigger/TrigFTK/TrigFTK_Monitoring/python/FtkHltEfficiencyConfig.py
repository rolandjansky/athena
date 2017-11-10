# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: FtkHltEfficiencyConfig.py.py $
#

from TrigFTK_Monitoring.TrigFTK_MonitoringConf import TrigFTK__FtkHltEfficiencyFex

from AthenaCommon.AppMgr import ToolSvc

class FtkHltEfficiencyFex ( TrigFTK__FtkHltEfficiencyFex ) :
    __slots__ = []
    def __init__ (self, name="FtkHltEfficiencyFex"):
        super(FtkHltEfficiencyFex, self).__init__(name)

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigFTK_Monitoring.FtkHltEfficiencyMonitoring import FtkHltEfficiencyMonitoring
        online     = FtkHltEfficiencyMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 100] 

        self.AthenaMonTools = [ time, online ]
        
        # Setup the self algorithm parameters
        # Track matching
        self.DeltaRMax            = 0.05    # float

        # Track Filtering
        self.MinPt              = 2.0     # float
        self.MaxEta             = 10.0    # float
        self.MaxZ0              = 100.0   # float
        self.MaxD0              = 1.0     # float

        self.MaxHLTZ0Err        = 100.0   # float
        self.MaxHLTD0Err        = 100.0   # float
        self.MinHLTSiHits       = 9       # int
        self.MinHLTPixHits      = 0       # int
        self.MinHLTSctHits      = 0       # int
        self.MaxHLTPixHoles     = 100     # int
        self.MaxHLTSctHoles     = 100     # int
        self.MaxHLTReducedChi2  = 100.0   # float

        self.MaxFTKZ0Err        = 100.0   # float
        self.MaxFTKD0Err        = 100.0   # float
        self.MinFTKSiHits       = 9       # int
        self.MinFTKPixHits      = 0       # int
        self.MinFTKSctHits      = 0       # int
        self.MaxFTKPixHoles     = 100     # int
        self.MaxFTKSctHoles     = 100     # int
        self.MaxFTKReducedChi2  = 100.0   # float

        # 2D Eta Phi parameters
        # if adapted, corresponding histograms need be adapted
        self.Numb2DEtaBins     = 4       # int
        self.Min2DEta          = -3      # float
        self.Max2DEta          =  3      # float
        self.Min2DZ0           = -100    # float
        self.Max2DZ0           =  100    # float
