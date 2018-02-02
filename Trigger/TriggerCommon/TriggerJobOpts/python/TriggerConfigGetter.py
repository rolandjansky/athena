# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__author__  = 'J. Stelzer'

import re

from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection

from TriggerJobOpts.TriggerFlags import TriggerFlags
from RecExConfig.RecFlags  import rec
from AthenaCommon.GlobalFlags  import globalflags

from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc

from RecExConfig.Configured import Configured 

from AthenaCommon.Logging import logging 
from AthenaCommon.Include import include
from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


class TriggerConfigGetter(Configured):
    """ This class brings to the job Trigger Configuration.
    """

    _environment=""

    def __init__(self, environment=""):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        if environment:
            log.info('Initialize (environment "%s")' % environment)
        else:
            log.info('Initialize (interpreting rec and trigger flags)')
        self._environment = environment
        super(TriggerConfigGetter,self).__init__() # calls configure



    def checkFileMetaData(self):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        from RecExConfig.InputFilePeeker import inputFileSummary
        self.hasLBwiseHLTPrescalesAndL1ItemDef = True
        if rec.readESD() or rec.readAOD() or "ReadPool" in self._environment:
            self.hasLBwiseHLTPrescalesAndL1ItemDef = inputFileSummary['metadata'].has_key('/TRIGGER/HLT/Prescales') # they were all added at the same time (Repro with 15.6.3.2 Prod)
        # protection against early runs
        if inputFileSummary.has_key('run_number') and self._environment=="" and globalflags.DataSource()=='data' and rec.readRDO() and any([run<134230 for run in inputFileSummary['run_number']]):
            self.hasLBwiseHLTPrescalesAndL1ItemDef = False
        if self.hasLBwiseHLTPrescalesAndL1ItemDef:
            log.info("Using LB-wise HLT prescales")
        else:
            log.info("Using run-wise HLT prescales")



    def checkInput(self):

        self.checkFileMetaData()

        log = logging.getLogger( "TriggerConfigGetter.py" )
        if "ReadPool" in self._environment or "WritePool" in self._environment:
            log.info("Flags are ignored, since 'ReadPool' or  'WritePool' is specified")
            return True

        if rec.readESD() or rec.readAOD(): # and globalflags.DataSource()=='data':  # need this for MC as well
            protectedInclude("TrigTier0/TriggerConfigCheckMetadata.py")

        if rec.readRDO() and globalflags.InputFormat()=='bytestream' and globalflags.DataSource()=='data':
            protectedInclude("TrigTier0/TriggerConfigCheckHLTpsk.py")

        log.info("The following flags are set:")
        log.info("globalflags.InputFormat             : %s" % globalflags.InputFormat())
        log.info("rec.read.*                          : RDO: %s, ESD: %s, AOD: %s, TAG: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD(), rec.readTAG()) )
        log.info("rec.doWrite.*                       : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )
        log.info("globalflags.DataSource              : %s" % globalflags.DataSource())
        log.info("TriggerFlags.configForStartup       : %s" % TriggerFlags.configForStartup())
        log.info("TriggerFlags.dataTakingConditions   : %s" % TriggerFlags.dataTakingConditions())
        log.info("TriggerFlags.configurationSourceList: %s" % TriggerFlags.configurationSourceList())

        count = len([1 for x in [rec.readRDO(),rec.readESD(),rec.readAOD()] if x ]) #readTAG is only set with another input
        if count == 0:
            log.warning("Don't know what the input format is.")
            return False
        if count >= 2:
            log.warning("More than one input format specified, please set only the appropriate one.")
            return False
        return True




    def setConfigSvcConnParams(self,connectionParameters):
        sl = []
        if hasattr(svcMgr,'L1TopoConfigSvc'): sl += [svcMgr.L1TopoConfigSvc]
        if hasattr(svcMgr,'LVL1ConfigSvc'): sl += [svcMgr.LVL1ConfigSvc]
        if hasattr(svcMgr,'HLTConfigSvc'): sl += [svcMgr.HLTConfigSvc]

        if "alias" in connectionParameters:
            for svc in sl:
                svc.ConfigSource = 'DBLookUp'
                svc.DBServer  = connectionParameters["alias"]
                svc.DBUser = ""
        else:
            technology = connectionParameters["techno"]
            for svc in sl:
                svc.ConfigSource = technology

            if technology == 'sqlite':
                for svc in sl:
                    svc.DBServer = connectionParameters["filename"]
                    svc.DBUser = "dummy"

            if technology == 'oracle':
                for svc in sl:
                    svc.DBServer  = connectionParameters["server"]
                    svc.DBAccount = connectionParameters["schema"]
                    svc.DBUser    = connectionParameters["user"  ]
                    svc.DBPass    = connectionParameters["passwd"]

        if hasattr(svcMgr,'L1TopoConfigSvc'):
            svcMgr.L1TopoConfigSvc.DBSMKey     = TriggerFlags.triggerDbKeys()[0]
            svcMgr.L1TopoConfigSvc.UseFrontier = TriggerFlags.triggerUseFrontier()
        if hasattr(svcMgr,'LVL1ConfigSvc'):
            svcMgr.LVL1ConfigSvc.DBSMKey     = TriggerFlags.triggerDbKeys()[0]
            svcMgr.LVL1ConfigSvc.DBLVL1PSKey = TriggerFlags.triggerDbKeys()[1]
            svcMgr.LVL1ConfigSvc.DBBGSKey    = TriggerFlags.triggerDbKeys()[3]
            svcMgr.LVL1ConfigSvc.UseFrontier = TriggerFlags.triggerUseFrontier()
        if hasattr(svcMgr,'HLTConfigSvc'):
            svcMgr.HLTConfigSvc.DBSMKey      = TriggerFlags.triggerDbKeys()[0]
            svcMgr.HLTConfigSvc.DBHLTPSKey   = TriggerFlags.triggerDbKeys()[2]
            svcMgr.HLTConfigSvc.UseFrontier  = TriggerFlags.triggerUseFrontier()




    def configure(self):
        log = logging.getLogger( "TriggerConfigGetter.py" )

        # first check the input
        if "HIT2RDO" in self._environment:
            TriggerFlags.doLVL2 = False
            TriggerFlags.doEF = False
            log.info("For simulation jobs the following flags are set:")
            log.info("globalflags.InputFormat             : %s" % globalflags.InputFormat())
            log.info("globalflags.DataSource              : %s" % globalflags.DataSource())
            log.info("TriggerFlags.configForStartup       : %s" % TriggerFlags.configForStartup())
            log.info("TriggerFlags.dataTakingConditions   : %s" % TriggerFlags.dataTakingConditions())
            log.info("TriggerFlags.doLVL2                 : %s" % TriggerFlags.doLVL2())
            log.info("TriggerFlags.doEF                   : %s" % TriggerFlags.doEF())
        else:
            if not self.checkInput():
                log.error("Could not determine job input. Can't setup trigger configuration and will return!")
                return

        self.readPool       = globalflags.InputFormat() == 'pool'
        self.readRDO        = rec.readRDO()
        self.writeESDAOD    = rec.doWriteESD() or rec.doWriteAOD() or rec.doWriteDPD()
        self.ConfigSrcList  = TriggerFlags.configurationSourceList()
        self.readMC         = globalflags.DataSource()=='geant3' or globalflags.DataSource()=='geant4'
        self.readTriggerDB  = TriggerFlags.readMenuFromTriggerDb() and self.readRDO
        self.isCommisioning = globalflags.DataSource()=='data' and globalflags.DetGeo()=='commis' 
        self.l1Folders      = TriggerFlags.dataTakingConditions()=='FullTrigger' or TriggerFlags.dataTakingConditions()=='Lvl1Only'
        self.hltFolders     = TriggerFlags.dataTakingConditions()=='FullTrigger' or TriggerFlags.dataTakingConditions()=='HltOnly'
        self.isRun1Data     = False 
        if globalflags.DataSource()=='data':
            from RecExConfig.AutoConfiguration  import GetRunNumber
            runNumber = GetRunNumber()
            if runNumber > 0 and runNumber < 230000 :
                self.isRun1Data = True
        self.isTriggerReprocessing = False

        # the TriggerFlags.readMenuFromTriggerDb() tells us that we read the trigger menu from the database
        # the connection itself is defined in TriggerFlags.triggerDbConnection()

        # reading from the TriggerDB can mean different things:

        # a) TriggerFlags doLVL2() and doEF() are both False:
        #    - create a tmp sqlite file with the conditions (menu)
        #    - use DSConfigSvc


        # b) TriggerFlags doLVL2() or doEF() is True:
        #    - use HLTConfigSvc
        if self.readTriggerDB and (TriggerFlags.doLVL2() or TriggerFlags.doEF() or TriggerFlags.doHLT()):

            self.ConfigSrcList = ['xml'] # to use L1/HLTConfigSvc and not DSConfigSvc, but only if we are running the HLT


        if self._environment: # I don't think anyone calls TriggerConfigGetter with an argument
            self.readPool  = False
            self.writeESDAOD = False
            self.readHits = False
            if "ReadPoolRDO" in self._environment:
                self.readPool = True
                self.readRDO = True
            elif "ReadPool" in self._environment:
                self.readPool = True
                self.readRDO = False
            if "WritePool" in self._environment:
                self.writeESDAOD = True
            if "HIT2RDO" in self._environment:
                self.readRDO = False
                self.readHits = True


        # define ConfigSvc
        if not self.ConfigSrcList:
            if (self.readPool and not self.readRDO) or (self.readRDO and not self.readPool): # (ESD, AOD, DPD) or (RDO-BS)
                self.ConfigSrcList = ['ds']
            elif (self.readRDO and self.readPool) or rec.readTAG() or self.readHits:           # (RDO-MC) or TAG
                self.ConfigSrcList = ['xml']
            else: # should not get here: should be found by checkInput
                log.fatal('no reading of BS, RDO, AOD, ESD, or TAG specified')

        # we need the temporary COOL database, if we read the configuration from XML and write ESD/AOD (or have 'ds' set for some reason)
        self.makeTempCool   = self.readRDO and \
                              ( self.writeESDAOD or 'ds' in self.ConfigSrcList ) and \
                              ( self.readMC \
                                or (self.isCommisioning and (TriggerFlags.readLVL1configFromXML() and TriggerFlags.readHLTconfigFromXML())) \
                                or TriggerFlags.readMenuFromTriggerDb() )

        log.info("Need to create temporary cool file? : %r" % self.makeTempCool)

        log.info('Creating the Trigger Configuration Services')
        self.svc = SetupTrigConfigSvc()

        #set the merged system
        #self.svc.doMergedHLT = TriggerFlags.doHLT() 

        if 'xml' in self.ConfigSrcList or self.makeTempCool:
            # sets them if plain XML reading is to be used
            self.svc.l1topoXmlFile = TriggerFlags.outputL1TopoConfigFile()  # generated in python
            self.svc.l1XmlFile     = TriggerFlags.outputLVL1configFile()    # generated in python
            self.svc.hltXmlFile    = TriggerFlags.outputHLTconfigFile()     # generated in python
            if TriggerFlags.readL1TopoConfigFromXML():
                self.svc.l1topoXmlFile  = TriggerFlags.inputL1TopoConfigFile() # given XML
            if TriggerFlags.readLVL1configFromXML():
                self.svc.l1XmlFile  = TriggerFlags.inputLVL1configFile() # given XML
            if TriggerFlags.readHLTconfigFromXML():
                self.svc.hltXmlFile  = TriggerFlags.inputHLTconfigFile()   # given XML



        ### preparations are done!
        try:
            self.svc.SetStates( self.ConfigSrcList )
        except:
            log.error( 'Failed to set state of TrigConfigSvc to %r' % self.ConfigSrcList )
        else:
            log.info('The following configuration services will be tried: %r' % self.ConfigSrcList )


        try:     self.svc.InitialiseSvc()
        except Exception, ex: log.error( 'Failed to activate TrigConfigSvc: %r' % ex )

        if self.readTriggerDB:
            log.info( "Using TriggerDB connection '%s'" % TriggerFlags.triggerDbConnection() )
            self.trigDbConnectionParameters = interpretConnection(TriggerFlags.triggerDbConnection(), resolveAlias=False)
            self.setConfigSvcConnParams(self.trigDbConnectionParameters)

        log.info("TriggerFlags.triggerCoolDbConnection is '%s' [default: '']" % TriggerFlags.triggerCoolDbConnection())
        TrigCoolDbConnection = TriggerFlags.triggerCoolDbConnection()

        if self.makeTempCool:
            TrigCoolDbConnection = self.setupTempCOOLWriting(TrigCoolDbConnection)

        if 'ds' in self.ConfigSrcList or self.writeESDAOD:
            self.setupCOOLReading(TrigCoolDbConnection)

        if hasattr(svcMgr, 'DSConfigSvc'):
            db = 'TRIGGERDB'
            if self.isRun1Data:
                db = 'TRIGGERDB_RUN1'
            elif self.readMC:
                db = 'TRIGGERDBMC'
            elif self.isTriggerReprocessing:
                db = 'TRIGGERDBREPR'
            svcMgr.DSConfigSvc.ConfigSource = 'dblookup'
            svcMgr.DSConfigSvc.DBServer = db
            log.info("DSConfigSvc trigger database is '%s'" % db)
        
        if self.writeESDAOD:
            self.setupCOOLWriting()
            self.setupxAODWriting()

        # all went fine we are configured
        return True





    def setupTempCOOLWriting(self,TrigCoolDbConnection):
        log = logging.getLogger( "TriggerConfigGetter.py" )

        log.info( 'Trigger the copying of COOL data into DetectorStore. I am not certain this is needed any longer JS.')

        # if we have MC data (nothing in ORACLE/COOL) we need to write an SQlite file
        # and change the dbConnection
        if ( self.readMC \
             or (self.isCommisioning and (TriggerFlags.readLVL1configFromXML and TriggerFlags.readHLTconfigFromXML)) \
             or TriggerFlags.readMenuFromTriggerDb ):

            log.info( 'TempCoolSetup: Setting up the writing of a temporary COOL DB')

            from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
            if self.readTriggerDB:
                log.info("TempCoolSetup: source is db [%s] with keys %s/%s/%s" % (TriggerFlags.triggerDbConnection(),TriggerFlags.triggerDbKeys()[0],TriggerFlags.triggerDbKeys()[1],TriggerFlags.triggerDbKeys()[2]))
                theConfCOOLWriter.smk        = TriggerFlags.triggerDbKeys()[0]
                theConfCOOLWriter.l1psk      = TriggerFlags.triggerDbKeys()[1]
                theConfCOOLWriter.hltpsk     = TriggerFlags.triggerDbKeys()[2]
                theConfCOOLWriter.setTriggerDBConnection(self.trigDbConnectionParameters)
            else:
                log.info("TempCoolSetup: sources are '%s' and '%s'" % (self.svc.l1XmlFile,self.svc.hltXmlFile) )
                theConfCOOLWriter.lvl1menu = self.svc.l1XmlFile
                theConfCOOLWriter.hltmenu  = self.svc.hltXmlFile
            if TrigCoolDbConnection == "": # nothing specified by the user
                TrigCoolDbConnection = re.match(".*;schema=(.*);dbname=.*",theConfCOOLWriter.dbConnection).group(1)
                theConfCOOLWriter.isWritingNeeded = True
                log.info("TempCoolSetup: Setting TrigCoolDbConnection to %s" % TrigCoolDbConnection )
                log.info("TempCoolSetup: Enabling writing and IOV adjustment")

        return TrigCoolDbConnection



    def setupCOOLReading(self,TrigCoolDbConnection):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        if 'ds' in self.ConfigSrcList:
            log.info( 'DSConfigSvc enabled, will setup IOVDbSvc to access configuration meta data')
        if self.writeESDAOD:
            log.info( 'ESD/AOD writing enabled, will setup IOVDbSvc to access configuration meta data')
        usePresetConnection = (TrigCoolDbConnection != "")

        ## if we process MC from an XML file the dbConnection needs to
        ## be set to a local SQlite file

        # when reading from the COOL database (either form
        # ORACLE/SQlite) we need an algorithm that triggers the
        # copying
        #
        # In addition for MC the IOV has to be adjusted since in COOL the config data is written with IOV=infinity
        if self.readRDO:

            # setup the IOV Changer
            from AthenaCommon.AlgSequence import AlgSequence
            from TrigConfigSvc.TrigConfigSvcConf import TrigConf__TrigConfDataIOVChanger as TrigConfDataIOVChanger
            topAlgs = AlgSequence()
            TrigConfDataIOVChanger = TrigConfDataIOVChanger('TrigConfDataIOVChanger')
            topAlgs += TrigConfDataIOVChanger
            if self.makeTempCool:
                TrigConfDataIOVChanger.AdjustIOV = True


        if TrigCoolDbConnection=='':
            log.info("COOL DBConnection: not set, will use default conditions database" )
            TrigCoolDbConnection = 'TRIGGER'
            addNewFolders = TriggerFlags.configForStartup()=="HLTonline" and self.readRDO
        else: # for sqlite COOL: temp (usually /tmp/hltMenu.xxx.db) or predefined (e.g. trigconf.db)
            log.info("COOL DBConnection: " + TrigCoolDbConnection )
            addNewFolders = ( ( TriggerFlags.configForStartup()=="HLToffline"
                                or TriggerFlags.configForStartup()=="HLTonline"
                                or globalflags.DataSource()!='data')
                              and self.readRDO )  # bytestream or MC RDO

        # add folders for reading
        from IOVDbSvc.CondDB import conddb
        folders = []
        if self.hltFolders: folders += [ "HLT/Menu", "HLT/HltConfigKeys" ]
        if self.l1Folders:  folders += [ "LVL1/Lvl1ConfigKey", "LVL1/Menu", "LVL1/Prescales" ]
        if globalflags.DataSource() == 'data':
            if self.l1Folders:  folders += [ "LVL1/BunchGroupKey", "LVL1/BunchGroupDescription", "LVL1/BunchGroupContent" ]
        if self.hasLBwiseHLTPrescalesAndL1ItemDef:
            if self.hltFolders: folders += [ "HLT/Prescales", "HLT/PrescaleKey" ]
            if self.l1Folders:  folders += [ "LVL1/ItemDef" ]

        log.info("Adding folders to IOVDbSvc")

        if addNewFolders:
            # Need thresholds folders but only for Tier0 BS->ESD
            log.info("Also adding new folders to IOVDbSvc")
            if self.hltFolders: folders += [ "HLT/Groups" ]
            if self.l1Folders: folders += [ "LVL1/Thresholds" ]

        for f in folders:
            log.info("     /TRIGGER/%s" % f)
            conddb.addFolderWithTag(TrigCoolDbConnection, "/TRIGGER/%s" % f, "HEAD")




    def setupCOOLWriting(self):
        """method to setup the writing of the configuration
        to ESD/AOD/poolDPD"""
        log = logging.getLogger( "TriggerConfigGetter.py" )
        log.info( 'ESD/AOD writing enabled, will setup IOVDbSvc to write configuration meta data')

        folders = []
        if self.hltFolders: folders += ["HLT/Menu", "HLT/HltConfigKeys"]
        if self.l1Folders:  folders += ["LVL1/Lvl1ConfigKey", "LVL1/Menu", "LVL1/Prescales", "LVL1/Thresholds", "LVL1/BunchGroupKey"]

        if self.hasLBwiseHLTPrescalesAndL1ItemDef:
            if self.hltFolders: folders += [ "HLT/Prescales", "HLT/PrescaleKey" ]
            if self.l1Folders:  folders += [ "LVL1/ItemDef" ]

        log.info("Adding output folders to IOVDbSvc")
        for f in folders:
            log.info("     /TRIGGER/%s" % f)
            svcMgr.IOVDbSvc.FoldersToMetaData+=["/TRIGGER/%s" % f]



    def setupxAODWriting( self ):
        """
        Method setting up the writing of the ROOT-readable trigger configuration
        metadata.
        """

        # Get a logger:
        log = logging.getLogger( "TriggerConfigGetter.py" )
        log.info( "ESD/AOD writing enabled, will set up xAOD trigger "
                  "configuration metadata writing" )

        # Get the algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topAlgs = AlgSequence()

        # Add the algorithm creating the trigger configuration metadata for
        # the output:
        try: 
            from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriter
            topAlgs += TrigConf__xAODMenuWriter( OverwriteEventObj = True )

            # The metadata objects to add to the output:
            metadataItems = [ "xAOD::TriggerMenuContainer#TriggerMenu",
                              "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]

            # Set up the metadata for the output ESD and AOD:
            from RecExConfig.ObjKeyStore import objKeyStore
            objKeyStore.addManyTypesMetaData( metadataItems )
        except ImportError: # don't want to branch in rel 18
            pass

        # Return gracefully:
        return
