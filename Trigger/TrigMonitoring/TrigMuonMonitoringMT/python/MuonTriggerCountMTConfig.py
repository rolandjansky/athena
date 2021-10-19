#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration



def MuonTriggerCountMTConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'TriggerCount'
    Chains =  ['HLT_mu6_L1MU5VF', 'HLT_mu26_ivarmedium_L1MU14FCH', 'HLT_mu50_L1MU14FCH', 'HLT_mu60_0eta105_msonly_L1MU14FCH', 'HLT_2mu14_L12MU8F', 'HLT_mu22_mu8noL1_L1MU14FCH']

    monAlg = helper.addAlgorithm(CompFactory.MuonTriggerCountMT,'MuonTriggerCountMT')
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    monAlg.MonitoredChains = Chains 
    monAlg.Group = GroupName
    

    histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/TriggerCount')

    histGroup.defineHistogram('TriggerCount;Monitoring_Chain',
                              title='Monitoring Chain Count;;Events',
                              type='TH1I',path='', xlabels=Chains)
