# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags  import rec
from RecExConfig.Configured import Configured

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.Logging import logging

log = logging.getLogger( "TriggerConfigGetter.py" )

class TriggerConfigGetter(Configured):
    """ This class brings to the job Trigger Configuration.
    """

    _environment=""

    def __init__(self, environment=""):

        if environment:
            log.info('Initialize (environment "%s")', environment)
        else:
            log.info('Initialize (interpreting rec and trigger flags)')
        self._environment = environment
        super(TriggerConfigGetter,self).__init__() # calls configure


    def configure(self):

        log.info("The following flags are set:")
        log.info("globalflags.InputFormat             : %s", globalflags.InputFormat())
        log.info("globalflags.DataSource              : %s", globalflags.DataSource())
        log.info("rec.read.*                          : RDO: %s, ESD: %s, AOD: %s, TAG: %s", rec.readRDO(), rec.readESD(), rec.readAOD(), rec.readTAG())
        log.info("rec.doWrite.*                       : ESD: %s, AOD: %s, TAG: %s", rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG())

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaConfiguration.AllConfigFlags import ConfigFlags

        # first check the input (TODO: review these environments)
        if self._environment not in ["ReadPool", "WritePool"]:

            if not ConfigFlags.Trigger.availableRecoMetadata:
                log.error("At least one run does not contain any trigger configuration data. "
                          "Turning off trigger [rec.doTrigger=False]")
                rec.doTrigger = False

            if not rec.doTrigger():
                log.info("Aborting TriggerConfigGetter as the trigger flags were switched to false")
                return True

        log.info('Creating the Trigger Configuration Services')
        log.info("ConfigFlags.Trigger.EDMVersion: %i", ConfigFlags.Trigger.EDMVersion)
        if ConfigFlags.Trigger.EDMVersion >= 3:
            if ConfigFlags.Trigger.InputContainsConfigMetadata:
                if not hasattr(svcMgr, 'xAODConfigSvc'):
                    from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
                    svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')
            else: # Does not have xAODMeta
                # Run-3 Trigger Configuration Services (just producing menu data)
                from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
                from TrigConfigSvc.TrigConfigSvcCfg import TrigConfigSvcCfg
                CAtoGlobalWrapper(TrigConfigSvcCfg,ConfigFlags)

        if not ConfigFlags.Trigger.InputContainsConfigMetadata:
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

