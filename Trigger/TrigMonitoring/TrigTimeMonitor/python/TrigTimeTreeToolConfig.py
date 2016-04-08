# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigTimeMonitor.TrigTimeMonitorConf import TrigTimeTreeTool

        
class TrigTimeTreeToolConfig(TrigTimeTreeTool):
    __slots__ = []
    def __init__(self, name):
        super(TrigTimeTreeToolConfig, self).__init__(name)
        # this prescale must be carefully set
        self.PreScale=10000
    def target(self):
        return [ "TimeTree" ]

