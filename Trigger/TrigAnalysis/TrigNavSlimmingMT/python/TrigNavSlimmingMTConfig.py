#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging

def getTrigNavSlimmingMTConfig(ConfigFlags):

  log = logging.getLogger("getTrigNavSlimmingMTConfig.py")

  from TrigDecisionTool.TrigDecisionToolConfig import getTrigDecisionTool
  ca = getTrigDecisionTool(ConfigFlags)
  tdt = ca.getPrimary()

  # TODO remove when deprecated
  from RecExConfig.RecFlags  import rec

  # NOTE: This setup does not yet support derivations

  if ConfigFlags.Output.doWriteESD or rec.doWriteESD():
    esdSlim = CompFactory.TrigNavSlimmingMTAlg('TrigNavSlimmingMTAlg_ESD')
    esdSlim.TrigDecisionTool = tdt
    esdSlim.OutputCollection = "HLTNav_Summary_ESDSlimmed"
    esdSlim.PrimaryInputCollection = "HLTNav_Summary"
    esdSlim.KeepFailedBranched = True
    esdSlim.KeepOnlyFinalFeatures = False
    ca.addEventAlgo(esdSlim)
    log.info("Producing ESD Slimmed Trigger Navigation Collections")

  if ConfigFlags.Output.doWriteAOD or rec.doWriteAOD():
    aodSlim = CompFactory.TrigNavSlimmingMTAlg('TrigNavSlimmingMTAlg_ESD')
    aodSlim.TrigDecisionTool = tdt
    aodSlim.OutputCollection = "HLTNav_Summary_AODSlimmed"
    if rec.readESD():
      aodSlim.PrimaryInputCollection = "HLTNav_Summary_ESDSlimmed"
    else:
      aodSlim.PrimaryInputCollection = "HLTNav_Summary"
    #
    if ConfigFlags.Trigger.AODEDMSet == "AODFULL" or ConfigFlags.Trigger.AODEDMSet == "AODRun3_LARGE":
      aodSlim.KeepFailedBranched = True
      aodSlim.KeepOnlyFinalFeatures = False
      log.info("Producing AOD LARGE Slimmed Trigger Navigation Collections")
    else:
      aodSlim.KeepFailedBranched = False
      aodSlim.KeepOnlyFinalFeatures = True
      log.info("Producing AOD SMALL Slimmed Trigger Navigation Collections")
    ca.addEventAlgo(aodSlim)

  return ca