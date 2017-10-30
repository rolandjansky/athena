# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo import TrigBphysVertexingConfig

from TrigBphysHypo.TrigBphysHypoConf import TrigBphysElectronCounter

from AthenaCommon.AppMgr import ToolSvc

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigBphysHypo.TrigBphysElectronCounterMonitoring import TrigBphysElectronCounterValidationMonitoring
from TrigBphysHypo.TrigBphysElectronCounterMonitoring import TrigBphysElectronCounterOnlineMonitoring

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


class TrigBphysElectronCounterPy (TrigBphysElectronCounter):
    __slots__ = []
    def __init__(self, name = "MultiTrkFexPy"):
        super( TrigBphysElectronCounterPy, self ).__init__( name )
            
    def setEFElectronThresholds(self, thresholds, defaultThreshold = 4000.) :
        self.ptElectronMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptElectronMin) < self.nEfElectron :
                self.ptElectronMin.append( thr )
        while len(self.ptElectronMin) < self.nEfElectron  :
            self.ptElectronMin.append(defaultThreshold)  # lower pt cut makes no sense at trigger



###################################################################################
class TrigBphysElectronCounter_bNe (TrigBphysElectronCounterPy):
    __slots__ = []
    # lets force name setting, as it needs to match pt cuts
    def __init__(self, name, ptElectronMin, pidKey ):
        super( TrigBphysElectronCounter_bNe, self ).__init__( name )
 
        self.nEfElectron = len(ptElectronMin)
        self.ptElectronMin = []
        for thr in ptElectronMin :
            self.ptElectronMin.append(thr)

        # set up PID - defined in TrigHypothesis/TrigEgammaHypo/python/TrigEgammaPidTools
        from TrigEgammaHypo.TrigEgammaPidTools import (ElectronToolName,BLooseISEMBits)
        if pidKey == None or not pidKey in ElectronToolName.keys() or not pidKey in BLooseISEMBits.keys() :
            log.info(" Setting up bphys electron chain with no electron PID requirement")
            self.ApplyIsEM = False
        else :        
            self.ApplyIsEM = True
            self.egammaElectronCutIDToolName =  ElectronToolName[pidKey]
            self.IsEMrequiredBits = BLooseISEMBits[pidKey]            

        time = TrigTimeHistToolConfig("Time")
        validation = TrigBphysElectronCounterValidationMonitoring()
        online = TrigBphysElectronCounterOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

###################################################################################
class TrigBphysElectronCounter_bBee (TrigBphysElectronCounterPy):
    __slots__ = []
    # lets force name setting, as it needs to match pt cuts
    def __init__(self, name, ptElectronMin, pidKey ,  defaultThreshold = 4650.):
        super( TrigBphysElectronCounter_bBee, self ).__init__( name )
 
        self.nEfElectron = 2
        self.ptElectronMin = []
        self.setEFElectronThresholds( self.ptElectronMin, defaultThreshold )

        # set up PID - defined in TrigHypothesis/TrigEgammaHypo/python/TrigEgammaPidTools
        from TrigEgammaHypo.TrigEgammaPidTools import (ElectronToolName,BLooseISEMBits)
        if pidKey == None or not pidKey in ElectronToolName.keys() or not pidKey in BLooseISEMBits.keys() :
            log.info(" Setting up bphys electron chain with no electron PID requirement")
            self.ApplyIsEM = False
        else :        
            self.ApplyIsEM = True
            self.egammaElectronCutIDToolName =  ElectronToolName[pidKey]
            self.IsEMrequiredBits = BLooseISEMBits[pidKey]            

        time = TrigTimeHistToolConfig("Time")
        validation = TrigBphysElectronCounterValidationMonitoring()
        online = TrigBphysElectronCounterOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

