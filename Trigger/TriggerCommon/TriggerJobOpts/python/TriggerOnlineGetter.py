# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties
from RecExConfig.RecFlags  import jobproperties

from TriggerJobOpts.TriggerFlags import TriggerFlags


from AthenaCommon.Logging import logging  # loads logger

from AthenaCommon.Include import include  # to include old style job options

from AthenaCommon.AppMgr import ServiceMgr
from TriggerMenuPython.GenerateMenu import GenerateMenu


from RecExConfig.Configured import Configured 


class TriggerOnlineGetter(Configured):
    _output = {"HLT::HLTResult" : "HLTResult_L2", "HLT::HLTResult" : "HLTResult_EF" }
    _configured=True
    _done=False
    def configure(self):
        log = logging.getLogger( "TriggerOnlineGetter.py" )
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True
        
        # this is to limit messags when running with -s
        from AthenaCommon.Include import excludeTracePattern
        excludeTracePattern.append("*/TriggerMenuPython/TriggerPythonConfig.py")
        excludeTracePattern.append("*/TrigConfigSvc/TrigConfigSvcConfig.py")

        log.info("generating menu")
        # trigger menu files generation
        g = GenerateMenu()
        g.generate()


        # setup Lvl1
        # initialize LVL1ConfigSvc
        log.info("configuring LVL1")
        # Xin
        from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
        lvl1 = Lvl1SimulationGetter()

        if globalflags.InputFormat()=='pool':
            log.info("setting up transient BS")
            include( "TriggerRelease/jobOfragment_TransBS_Modern.py" )

        # setup HLT
        # initialize HLT config svc
        log.info("configuring hlt")
        from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
        hlt = HLTSimulationGetter()
        
        
        return True


