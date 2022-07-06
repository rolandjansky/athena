#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#


def TrigCostMonitorCfg(flags, seqName=""):
    """
    Component Accumulator based configuration of Trigger Cost Service and associated Auditor
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TrigCostMonitorSetup')

    acc = ComponentAccumulator(seqName) if seqName else ComponentAccumulator()

    if flags.Trigger.CostMonitoring.doCostMonitoring:
      costSvc = CompFactory.TrigCostSvc(
          MonitorAllEvents = flags.Trigger.CostMonitoring.monitorAllEvents,
          SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
      )
      acc.addService(costSvc)
      acc.addAuditor(CompFactory.TrigCostAuditor())
      acc.setAppProperty("AuditAlgorithms", True)

      costSupervisorAlg = CompFactory.TrigCostSupervisorAlg()
      costSupervisorAlg.TrigCostSvc = costSvc
      acc.addEventAlgo(costSupervisorAlg)

      log.info('Enabling online trigger cost monitoring')
    else:
      log.info('Will NOT schedule online trigger cost monitoring')
      return ComponentAccumulator()

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