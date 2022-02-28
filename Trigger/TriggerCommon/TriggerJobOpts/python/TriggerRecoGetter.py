# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging
log = logging.getLogger( "TriggerRecoGetter.py" )

class TriggerRecoGetter(Configured):

    _configured=True
    _done=False
    
    def configure(self):
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True

        # setup configuration services
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter()  # noqa: F841

        # configure TrigDecisionTool
        from AthenaCommon.Configurable import ConfigurableRun3Behavior
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        tdtAcc = None
        with ConfigurableRun3Behavior():
            from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
            tdtAcc = TrigDecisionToolCfg(ConfigFlags)

        from AthenaConfiguration.ComponentAccumulator import appendCAtoAthena
        appendCAtoAthena(tdtAcc)

        if 'L1' in ConfigFlags.Trigger.availableRecoMetadata:
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
            lvl1 = Lvl1ResultBuilderGetter()  # noqa: F841

        if 'HLT' in ConfigFlags.Trigger.availableRecoMetadata:
            log.info("configuring hlt")
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hlt = HLTTriggerResultGetter()   # noqa: F841

        #Call the tools to unpack the bytestream
        #bsu=ByteStreamUnpackGetter()
        
        #Call the tool to make the TrigDecision object for ESD or AOD
        #tdg=TrigDecisionGetter()
        
        return True
