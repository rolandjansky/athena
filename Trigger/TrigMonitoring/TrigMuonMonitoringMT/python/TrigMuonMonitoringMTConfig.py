#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from TrigMuonMonitoringMT.L1MuonMonConfig import L1MuonMonConfig
from TrigMuonMonitoringMT.L2MuonSAMonConfig import L2MuonSAMonConfig
from TrigMuonMonitoringMT.L2MuonSAIOMonConfig import L2MuonSAIOMonConfig
from TrigMuonMonitoringMT.L2muCombMonConfig import L2muCombMonConfig
from TrigMuonMonitoringMT.L2OverlapRemoverMonConfig import L2OverlapRemoverMonConfig
from TrigMuonMonitoringMT.EFMuonMonConfig import EFMuonMonConfig
from TrigMuonMonitoringMT.TrigMuonEfficiencyMonConfig import TrigMuonEfficiencyMonTTbarConfig, TrigMuonEfficiencyMonZTPConfig
from TrigMuonMonitoringMT.MuonTriggerCountMTConfig import MuonTriggerCountMTConfig


def TrigMuonMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMuonMonitoringMTCfg')

    L1MuonMonConfig(helper)
    L2MuonSAMonConfig(helper)
    L2MuonSAIOMonConfig(helper)
    L2muCombMonConfig(helper)
    L2OverlapRemoverMonConfig(helper)
    EFMuonMonConfig(helper)
    TrigMuonEfficiencyMonTTbarConfig(helper)
    TrigMuonEfficiencyMonZTPConfig(helper)
    MuonTriggerCountMTConfig(helper)

    return helper.result()
