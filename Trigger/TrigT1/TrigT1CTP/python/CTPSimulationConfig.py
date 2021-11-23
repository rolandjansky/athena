# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
def CTPSimulationCfg(flags):
    from AthenaCommon.Logging import logging
    log = logging.getLogger("CTPMCSimulationCfg")
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.LVL1CTP.CTPSimulation("CTPSimulation",
                                                        #DoL1Topo    = not flags.Trigger.enableL1Phase1, #TODO confirm that this setting exists actually
                                                        DoL1CaloLegacy = flags.Trigger.enableL1CaloLegacy,
                                                        #TODO enable when input are also simulatedDetectors (and remove message)
                                                        jFexJetInput = "",
                                                        jFexLJetInput = "",
                                                        gFexJetInput =  "",
                                                        gFexMETNCInput = "",
                                                        gFexMETRhoInput = "",
                                                        gFexMETJwoJInput = "",
                                                        eFexClusterInput = "",
                                                        eFexTauInput = "",
                                                        TopoInput = "L1TopoToCTPLocation",
                                                        LegacyTopoInput = "L1TopoLegacyToCTPLocation"
                                                        ))
    log.info("Not all part of CTP simulation are enabled yet")
    if flags.Trigger.enableL1CaloLegacy:
        roib = CompFactory.ROIB.RoIBuilder("RoIBuilder",
                                            DoCalo = flags.Trigger.enableL1CaloLegacy,
                                            DoMuon = False)   # not needed for L1MuonPhase1
        acc.addEventAlgo(roib)


    from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
    acc.merge(TriggerHistSvcConfig(flags))
    return acc
