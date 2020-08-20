#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from TrigMuonMonitoringMT.L1MuonMonConfig import L1MuonMonConfig
from TrigMuonMonitoringMT.L2MuonSAMonConfig import L2MuonSAMonConfig
from TrigMuonMonitoringMT.L2muCombMonConfig import L2muCombMonConfig
from TrigMuonMonitoringMT.EFMuonMonConfig import EFMuonMonConfig
from TrigMuonMonitoringMT.TrigMuonEfficiencyMonConfig import TrigMuonEfficiencyMonConfig


def TrigMuonMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMuonMonitoringMTCfg')

    L1MuonMonConfig(helper)
    L2MuonSAMonConfig(helper)
    L2muCombMonConfig(helper)
    EFMuonMonConfig(helper)
    TrigMuonEfficiencyMonConfig(helper)

    return helper.result()
