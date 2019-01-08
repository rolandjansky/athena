
include.block ( "EventOverlayJobTransforms/Level1Overlay_jobOptions.py" )

from AthenaCommon import CfgGetter
from AthenaCommon.DetFlags import DetFlags
from Digitization.DigitizationFlags import jobproperties
from OverlayCommonAlgs.OverlayFlags import overlayFlags

if DetFlags.overlay.LVL1_on():

    if overlayFlags.doBkg==True:
       from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects
       job += DeepCopyObjects("BkgRdo5")
       job.BkgRdo5.Level1Objects = True
          
    if readBS and isRealData:
       include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
       #include ("MuonCommRecExample/ReadMuCTPI_jobOptions.py")

       ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [                         
          "ROIB::RoIBResult/RoIBResult",
          "MuCTPI_RDO/MUCTPI_RDO",        
          "L2Result/L2Result",
          "CTP_RDO/CTP_RDO",
          "ROIB::RecRoIBResult/RecRoIBResult",
          "MuCTPI_RIO/MUCTPI_RIO",
          "CTP_RIO/CTP_RIO"
       ] 

    if DetFlags.simulateLVL1.LAr_on():
        include( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )
        # Noise 
        if not jobproperties.Digitization.doCaloNoise.get_Value():
            job.LArTTL1Maker.NoiseOnOff= False #(default:True) 
        # PileUp
        job.LArTTL1Maker.PileUp = True
        # If we are doing MC overlay
        if not isRealData:
            job.LArTTL1Maker.EvtStore = overlayFlags.evtStore()
    
    if DetFlags.simulateLVL1.Tile_on():
        include( "TileSimAlgs/TileTTL1_jobOptions.py" )

        # If we are doing MC overlay
        if not isRealData:
            job.TileHitToTTL1.TileTTL1Container = overlayFlags.evtStore() + '/TileTTL1Cnt'
            job.TileHitToTTL1.TileMBTSTTL1Container = overlayFlags.evtStore() + '/TileTTL1MBTS'

        include( "TileSimAlgs/TileMuonReceiver_jobOptions.py" )

    # Add special TTL1 overlay algorithm only for MC+MC overlay
    if not isRealData and DetFlags.simulateLVL1.LAr_on() and DetFlags.simulateLVL1.Tile_on():
        job += CfgGetter.getAlgorithm("OverlayTTL1")

    if DetFlags.digitize.LVL1_on():
       #--------------------------------------------------------------
       # set up TrigConfigSvc for LVL1 simulation
       #--------------------------------------------------------------
       #In case TriggerFlags are not setup
       if 'TriggerFlags' not in dir():
          log.info( "TriggerFlags not previously defined so using default XML file" )
          # enable reading from XML file option and then set trigger menu
          from TriggerJobOpts.TriggerFlags import TriggerFlags
          # default is taken from TriggerFlags of TriggerJobOpts package
          TriggerFlags.triggerConfig = "LVL1:default"
        
       #PJB 9/2/2009 now use the triggerConfig flags
       # - TriggerFlags already setup so use them here
       #
       from AthenaCommon.AppMgr import ServiceMgr
       if not hasattr( ServiceMgr, 'LVL1ConfigSvc' ):
          log.info( "Will setup LVL1ConfigSvc and add instance to ServiceMgr" )
        
          from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc,findFileInXMLPATH
          LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')

          #If read from DB then set up the connection and pass keys
          from TriggerJobOpts.TriggerFlags import TriggerFlags
          if TriggerFlags.readMenuFromTriggerDb():
             log.info( "LVL1ConfigSvc uses the TriggerDB %s ", TriggerFlags.triggerDbConnection() )
             db = TriggerFlags.triggerDbConnection()
             if 'sqlite' in db.lower():  #e.g. "sqlite_file://test.db"
                LVL1ConfigSvc.ConfigSource = 'sqlite'
                LVL1ConfigSvc.DBServer = db.split('//')[-1]
             if 'oracle' in db.lower():  #e.g. "oracle://ATONR;schema=ATLAS_CONF_TRIGGER_V2;user=ATLAS_CONF_TRIGGER_V2_R;passwd=xxx;'"
                LVL1ConfigSvc.ConfigSource = 'oracle'
                LVL1ConfigSvc.DBServer  = (db.split(';')[0]).split('//')[-1]
                LVL1ConfigSvc.DBAccount = (db.split(';')[1]).split('=')[-1]
                LVL1ConfigSvc.DBUser    = (db.split(';')[2]).split('=')[-1]
                LVL1ConfigSvc.DBPass    = (db.split(';')[3]).split('=')[-1]
             if 'dblookup' in db.lower(): #e.g. "dblookup://TRIGGERDB
                LVL1ConfigSvc.ConfigSource = 'DBLookUp'
                LVL1ConfigSvc.DBServer  = db.split('//')[-1]
                
             LVL1ConfigSvc.DBSMKey     = int(TriggerFlags.triggerDbKeys()[0])
             LVL1ConfigSvc.DBLVL1PSKey = int(TriggerFlags.triggerDbKeys()[1])
             LVL1ConfigSvc.DBBGSKey    = int(TriggerFlags.triggerDbKeys()[3])

          #Otherwise read from xml
          else:
             log.info( "LVL1ConfigSvc uses xml file %s ", TriggerFlags.inputLVL1configFile()  )
             LVL1ConfigSvc.XMLFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())

          ServiceMgr += LVL1ConfigSvc
       else:
          log.info( "LVL1ConfigSvc already created" )

       #--------------------------------------------------------------
       # RPC stuff
       #--------------------------------------------------------------
       if DetFlags.simulateLVL1.RPC_on():
          import TrigT1RPCsteering.TrigT1RPCsteeringConfig

       #--------------------------------------------------------------
       # TGC stuff
       #--------------------------------------------------------------
       #if DetFlags.simulateLVL1.TGC_on():
       #   import TrigT1TGC.TrigT1TGCConfig

       #--------------------------------------------------------------
       # TrigT1Muctpi Algos
       #--------------------------------------------------------------
       if DetFlags.simulateLVL1.RPC_on() or DetFlags.simulateLVL1.TGC_on():
          from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
          job += L1Muctpi()

       #-------------------------------------------------------
       # TrigT1CaloSim Algos
       #-------------------------------------------------------
       if DetFlags.simulateLVL1.Calo_on():
          if DetFlags.simulateLVL1.LAr_on() and DetFlags.simulateLVL1.Tile_on():
             from TriggerJobOpts.Lvl1TriggerGetter import Lvl1SimulationGetter
             lvl1 = Lvl1SimulationGetter()
             protectedInclude( "TrigT1CaloCalibConditions/L1CaloCalibConditions_Overlay.py" )
             job.Run2TriggerTowerMaker.DoOverlay = True
          else:
             log.warning("NOT SIMULATING L1CALO!")
             log.warning("If only one of LAr and Tile LVL1 digitzation is set on then the L1Calo will NOT be simulated.")
             log.warning("Currently DetFlags.simulateLVL1.LAr_on()=%s, DetFlags.simulateLVL1.Tile_on()=%s.", str(DetFlags.simulateLVL1.LAr_on()), str(DetFlags.simulateLVL1.Tile_on()) )
             log.warning("If you REALLY want to do this then run L1Calo within reconstruction.")

       #-------------------------------------------------------
       # TrigT1MBTS Alg
       #-------------------------------------------------------
    #   if DetFlags.simulateLVL1.Calo_on():
    #      from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
    #      job += LVL1__TrigT1MBTS()

       #-------------------------------------------------------
       # TrigT1BCM Alg
       #-------------------------------------------------------
       if DetFlags.simulateLVL1.BCM_on():
          from TrigT1BCM.TrigT1BCMConf import LVL1__TrigT1BCM
          job += LVL1__TrigT1BCM()

       #-------------------------------------------------------
       # TrigT1LUCID Alg
       #-------------------------------------------------------
       if DetFlags.simulateLVL1.Lucid_on():
          from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid
          job += LVL1__TrigT1Lucid()
    
       #-------------------------------------------------------
       # TrigT1CTP Algos
       #-------------------------------------------------------
       #from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInDigi
       #job += CTPSimulationInDigi()

       #-------------------------------------------------------
       # TrigT1RoIB Algos
       #-------------------------------------------------------
       #from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilderInDigi
       #job += RoIBuilderInDigi("RoIBuilder")
    
