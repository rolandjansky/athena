#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


def TrigCostMonitorMTCfg(flags):
    """
    Component Accumulator based configuration of Trigger Cost MT Service and associated Auditor
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TrigCostMonitorMTPostSetup')

    acc = ComponentAccumulator()

    if flags.Trigger.CostMonitoring.doCostMonitoring:
      trigCostService = CompFactory.TrigCostMTSvc()
      trigCostService.MonitorAllEvents = flags.Trigger.CostMonitoring.monitorAllEvents
      trigCostService.SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
      acc.addService(trigCostService)

      auditorService = CompFactory.AuditorSvc()
      tca=CompFactory.TrigCostMTAuditor()
      auditorService.Auditors=[tca.getFullJobOptName(),]
      acc.addService(auditorService)
      acc.setAppProperty("AuditAlgorithms", True)
      log.info('Enabling online trigger cost monitoring')
    else:
      log.info('Will NOT schedule online trigger cost monitoring')

    return acc

def TrigCostMonitorMTPostSetup():
  from AthenaCommon.Logging import logging
  log = logging.getLogger('TrigCostMonitorMTPostSetup')
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  if 'doCostMonitoring' in svcMgr.ROBDataProviderSvc.properties():
      svcMgr.ROBDataProviderSvc.doCostMonitoring = \
          (ConfigFlags.Trigger.CostMonitoring.doCostMonitoring and ConfigFlags.Trigger.CostMonitoring.monitorROBs)
      log.info('Set ROBDataProviderSvc.doCostMonitoring=%s', svcMgr.ROBDataProviderSvc.doCostMonitoring)
  else:
      log.info('ROBDataProviderSvc does not have property doCostMonitoring - will not do cost monitor for ROS.')

  # Set CostSvc for TrigErrorMonTool
  if hasattr (svcMgr,'HltEventLoopMgr') and hasattr(svcMgr.HltEventLoopMgr,'TrigErrorMonTool'):
      if ConfigFlags.Trigger.CostMonitoring.doCostMonitoring:
          from AthenaConfiguration.ComponentFactory import CompFactory
          svcMgr.HltEventLoopMgr.TrigErrorMonTool.TrigCostMTSvc = CompFactory.TrigCostMTSvc()

