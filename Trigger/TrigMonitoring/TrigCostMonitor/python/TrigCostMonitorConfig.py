#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#


def TrigCostMonitorCfg(flags):
    """
    Component Accumulator based configuration of Trigger Cost Service and associated Auditor
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TrigCostMonitorPostSetup')

    acc = ComponentAccumulator()

    if flags.Trigger.CostMonitoring.doCostMonitoring:
      acc.addService(CompFactory.TrigCostSvc(
          MonitorAllEvents = flags.Trigger.CostMonitoring.monitorAllEvents,
          SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
      ))
      acc.addAuditor(CompFactory.TrigCostAuditor())
      acc.setAppProperty("AuditAlgorithms", True)
      log.info('Enabling online trigger cost monitoring')
    else:
      log.info('Will NOT schedule online trigger cost monitoring')

    return acc

def TrigCostMonitorPostSetup():
  from AthenaCommon.Logging import logging
  log = logging.getLogger('TrigCostMonitorPostSetup')
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
          svcMgr.HltEventLoopMgr.TrigErrorMonTool.TrigCostSvc = CompFactory.TrigCostSvc()

