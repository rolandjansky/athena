#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging

def getTrigNavSlimmingMTOnlineConfig():
  onlineSlim = CompFactory.TrigNavSlimmingMTAlg('TrigNavSlimmingMTAlg_Online')
  onlineSlim.TrigDecisionTool = ""
  onlineSlim.OutputCollection = "HLTNav_Summary_OnlineSlimmed"
  onlineSlim.PrimaryInputCollection = "HLTNav_Summary"
  onlineSlim.KeepFailedBranched = True
  onlineSlim.KeepOnlyFinalFeatures = False
  onlineSlim.EdgesToDrop = [""]
  onlineSlim.NodesToDrop = [""]
  return onlineSlim

def TrigNavSlimmingMTCfg(ConfigFlags):

  log = logging.getLogger("getTrigNavSlimmingMTConfig.py")

  from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg, getRun3NavigationContainerFromInput, possible_keys
  ca = TrigDecisionToolCfg(ConfigFlags)
  tdt = ca.getPrimary()

  # TODO remove when deprecated
  from RecExConfig.RecFlags  import rec

  # NOTE: This setup does not yet support derivations

  inputCollection = getRun3NavigationContainerFromInput(ConfigFlags)

  if ConfigFlags.Output.doWriteESD or rec.doWriteESD():
    esdSlim = CompFactory.TrigNavSlimmingMTAlg('TrigNavSlimmingMTAlg_ESD')
    esdSlim.TrigDecisionTool = tdt
    esdSlim.OutputCollection = "HLTNav_Summary_ESDSlimmed"
    esdSlim.PrimaryInputCollection = inputCollection
    esdSlim.AllOutputContainers = possible_keys
    esdSlim.KeepFailedBranched = True
    esdSlim.KeepOnlyFinalFeatures = False
    ca.addEventAlgo(esdSlim)
    log.info("Producing ESD Slimmed Trigger Navigation Collection. Reading {} and writing {}".format(esdSlim.PrimaryInputCollection, esdSlim.OutputCollection))

    if esdSlim.OutputCollection not in possible_keys:
      log.error("Producing a collection {} which is not listed in 'possible_keys'! Add this here too.".format(esdSlim.OutputCollection))

  if ConfigFlags.Output.doWriteAOD or rec.doWriteAOD():
    aodSlim = CompFactory.TrigNavSlimmingMTAlg('TrigNavSlimmingMTAlg_AOD')
    aodSlim.TrigDecisionTool = tdt
    aodSlim.OutputCollection = "HLTNav_Summary_AODSlimmed"
    aodSlim.PrimaryInputCollection = inputCollection
    aodSlim.AllOutputContainers = possible_keys
    #
    if ConfigFlags.Trigger.AODEDMSet == "AODFULL" or ConfigFlags.Trigger.AODEDMSet == "AODRun3_LARGE":
      aodSlim.KeepFailedBranched = True
      aodSlim.KeepOnlyFinalFeatures = False
      log.info("Producing AOD LARGE Slimmed Trigger Navigation Collection. Reading {} and writing {}".format(aodSlim.PrimaryInputCollection, aodSlim.OutputCollection))
    else:
      aodSlim.KeepFailedBranched = False
      aodSlim.KeepOnlyFinalFeatures = True
      log.info("Producing AOD SMALL Trigger Navigation Collection. Reading {} and writing {}".format(aodSlim.PrimaryInputCollection, aodSlim.OutputCollection))
    ca.addEventAlgo(aodSlim)

    if aodSlim.OutputCollection not in possible_keys:
      log.error("Producing a collection {} which is not listed in 'possible_keys'! Add this here too.".format(esdSlim.OutputCollection))

  return ca
