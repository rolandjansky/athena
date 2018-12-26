# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT, TrigMissingETHypoToolMT

from AthenaCommon.SystemOfUnits import GeV


class MissingETHypoAlgMT(TrigMissingETHypoAlgMT):
    __slots__ = []
    def __init__(self, name, hTools=[], metKey=""):
        super( MissingETHypoAlgMT, self ).__init__( name )

        if len(hTools)!=0: 
            self.HypoTools = hTools 
        if metKey!="": 
            self.METContainerKey = metKey 

        # Monitoring code copied from old hypo config file.
        # Kept here in case it may be useful during development
        # Will be removed/re-implemented soon.
        '''
        from TrigMissingETHypo.TrigMissingETHypoMonitoring import TrigEFMissingETHypoValidationMonitoring, TrigEFMissingETHypoCosmicMonitoring
        validation = TrigEFMissingETHypoValidationMonitoring()
        cosmic = TrigEFMissingETHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("METHypo_Time")
        time.TimerHistLimits = [0, 5]

        self.AthenaMonTools = [ time, validation, cosmic ]
        '''

    def onlineMonitoring(self):
        from TrigMissingETHypo.TrigMissingETHypoMonitoringTool import TrigMissingETHypoMonitoringTool
        self.MonTool = TrigMissingETHypoMonitoringTool()

class MissingETHypoToolMT(TrigMissingETHypoToolMT):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( MissingETHypoToolMT, self ).__init__( name )

        # Configure threshold from trigger name
        if 'alg' in kwargs:
            trigParts = name.split('_')
            alg = kwargs['alg']
            if alg=='cell':
                alg = trigParts[-1]
            idx = trigParts.index(alg) 
            self.metThreshold = int(filter(str.isdigit, trigParts[idx-1]))


def TrigMETCellHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='cell')

def TrigMETPufitHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='pufit')

def TrigMETMhtHypoToolFromName(name, conf):
    return MissingETHypoToolMT(name, alg='mht')
