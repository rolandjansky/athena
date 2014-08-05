tdt_local_logger = logging.getLogger('TrigDecTool_jobOptions')

# Set up the trigger decision tool (for trigger-aware monitoring)
if not 'DQMonFlags' in dir():
   tdt_local_logger.debug("DQMonFlags not yet imported - I import them now")
   from AthenaMonitoring.DQMonFlags import DQMonFlags

if DQMonFlags.useTrigger():
   if not 'rec' in dir():
      from RecExConfig.RecFlags import rec

   if rec.readESD() and (DQMonFlags.monManEnvironment=='tier0ESD'):
      # for ESD->AOD use trigger config stored as in-file meta-data,
      if not 'TriggerFlags' in dir():
         from TriggerJobOpts.TriggerFlags import TriggerFlags
      TriggerFlags.configurationSourceList = ['ds']

      # set up trigger config service
      if not 'TriggerConfigGetter' in dir():
         from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
         cfg = TriggerConfigGetter()

   if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
      from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
      monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool(),
                                              OutputLevel=ERROR,
                                              PublicChainGroups = {"EFTau": "EF_[0-9]?tau.*",
                                                                   "EFPhoton": "EF_[0-9]?g*",
                                                                   "EFJets":"EF_J.*",
                                                                  }
                                             )
      ToolSvc += monTrigDecTool

del tdt_local_logger
