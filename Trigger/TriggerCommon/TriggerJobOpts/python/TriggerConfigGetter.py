# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection

from RecExConfig.RecFlags  import rec
from RecExConfig.Configured import Configured

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


class TriggerConfigGetter(Configured):
    """ This class brings to the job Trigger Configuration.
    """

    _environment=""

    def __init__(self, environment=""):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        if environment:
            log.info('Initialize (environment "%s")', environment)
        else:
            log.info('Initialize (interpreting rec and trigger flags)')
        self._environment = environment
        super(TriggerConfigGetter,self).__init__() # calls configure


    def checkInput(self):

        log = logging.getLogger( "TriggerConfigGetter.py" )
        if "ReadPool" in self._environment or "WritePool" in self._environment:
            log.info("Flags are ignored, since 'ReadPool' or  'WritePool' is specified")
            return True

        if rec.readESD() or rec.readAOD(): # and globalflags.DataSource()=='data':  # need this for MC as well
            protectedInclude("TriggerJobOpts/TriggerConfigCheckMetadata.py")

        if rec.readRDO() and globalflags.InputFormat()=='bytestream' and globalflags.DataSource()=='data' and TriggerFlags.configForStartup()!='HLToffline':
            protectedInclude("TriggerJobOpts/TriggerConfigCheckHLTpsk.py")

        log.info("The following flags are set:")
        log.info("globalflags.InputFormat             : %s", globalflags.InputFormat())
        log.info("rec.read.*                          : RDO: %s, ESD: %s, AOD: %s, TAG: %s", rec.readRDO(), rec.readESD(), rec.readAOD(), rec.readTAG())
        log.info("rec.doWrite.*                       : ESD: %s, AOD: %s, TAG: %s", rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG())
        log.info("globalflags.DataSource              : %s", globalflags.DataSource())
        log.info("TriggerFlags.configForStartup       : %s", TriggerFlags.configForStartup())
        log.info("TriggerFlags.dataTakingConditions   : %s", TriggerFlags.dataTakingConditions())

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
        if hasattr(svcMgr,'LVL1ConfigSvc'):
            sl += [svcMgr.LVL1ConfigSvc]
        if hasattr(svcMgr,'HLTConfigSvc'):
            sl += [svcMgr.HLTConfigSvc]

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
        from PyUtils.MetaReaderPeekerFull import metadata

        # first check the input
        if "HIT2RDO" in self._environment:
            TriggerFlags.doHLT = False
            log.info("For simulation jobs the following flags are set:")
            log.info("globalflags.InputFormat             : %s", globalflags.InputFormat())
            log.info("globalflags.DataSource              : %s", globalflags.DataSource())
            log.info("TriggerFlags.configForStartup       : %s", TriggerFlags.configForStartup())
            log.info("TriggerFlags.dataTakingConditions   : %s", TriggerFlags.dataTakingConditions())
            log.info("TriggerFlags.doHLT                  : %s", TriggerFlags.doHLT())
        else:
            if not self.checkInput():
                log.error("Could not determine job input. Can't setup trigger configuration and will return!")
                return
            # self.checkInput() may call TriggerConfigCheckMetadata, this can in turn set "rec.doTrigger.set_Value_and_Lock(False)"
            # but TriggerConfigGetter might have only been called in the first place due to this flag having been true, 
            # so re-check that we're still OK to be executing here
            if not (rec.doTrigger() or TriggerFlags.doTriggerConfigOnly()):
                log.info("Aborting TriggerConfigGetter as the trigger flags were switched to false in checkInput()")
                return True

        self.readTriggerDB  = TriggerFlags.readMenuFromTriggerDb() and self.readRDO
        self.hasxAODMeta    = (
            ("metadata_items" in metadata)
            and 
            any((('TriggerMenu' or 'MenuJSON') in key) for key in metadata["metadata_items"].keys())
        )

        log.info('Creating the Trigger Configuration Services')
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        log.info("ConfigFlags.Trigger.EDMVersion: %i", ConfigFlags.Trigger.EDMVersion)
        if ConfigFlags.Trigger.EDMVersion >= 3:
            if self.hasxAODMeta:
                if not hasattr(svcMgr, 'xAODConfigSvc'):
                    from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
                    svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')
            else: # Does not have xAODMeta
                # Run-3 Trigger Configuration Services (just producing menu data)
                from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
                from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg
                CAtoGlobalWrapper(TrigConfigSvcCfg,ConfigFlags)

        if self.readTriggerDB:
            log.info( "Using TriggerDB connection '%s'", TriggerFlags.triggerDbConnection() )
            self.trigDbConnectionParameters = interpretConnection(TriggerFlags.triggerDbConnection(), resolveAlias=False)
            self.setConfigSvcConnParams(self.trigDbConnectionParameters)

        log.info("TriggerFlags.triggerCoolDbConnection is '%s' [default: '']", TriggerFlags.triggerCoolDbConnection())

        if not self.hasxAODMeta:
            self.setupxAODWriting()
        else:
            log.info("Input file already has xAOD trigger metadata. Will not re-create it.")

        # all went fine we are configured
        return True


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
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        if ConfigFlags.Trigger.EDMVersion == 1 or ConfigFlags.Trigger.EDMVersion == 2:
            from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
            from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg
            CAtoGlobalWrapper(TrigConfigSvcCfg, ConfigFlags)

            # xAODConfigSvc for accessing the Run-3 converted menu
            from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc',
                                              UseInFileMetadata = False)

        elif ConfigFlags.Trigger.EDMVersion >= 3:
            from TrigEDMConfig.TriggerEDMRun3 import recordable
            from AthenaConfiguration.ComponentFactory import CompFactory
            from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

            enhancedBiasWeightCompAlg = CompFactory.EnhancedBiasWeightCompAlg()
            enhancedBiasWeightCompAlg.EBWeight = recordable("HLT_EBWeight")
            from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
            enhancedBiasWeightCompAlg.FinalDecisionKey = getRun3NavigationContainerFromInput(ConfigFlags)
            topAlgs += conf2toConfigurable( enhancedBiasWeightCompAlg )
        else:
            raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)


        from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriterMT, TrigConf__KeyWriterTool
        topAlgs += TrigConf__xAODMenuWriterMT(KeyWriterTool=TrigConf__KeyWriterTool('KeyWriterToolOffline'))

        # Schedule also the prescale conditions algs
        from AthenaCommon.Configurable import ConfigurableRun3Behavior
        with ConfigurableRun3Behavior():
            from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, appendCAtoAthena
            from TrigConfigSvc.TrigConfigSvcCfg import L1PrescaleCondAlgCfg, HLTPrescaleCondAlgCfg
            acc = ComponentAccumulator()
            acc.merge( L1PrescaleCondAlgCfg( ConfigFlags ) )
            acc.merge( HLTPrescaleCondAlgCfg( ConfigFlags ) )
        appendCAtoAthena( acc )

        # Set up the metadata for the output ESD and AOD:
        from RecExConfig.ObjKeyStore import objKeyStore
        metadataItems = [ "xAOD::TriggerMenuJsonContainer#MenuJSON_HLT",
                          "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_HLTAux.",
                          "xAOD::TriggerMenuJsonContainer#MenuJSON_L1",
                          "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_L1Aux.",
                          "xAOD::TriggerMenuJsonContainer#MenuJSON_HLTPS",
                          "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_HLTPSAux.",
                          "xAOD::TriggerMenuJsonContainer#MenuJSON_L1PS",
                          "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_L1PSAux.",
                          # "xAOD::TriggerMenuJsonContainer#MenuJSON_BG", // TODO
                          # "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_BGAux.", // TODO
                        ]
        objKeyStore.addManyTypesMetaData( metadataItems )
