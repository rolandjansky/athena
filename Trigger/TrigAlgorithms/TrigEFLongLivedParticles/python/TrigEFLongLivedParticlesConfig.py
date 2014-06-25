# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEFLongLivedParticles.TrigEFLongLivedParticlesConf import TrigLoFRemoval
from AthenaCommon.Constants import INFO,DEBUG
from AthenaCommon.SystemOfUnits import GeV


class TrigLoFRemovalConfig(TrigLoFRemoval):
   __slots__ = []
   def __init__ (self, name="TrigLoFRemovalConfig"):
        super(TrigLoFRemovalConfig, self).__init__(name)

        from TrigEFLongLivedParticles.TrigEFLongLivedParticlesMonitoring import TrigLoFRemovalValidationMonitoring, TrigLoFRemovalOnlineMonitoring, TrigLoFRemovalCosmicMonitoring
        validation = TrigLoFRemovalValidationMonitoring()
        online     = TrigLoFRemovalOnlineMonitoring()
        cosmic     = TrigLoFRemovalCosmicMonitoring()


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigLoFRemoval_Time")

        self.AthenaMonTools = [ validation, online, time, cosmic]

        self.OutputLevel=INFO
