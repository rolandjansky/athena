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
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from PyUtils.MetaReaderPeekerFull import metadata
        has_trig_metadata = ("metadata_items" in metadata and any(('TriggerMenu' in key) for key in metadata["metadata_items"].keys())) 
        if has_trig_metadata or ConfigFlags.Trigger.EDMVersion == 3: #or ConfigFlags.Trigger.doEDMVersionConversion:
            # has_trig_metadata == True: we are reading a POOL file which contains the trigger payload, use xAOD configuration service. 
            # EDMVersion == 3: We are in a RAWtoALL style job on Run3 data, use xAOD configuration service in UseInFileMetadata=False mode
            # doEDMVersionConversion == True: We are in a RAWtoALL style job on Run1/2 data. But we are converting it to the Run3 format. So can treat this the same as EDMVersion == 3.
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            if not hasattr(svcMgr, 'xAODConfigSvc'):
                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
                svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')

            # For RAWtoESD, RAWtoALL, the data must be read directly from the Conditions or Detector store.
            # This is predicated on there not being any trigger metadata in the input file, and enabled by this flag.
            svcMgr.xAODConfigSvc.UseInFileMetadata = has_trig_metadata
            ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
            ToolSvc.TrigDecisionTool.TrigConfigSvc = svcMgr.xAODConfigSvc
        else:
            # We are in a RAWtoALL style job on Run1/2 data. And !doEDMVersionConversion, so we must fall back on the deprecated service
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
