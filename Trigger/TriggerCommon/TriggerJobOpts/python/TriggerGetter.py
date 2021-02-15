# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.Logging import logging

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.Configured import Configured 

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF

log = logging.getLogger( "TriggerGetter.py" )

# this is to limit messags when running with -s
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/TriggerPythonConfig.py")
excludeTracePattern.append("*/TrigMonitorBase/TrigGenericMonitoringToolConfig.py")
excludeTracePattern.append ("*/TriggerMenu/*")
excludeTracePattern.append("*/TrigSteering/TrigSteeringConfig.py")





class TriggerGetter(Configured):
    #    _output = {"HLT::HLTResult" : ["HLTResult_L2", "HLTResult_EF"] }
    _configured=True
    _done=False

    def __init__(self):
        # tell the Configured machinery that we do not care about the objects already there
        if not TF.doFEX() and TF.doHypo():
            Configured.__init__(self, ignoreExistingDataObject=True)
        else:
            Configured.__init__(self, ignoreExistingDataObject=False)


    def configure(self):
        
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True

        if TF.triggerMenuSetup() == "Physics_pp_v7_primaries":
            # the Run 2 triggger menu Physics_pp_v7_primaries does not exist in json (it should be phased out soon)
            # Need to disable the new menu through the new config flags
            log.info("Setting ConfigFlags.Trigger.readLVL1FromJSON to False because TriggerFlags.triggerMenuSetup == %s", TF.triggerMenuSetup())
            from AthenaConfiguration.AllConfigFlags import ConfigFlags
            ConfigFlags.Trigger.readLVL1FromJSON = False

        # start with print some information what this will do
        log.info("Basic configuration flags RecAlgsFlag.doTrigger: %d   RecFlags.doTrigger: %d TriggerFlags.doTriggerConfigOnly %d", recAlgs.doTrigger(), rec.doTrigger(), TF.doTriggerConfigOnly() )
        log.info("TriggerFlags: doL1Topo: %s, doLVL1: %s, doHLT: %s", TF.doL1Topo(), TF.doLVL1(), TF.doHLT() )

        willGenerateMenu = recAlgs.doTrigger() and (TF.doLVL1() or TF.doHLT()) and not TF.doTriggerConfigOnly()
        willRunTriggerConfigGetter = recAlgs.doTrigger() or rec.doTrigger() or TF.doTriggerConfigOnly()
        willRunLVL1SimulationGetter = recAlgs.doTrigger() and not TF.doTriggerConfigOnly()
        willRunHLTSimulationGetter = willRunLVL1SimulationGetter and TF.doHLT()

        log.info("Will run: %s%s%s%s", "GenerateMenu " if willGenerateMenu else "",
                                       "TriggerConfigGetter " if willRunTriggerConfigGetter else "",
                                       "LVL1SimulationGetter " if willRunLVL1SimulationGetter else "",
                                       "HLTSimulationGetter " if willRunHLTSimulationGetter else "" )

        log.info("Will not run: %s%s%s%s", "GenerateMenu " if not willGenerateMenu else "",
                                           "TriggerConfigGetter " if not willRunTriggerConfigGetter else "",
                                           "LVL1SimulationGetter " if not willRunLVL1SimulationGetter else "",
                                           "HLTSimulationGetter " if not willRunHLTSimulationGetter else "" )
        if recAlgs.doTrigger():
            from TriggerMenu.menu.GenerateMenu import GenerateMenu

            if ((TF.doLVL1() or TF.doHLT()) and not TF.doTriggerConfigOnly()):
                log.info("generating menu")
                # trigger menu files generation
                g = GenerateMenu()
                g.generate()

                # after the menu xml file has been created or the TriggerDB access is configured,
                # the COOL/SQlite db can be written
                # TB this needs to be optimized -- we do not need ti always but only when AOD or ESD are created
                


        if recAlgs.doTrigger() or rec.doTrigger() or TF.doTriggerConfigOnly():
            # setup configuration services
            from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
            cfg = TriggerConfigGetter()  # noqa: F841

            from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
            theConfCOOLWriter.writeConf2COOL()


        # preconfigure TrigDecisionTool
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
        from PyUtils.MetaReaderPeekerFull import metadata
        if "metadata_items" in metadata and any(('TriggerMenu' in key) for key in metadata["metadata_items"].keys()):
            # Use xAOD configuration. 
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            if not hasattr(svcMgr, 'xAODConfigSvc'):
                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
                svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')
            ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
            ToolSvc.TrigDecisionTool.TrigConfigSvc = svcMgr.xAODConfigSvc
        else:
            # Use TrigConfigSvc
            ToolSvc.TrigDecisionTool.TrigConfigSvc = "TrigConf::TrigConfigSvc/TrigConfigSvc"

        from TrigEDMConfig.TriggerEDM import EDMLibraries
        ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]


            
        # actuall trigger simulation running
        if recAlgs.doTrigger() and not TF.doTriggerConfigOnly():
            # setup Lvl1
            # initialize LVL1ConfigSvc
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
            lvl1 = Lvl1SimulationGetter()  # noqa: F841
            

            if TF.doTransientByteStream() or (jobproperties.Global.InputFormat() != 'bytestream' and TF.doHLT()):
                # Transient BS construction and intialization
                from ByteStreamCnvSvc import WriteByteStream
                StreamBS = WriteByteStream.getStream("Transient","StreamBS")
                StreamBS.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
                StreamBS.ItemList += [ "TRT_RDO_Container#TRT_RDOs" ]
                StreamBS.ItemList += [ "SCT_RDO_Container#SCT_RDOs" ]
                StreamBS.ItemList += [ "PixelRDO_Container#PixelRDOs" ]
                # StreamBS.ItemList +=["LArRawChannelContainer#*"]
                StreamBS.ItemList +=["2721#*"]
                # StreamBS.ItemList +=["TileRawChannelContainer#*"]
                StreamBS.ItemList +=["2927#*"]
                StreamBS.ItemList +=["2934#*"] # added on request from: Arantxa Ruiz Martinez for TileRODMu
                
                # don't need Muons in transient BS
                # StreamBS.ItemList +=["MdtCsmContainer#*"]
                # StreamBS.ItemList +=["RpcPadContainer#*"]
                # StreamBS.ItemList +=["TgcRdoContainer#*"]
                # StreamBS.ItemList +=["CscRawDataContainer#*"]

                from AthenaCommon.Include import include
                # setup trans BS for the ID
                include ("InDetRecExample/InDetRecCabling.py")


            # setup HLT
            # initialize HLT config svc
            log.info("TriggerFlags: doHLT  %r", TF.doHLT())
            if TF.doHLT():
                log.info("configuring hlt")
                from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
                hlt = HLTSimulationGetter(g)  # noqa: F841
            else:
                from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
                from AthenaCommon.AppMgr import ServiceMgr
                ServiceMgr += RegSelSvcDefault()
                ServiceMgr.RegSelSvc.enablePixel=False
                ServiceMgr.RegSelSvc.enableSCT=False        

        # prepare result making of L1
        from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
        hltouput = Lvl1ResultBuilderGetter()

        # prepare result making of HLT
        if TF.doHLT() or (recAlgs.doTrigger() and TF.readBS()):
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hltouput = HLTTriggerResultGetter()  # noqa: F841
      
        return True
