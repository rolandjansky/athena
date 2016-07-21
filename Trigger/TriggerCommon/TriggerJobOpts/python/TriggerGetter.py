# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from AthenaCommon.Logging import logging

from RecExConfig.RecFlags  import jobproperties
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.Configured import Configured 

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
from TriggerMenu.menu.GenerateMenu import GenerateMenu

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

        # start with print some information what this will do
        log.info("Basic configuration flags RecAlgsFlag.doTrigger: %d   RecFlags.doTrigger: %d TriggerFlags.doTriggerConfigOnly %d" % (recAlgs.doTrigger(), rec.doTrigger(), TF.doTriggerConfigOnly()) )
        log.info("TriggerFlags: doL1Topo: %s, doLVL1: %s, doLVL2: %s, doEF: %s, doHLT: %s" % (TF.doL1Topo(), TF.doLVL1(), TF.doLVL2(), TF.doEF(), TF.doHLT() ) )

        willGenerateMenu = recAlgs.doTrigger() and (TF.doLVL1() or TF.doLVL2() or TF.doEF() or TF.doHLT()) and not TF.doTriggerConfigOnly()
        willRunTriggerConfigGetter = recAlgs.doTrigger() or rec.doTrigger() or TF.doTriggerConfigOnly()
        willRunLVL1SimulationGetter = recAlgs.doTrigger() and not TF.doTriggerConfigOnly()
        willRunHLTSimulationGetter = willRunLVL1SimulationGetter and (TF.doLVL2() or TF.doEF() or TF.doHLT())

        log.info("Will run: %s%s%s%s" % ("GenerateMenu " if willGenerateMenu else "",
                                         "TriggerConfigGetter " if willRunTriggerConfigGetter else "",
                                         "LVL1SimulationGetter " if willRunLVL1SimulationGetter else "",
                                         "HLTSimulationGetter " if willRunHLTSimulationGetter else "",
                                         ) )
        log.info("Will not run: %s%s%s%s" % ("GenerateMenu " if not willGenerateMenu else "",
                                             "TriggerConfigGetter " if not willRunTriggerConfigGetter else "",
                                             "LVL1SimulationGetter " if not willRunLVL1SimulationGetter else "",
                                             "HLTSimulationGetter " if not willRunHLTSimulationGetter else "",
                                         ) )

        if recAlgs.doTrigger():

            # setup the trigger from the DB
            if TF.readConfigFromTriggerDb():
                return self.configureTriggerFromDB()

        
            if ((TF.doLVL1()==True or TF.doLVL2()==True or TF.doEF()==True or TF.doHLT()==True) and TF.doTriggerConfigOnly()==False):
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
            cfg =  TriggerConfigGetter()

            from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
            theConfCOOLWriter.writeConf2COOL()


        # preconfigure TrigDecisionTool
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
        # tell tdt to use TrigConfigSvc (Since TrigDecisionTool-00-03-40, defaults to not use it) 
        ToolSvc.TrigDecisionTool.TrigConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"

        from TrigEDMConfig.TriggerEDM import EDMLibraries
        ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]


            
        # actuall trigger simulation running
        if recAlgs.doTrigger() and not TF.doTriggerConfigOnly():
            # setup Lvl1
            # initialize LVL1ConfigSvc
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
            lvl1 = Lvl1SimulationGetter()
            

            if jobproperties.Global.InputFormat()  != 'bytestream' and (TF.doLVL2==True or TF.doEF==True or TF.doHLT==True):
                # Transient BS construction and intialization
                from ByteStreamCnvSvc import WriteByteStream
                StreamBS = WriteByteStream.getStream("Transient","StreamBS")
                StreamBS.ForceRead=True
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
            log.info("TriggerFlags: doLVL2 %r" % TF.doLVL2())
            log.info("TriggerFlags: doEF   %r" % TF.doEF())
            log.info("TriggerFlags: doHLT  %r" % TF.doHLT())
            if TF.doLVL2()==True or TF.doEF()==True or TF.doHLT()==True:
                log.info("configuring hlt")
                from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
                hlt = HLTSimulationGetter(g)
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
        if TF.doLVL2()==True or TF.doEF()==True or TF.doHLT() or (recAlgs.doTrigger() and TF.readBS()):
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hltouput = HLTTriggerResultGetter()
      
        return True

    def configureTriggerFromDB(self):
        """ configures trigger from the DB """
        log = logging.getLogger( "TriggerGetter.py")
        log.info("configureTriggerFromDb")
        from TrigConfOffline.HLTConfOffline import HLTConfOffline
        hltConfOffline = HLTConfOffline()
        # Set the properties
        hltConfOffline.setupSource = 'db'
        hltConfOffline.OutputLevel = 1
        # Set the connection to the DB
        if TF.triggerDbConnection.statusOn :
            hltConfOffline.dbType = TF.triggerDbConnection()['dbType']
            hltConfOffline.dbHost = TF.triggerDbConnection()['dbServer']
            hltConfOffline.dbUser = TF.triggerDbConnection()['dbUser']
            hltConfOffline.dbName = TF.triggerDbConnection()['dbName']
            hltConfOffline.dbPasswd = TF.triggerDbConnection()['dbPasswd']
        else:
            # try to get connection parameters from authentication files
            if not hltConfOffline.setDbConnectionFromAuthFile() :
                log.error('failed to set HLTConfOffline service')
                return False
            
        if TF.triggerDbKeys.statusOn :
            hltConfOffline.SMKey = TF.triggerDbKeys()[0]
            hltConfOffline.LVL1PrescaleKey = TF.triggerDbKeys()[1]
            hltConfOffline.HLTPrescaleKey = TF.triggerDbKeys()[2]
        else:
            log.error( 'missing DB keys, set the TriggerFlags.triggerDBKeys flag')
            return False
      
        if TF.doLVL2() and TF.doEF() :
            hltConfOffline.Level = 'BOTH'
        elif TF.doLVL2() :
            hltConfOffline.Level = 'L2'
        elif TF.doEF() :
            hltConfOffline.Level = 'EF'
        elif TF.doHLT() :
            hltConfOffline.Level = 'HLT'
        else:
            hltConfOffline.Level = None
            log.error( 'no trigger level set')
            return False
          
        # Load the setup and set the services on this place
        hltConfOffline.load()
              
        return True

