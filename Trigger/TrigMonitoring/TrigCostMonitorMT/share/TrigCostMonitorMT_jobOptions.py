#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

"""
Non-Component Accumulator based configuration of Trigger Cost MT Service and associated Auditor
Prefer the use of TrigCostMonitorMT.TrigCostMonitorMTConfig, if possible
"""

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp
from GaudiSvc.GaudiSvcConf import AuditorSvc
from TrigCostMonitorMT.TrigCostMonitorMTConf import TrigCostMTAuditor, TrigCostMTSvc
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.CostMonitoring.doCostMonitoring:
  trigCostService = TrigCostMTSvc()
  trigCostService.MonitorAllEvents = ConfigFlags.Trigger.CostMonitoring.monitorAllEvents
  trigCostService.SaveHashes = True # This option will go away once the TrigConfigSvc is fully up & running
  ServiceMgr += trigCostService
  
  if not hasattr(ServiceMgr, "AuditorSvc"):
    ServiceMgr += AuditorSvc()
  ServiceMgr.AuditorSvc += TrigCostMTAuditor()
  theApp.AuditAlgorithms=True
