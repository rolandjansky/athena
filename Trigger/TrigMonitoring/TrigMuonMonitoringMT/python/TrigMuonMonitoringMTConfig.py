#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from TrigMuonMonitoringMT.L2MuonSAMonConfig import L2MuonSAMonConfig
from TrigMuonMonitoringMT.L1MuonMonConfig import L1MuonMonConfig
from TrigMuonMonitoringMT.TrigMuonEfficiencyMonConfig import TrigMuonEfficiencyMonConfig


def TrigMuonMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMuonMonitoringMTCfg')

    L1MuonMonConfig(helper)
    L2MuonSAMonConfig(helper)
    TrigMuonEfficiencyMonConfig(helper)

    return helper.result()
