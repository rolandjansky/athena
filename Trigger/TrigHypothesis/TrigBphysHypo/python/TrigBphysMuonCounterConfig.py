# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo import TrigBphysVertexingConfig

from TrigBphysHypo.TrigBphysHypoConf import TrigBphysMuonCounter

from AthenaCommon.AppMgr import ToolSvc

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigBphysHypo.TrigBphysMuonCounterMonitoring import TrigBphysMuonCounterValidationMonitoring
from TrigBphysHypo.TrigBphysMuonCounterMonitoring import TrigBphysMuonCounterOnlineMonitoring


class TrigBphysMuonCounterPy (TrigBphysMuonCounter):
    __slots__ = []
    def __init__(self, name = "MultiTrkFexPy"):
        super( TrigBphysMuonCounterPy, self ).__init__( name )
            
    def setEFMuonThresholds(self, thresholds) :
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptMuonMin) < self.nEfMuon :
                self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nEfMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger



###################################################################################
class TrigBphysMuonCounter_bNmu (TrigBphysMuonCounterPy):
    __slots__ = []
    # lets force name setting, as it needs to match pt cuts
    def __init__(self, name, ptMuonMin ):
        super( TrigBphysMuonCounter_bNmu, self ).__init__( name )
 
        self.nEfMuon = len(ptMuonMin)
        self.ptMuonMin = []
        for thr in ptMuonMin :
            self.ptMuonMin.append(thr)

        time = TrigTimeHistToolConfig("Time")
        validation = TrigBphysMuonCounterValidationMonitoring()
        online = TrigBphysMuonCounterOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

