###############################################################
#
# Job options file to configure:
#
#        TrigConf
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Attila Krasznahorkay (Attila.Krasznahorkay@cern.ch)
# Author: Wolfgang Ehrenfeld (wolfgang.ehrenfeld@desy.de)
#
#==============================================================
#
# LVL1 Simulation
#
#==============================================================
from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.DetFlags import DetFlags
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( "LVL1Digitization" )

#--------------------------------------------------------------
# set up simulation of LVL1 Trigger inputs
# (a prerequisite for LVL1 simulation)
#--------------------------------------------------------------
if DetFlags.simulateLVL1.LAr_on():
    protectedInclude( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )
    # Noise
    if not digitizationFlags.doCaloNoise.get_Value():
        topSequence.LArTTL1Maker.NoiseOnOff= False #(default:True)
    # PileUp
    if DetFlags.pileup.LAr_on(): #DetFlags.pileup.LVL1_on():
        topSequence.LArTTL1Maker.PileUp = True
    else:
        topSequence.LArTTL1Maker.PileUp = False

if DetFlags.simulateLVL1.Tile_on():
    protectedInclude( "TileSimAlgs/TileTTL1_jobOptions.py" )
    protectedInclude( "TileSimAlgs/TileMuonReceiver_jobOptions.py" )

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
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("HIT2RDO")
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
    if DetFlags.simulateLVL1.TGC_on():
        import TrigT1TGC.TrigT1TGCConfig

    #--------------------------------------------------------------
    # TrigT1Muctpi Algos
    #--------------------------------------------------------------
    if DetFlags.simulateLVL1.RPC_on() or DetFlags.simulateLVL1.TGC_on():
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        if ConfigFlags.Trigger.enableL1Phase1:
            from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1
            topSequence += L1MuctpiPhase1()
        else:
            from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
            topSequence += L1Muctpi()

    #-------------------------------------------------------
    # TrigT1CaloSim Algos
    #-------------------------------------------------------
    if DetFlags.simulateLVL1.Calo_on():
        if DetFlags.simulateLVL1.LAr_on() and DetFlags.simulateLVL1.Tile_on():
            protectedInclude( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1_NoCalib.py" )
        else:
            log.warning("NOT SIMULATING L1CALO!")
            log.warning("If only one of LAr and Tile LVL1 digitzation is set on then the L1Calo will NOT be simulated.")
            log.warning("Currently DetFlags.simulateLVL1.LAr_on()=%s, DetFlags.simulateLVL1.Tile_on()=%s.", str(DetFlags.simulateLVL1.LAr_on()), str(DetFlags.simulateLVL1.Tile_on()) )
            log.warning("If you REALLY want to do this then run L1Calo within reconstruction.")

    #-------------------------------------------------------
    # TrigT1MBTS Alg
    #-------------------------------------------------------
    if DetFlags.simulateLVL1.Calo_on():
        from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
        topSequence += LVL1__TrigT1MBTS()

    #-------------------------------------------------------
    # TrigT1BCM Alg
    #-------------------------------------------------------
    if DetFlags.simulateLVL1.BCM_on():
        from TrigT1BCM.TrigT1BCMConf import LVL1__TrigT1BCM
        topSequence += LVL1__TrigT1BCM()

    #-------------------------------------------------------
    # TrigT1TRT Alg
    #-------------------------------------------------------
    from AthenaCommon.BeamFlags import jobproperties
    if DetFlags.simulateLVL1.TRT_on():
        from TrigT1TRT.TrigT1TRTConf import LVL1__TrigT1TRT
        topSequence += LVL1__TrigT1TRT()

    #-------------------------------------------------------
    # TrigT1LUCID Alg
    #-------------------------------------------------------
    if DetFlags.simulateLVL1.Lucid_on():
        from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid
        topSequence += LVL1__TrigT1Lucid()

    #-------------------------------------------------------
    # TrigT1CTP Algos
    #-------------------------------------------------------
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInDigi
    topSequence += CTPSimulationInDigi()

    #-------------------------------------------------------
    # TrigT1RoIB Algos
    #-------------------------------------------------------
    from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilderInDigi
    topSequence += RoIBuilderInDigi("RoIBuilder")
