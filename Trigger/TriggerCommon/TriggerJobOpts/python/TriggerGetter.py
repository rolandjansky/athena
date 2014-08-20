# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.GlobalFlags import jobproperties
#from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AthenaCommonFlags import jobproperties
from RecExConfig.RecFlags  import jobproperties

from TriggerJobOpts.TriggerFlags import TriggerFlags
TF = TriggerFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging
log = logging.getLogger( "TriggerGetter.py" )


from AthenaCommon.AppMgr import ServiceMgr,ToolSvc


try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
    from TriggerMenuPython.GenerateMenu import GenerateMenu

from RecExConfig.Configured import Configured 

# this is to limit messags when running with -s
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/TriggerPythonConfig.py")
excludeTracePattern.append("*/TrigMonitorBase/TrigGenericMonitoringToolConfig.py")
excludeTracePattern.append ("*/TriggerMenuPython/*")
excludeTracePattern.append("*/TrigSteering/TrigSteeringConfig.py")



class TriggerGetter(Configured):
    #    _output = {"HLT::HLTResult" : ["HLTResult_L2", "HLTResult_EF"] }
    _configured=True
    _done=False

    def __init__(self):
        # tell the Configured machinery that we do not care about the objects already there
        if not TriggerFlags.doFEX() and TriggerFlags.doHypo():
            Configured.__init__(self, ignoreExistingDataObject=True)
        else:
            Configured.__init__(self, ignoreExistingDataObject=False)


    def configure(self):
        
        if self._done:
            log.info("configuration already done, who is calling it again?")
            return True
        self._done=True

        # start with print some information what this will do
        log.info("Basic configuration flags RecAlgsFlag.doTrigger: %d   RecFlags.doTrigger: %d TriggerFlags.doTriggerConfigOnly %d" % (recAlgs.doTrigger(), rec.doTrigger(), TriggerFlags.doTriggerConfigOnly()) )
        log.info("TriggerFlags: doLVL1: %s, doLVL2: %s, doEF: %s, doHLT: %s" % (TriggerFlags.doLVL1(), TriggerFlags.doLVL2(), TriggerFlags.doEF(), TriggerFlags.doHLT() ) )

        willGenerateMenu = recAlgs.doTrigger() and (TF.doLVL1() or TF.doLVL2() or TF.doEF() or TF.doHLT()) and not TF.doTriggerConfigOnly()
        willRunTriggerConfigGetter = recAlgs.doTrigger() or rec.doTrigger() or TF.doTriggerConfigOnly()
        willRunLVL1SimulationGetter = recAlgs.doTrigger() and not TriggerFlags.doTriggerConfigOnly()
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
            if TriggerFlags.readConfigFromTriggerDb():
                return self.configureTriggerFromDB()

        
            if ((TriggerFlags.doLVL1()==True or TriggerFlags.doLVL2()==True or TriggerFlags.doEF()==True or TriggerFlags.doHLT()==True) and TriggerFlags.doTriggerConfigOnly()==False):
                log.info("generating menu")
                # trigger menu files generation
                g = GenerateMenu()
                g.generate()

                # after the menu xml file has been created or the TriggerDB access is configured,
                # the COOL/SQlite db can be written
                # TB this needs to be optimized -- we do not need ti always but only when AOD or ESD are created
                


        if recAlgs.doTrigger() or rec.doTrigger() or TriggerFlags.doTriggerConfigOnly():
            # setup configuration services
            from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
            cfg =  TriggerConfigGetter()

            from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
            theConfCOOLWriter.writeConf2COOL()

        # actuall trigger simulation running
        if recAlgs.doTrigger() and not TriggerFlags.doTriggerConfigOnly():
            # setup Lvl1
            # initialize LVL1ConfigSvc
            log.info("configuring lvl1")
            from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
            lvl1 = Lvl1SimulationGetter()
            

            if jobproperties.Global.InputFormat()  != 'bytestream' and (TriggerFlags.doLVL2==True or TriggerFlags.doEF==True or TriggerFlags.doHLT==True):
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
            log.info("TriggerFlags: doLVL2 %r" % TriggerFlags.doLVL2())
            log.info("TriggerFlags: doEF   %r" % TriggerFlags.doEF())
            log.info("TriggerFlags: doHLT  %r" % TriggerFlags.doHLT())
            if TriggerFlags.doLVL2()==True or TriggerFlags.doEF()==True or TriggerFlags.doHLT()==True:
                log.info("configuring hlt")
                from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
                hlt = HLTSimulationGetter()
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
        if TriggerFlags.doLVL2()==True or TriggerFlags.doEF()==True or TriggerFlags.doHLT() or (recAlgs.doTrigger() and TriggerFlags.readBS()):
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hltouput = HLTTriggerResultGetter()

        # preconfigure TrigDecisionTool
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
	
        from TrigEDMConfig.TriggerEDM import EDMLibraries
        ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]

      
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
        if TriggerFlags.triggerDbConnection.statusOn :
            hltConfOffline.dbType = TriggerFlags.triggerDbConnection()['dbType']
            hltConfOffline.dbHost = TriggerFlags.triggerDbConnection()['dbServer']
            hltConfOffline.dbUser = TriggerFlags.triggerDbConnection()['dbUser']
            hltConfOffline.dbName = TriggerFlags.triggerDbConnection()['dbName']
            hltConfOffline.dbPasswd = TriggerFlags.triggerDbConnection()['dbPasswd']
        else:
            # try to get connection parameters from authentication files
            if not hltConfOffline.setDbConnectionFromAuthFile() :
                log.error('failed to set HLTConfOffline service')
                return False
            
        if TriggerFlags.triggerDbKeys.statusOn :
            hltConfOffline.SMKey = TriggerFlags.triggerDbKeys()[0]
            hltConfOffline.LVL1PrescaleKey = TriggerFlags.triggerDbKeys()[1]
            hltConfOffline.HLTPrescaleKey = TriggerFlags.triggerDbKeys()[2]
        else:
            log.error( 'missing DB keys, set the TriggerFlags.triggerDBKeys flag')
            return False
      
        if TriggerFlags.doLVL2() and TriggerFlags.doEF() :
            hltConfOffline.Level = 'BOTH'
        elif TriggerFlags.doLVL2() :
            hltConfOffline.Level = 'L2'
        elif TriggerFlags.doEF() :
            hltConfOffline.Level = 'EF'
        elif TriggerFlags.doHLT() :
            hltConfOffline.Level = 'HLT'
        else:
            hltConfOffline.Level = None
            log.error( 'no trigger level set')
            return False
          
        # Load the setup and set the services on this place
        hltConfOffline.load()
              
        return True


