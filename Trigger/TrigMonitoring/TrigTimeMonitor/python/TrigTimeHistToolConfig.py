# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigTimeMonitor.TrigTimeMonitorConf import TrigTimeHistTool


class TrigTimeHistToolConfig(TrigTimeHistTool):
    __slots__ = []
    def __init__(self, name):
        super(TrigTimeHistToolConfig, self).__init__(name)
    def target(self):
        return ["Time"]
