#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


def TrigCostMonitorMTCfg(flags):
    """
    Component Accumulator based configuration of Trigger Cost MT Service and associated Auditor
    """
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    acc = ComponentAccumulator()

    if flags.Trigger.CostMonitoring.doCostMonitoring:
      trigCostService = CompFactory.TrigCostMTSvc()
      trigCostService.MonitorAllEvents = flags.Trigger.CostMonitoring.monitorAllEvents
      trigCostService.SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
      acc.addService(trigCostService)

      auditorService = CompFactory.AuditorSvc()
      tca=CompFactory.TrigCostMTAuditor()
      #acc.addService(tca)
      auditorService.Auditors=[tca.getFullJobOptName(),]
      acc.addService(auditorService)
      acc.setAppProperty("AuditAlgorithms", True)

    return acc
