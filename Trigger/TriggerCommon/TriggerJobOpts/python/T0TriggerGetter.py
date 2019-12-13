# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging

log = logging.getLogger( "T0TriggerGetter.py" )

from RecExConfig.Configured import Configured

def withLVL1():
    return TriggerFlags.dataTakingConditions()=='Lvl1Only' or TriggerFlags.dataTakingConditions()=='FullTrigger'

def withHLT():
    return TriggerFlags.dataTakingConditions()=='HltOnly' or TriggerFlags.dataTakingConditions()=='FullTrigger'

class T0TriggerGetter(Configured):

    _configured=True
    _done=False
    
    def configure(self):
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True

        # setup configuration services
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg =  TriggerConfigGetter()  # noqa: F841

        # after the menu xml file has been created or the TriggerDB access is configured,
        # the COOL/SQlite db can be written 
        from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
        theConfCOOLWriter.writeConf2COOL()

        # preconfigure TrigDecisionTool
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
        from PyUtils.MetaReaderPeekerFull import metadata
        if "metadata_items" in metadata and any(('TriggerMenu' in key) for key in metadata["metadata_items"].keys()):
            # Use xAOD configuration. 
            if not hasattr(ToolSvc, 'xAODConfigTool'):
                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool
                ToolSvc += TrigConf__xAODConfigTool('xAODConfigTool')
            ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
            ToolSvc.TrigDecisionTool.ConfigTool = ToolSvc.xAODConfigTool
            ToolSvc.TrigDecisionTool.TrigConfigSvc = "" # Force use of in-file configuration
        else:
            # Use TrigConfigSvc
            ToolSvc.TrigDecisionTool.TrigConfigSvc = "TrigConf::TrigConfigSvc/TrigConfigSvc"

        from TrigEDMConfig.TriggerEDM import EDMLibraries
        ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]

        if withLVL1():
            # setup Lvl1
            # initialize LVL1ConfigSvc
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
            lvl1 = Lvl1ResultBuilderGetter()  # noqa: F841

        if withHLT():
            # setup HLT
            # initialize HLT config svc
            log.info("configuring hlt")
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hlt = HLTTriggerResultGetter()   # noqa: F841

        #Call the tools to unpack the bytestream
        #bsu=ByteStreamUnpackGetter()
        
        #Call the tool to make the TrigDecision object for ESD or AOD
        #tdg=TrigDecisionGetter()
        
        return True
