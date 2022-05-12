#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from TrigMuonMonitoring.L1MuonMonConfig import L1MuonMonConfig
from TrigMuonMonitoring.L2MuonSAMonConfig import L2MuonSAMonConfig
from TrigMuonMonitoring.L2MuonSAIOMonConfig import L2MuonSAIOMonConfig
from TrigMuonMonitoring.L2muCombMonConfig import L2muCombMonConfig
from TrigMuonMonitoring.L2OverlapRemoverMonConfig import L2OverlapRemoverMonConfig
from TrigMuonMonitoring.EFMuonMonConfig import EFMuonMonConfig
from TrigMuonMonitoring.TrigMuonEfficiencyMonConfig import TrigMuonEfficiencyMonTTbarConfig, TrigMuonEfficiencyMonZTPConfig
from TrigMuonMonitoring.MuonTriggerCountConfig import MuonTriggerCountConfig
from TrigMuonMonitoring.TrigMuonTruthMonConfig import TrigMuonTruthMonConfig


def TrigMuonMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMuonMonitoringCfg')

    L1MuonMonConfig(helper)
    L2MuonSAMonConfig(helper)
    L2MuonSAIOMonConfig(helper)
    L2muCombMonConfig(helper)
    L2OverlapRemoverMonConfig(helper)
    EFMuonMonConfig(helper)
    TrigMuonEfficiencyMonTTbarConfig(helper)
    TrigMuonEfficiencyMonZTPConfig(helper)
    MuonTriggerCountConfig(helper)
    if inputFlags.Input.isMC:
        TrigMuonTruthMonConfig(helper)

    return helper.result()
