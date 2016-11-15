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

   if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool().split('/')[-1]):
      tdt_local_logger.error('DQ Monitoring is being asked to set up the TrigDecisionTool for some reason.  THIS IS A TERRIBLE IDEA AND SHOULD BE CONSIDERED A BUG!')
      from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
      monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool(),
                                              OutputLevel=ERROR,
                                              PublicChainGroups = {"EFTau": "EF_[0-9]?tau.*",
                                                                   "EFPhoton": "EF_[0-9]?g*",
                                                                   "EFJets":"EF_J.*",
                                                                  }
                                             )
      ToolSvc += monTrigDecTool
      # The following should be provided automatically when setting up the tool, but is not
      # Still needs to be provided by the trigger
      # When trigger has a standard procedure for this, switch
      ToolSvc.TrigDecisionTool.TrigConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"
      from TrigEDMConfig.TriggerEDM import EDMLibraries
      ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
   else:
      monTrigDecTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool().split('/')[-1])
   tdt_local_logger.info('Scheduled monitoring TDT %s', monTrigDecTool)

   tdt_local_logger.info('Scheduling the trigger translator')
   # Look up all monitoring menu lists, shove into trigger translator
   # pass them in as joined strings, unpack in tool
   from TrigHLTMonitoring.HLTMonTriggerList import HLTMonTriggerList
   tdt_local_hltconfig = HLTMonTriggerList()
   import collections
   tdt_mapping = {}
   for tdt_menu, tdt_menu_item in tdt_local_hltconfig.__dict__.items():
      if not isinstance(tdt_menu_item, collections.Iterable): continue
      # work around possibly buggy category items
      if isinstance(tdt_menu_item, basestring): 
         tdt_local_logger.debug('String, not list: %s' % tdt_menu)
         tdt_menu_item = [tdt_menu_item]
      if len([_ for _ in tdt_menu_item if not (_.startswith('HLT_') or _.startswith('L1'))]) != 0:
         tdt_local_logger.debug('Bad formatting: %s' % tdt_menu)
      patched_names = []
      tdt_menu_item = [_ if (_.startswith('HLT_') or _.startswith('L1_')) else 'HLT_' + _
                       for _ in tdt_menu_item]
      tdt_mapping[tdt_menu] = ','.join(tdt_menu_item)
         
   for k, v in tdt_mapping.items():
      tdt_local_logger.info('Category %s resolves to %s' % (k, v))

   from AthenaMonitoring.AthenaMonitoringConf import TriggerTranslatorToolSimple
   monTrigTransTool = TriggerTranslatorToolSimple(
      name = DQMonFlags.nameTrigTransTool(),
      triggerMapping = tdt_mapping)
   ToolSvc += monTrigTransTool

del tdt_local_logger, tdt_local_hltconfig, tdt_mapping
