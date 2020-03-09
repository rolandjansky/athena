#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


def TrigMuonEfficiencyMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    Chains = ['HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20', 'HLT_mu6_L1MU6']
    
    for chain in Chains:
        monAlg = helper.addAlgorithm(CompFactory.TrigMuonEfficiencyMonMT,'TrigMuEff_'+chain)

        monAlg.EventTrigger = ''
        monAlg.MeasuredTrigger = chain
        monAlg.L1Seed = 'L1_'+chain.split('L1')[1]
        monAlg.Group = 'Eff_'+chain
        
        GroupName = 'Eff_'+chain
        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/Efficiency/'+chain)

    
        histGroup.defineHistogram(GroupName+'_muPt',title='All offline combined muon '+chain+';p_{T} [GeV];Events', type='TH1F', path='',xbins=30,xmin=0.0,xmax=60.0)
        histGroup.defineHistogram(GroupName+'_muPt,'+GroupName+'_muPt_L1passed',title='L1MU Efficiency '+chain+';p_{T} [GeV];Efficiency', type='TProfile', path='',xbins=30,xmin=0.0,xmax=60.0)
        histGroup.defineHistogram(GroupName+'_muPt,'+GroupName+'_muPt_SApassed',title='L2MuonSA Efficiency '+chain+';p_{T} [GeV];Efficiency', type='TProfile', path='',xbins=30,xmin=0.0,xmax=60.0)
        histGroup.defineHistogram(GroupName+'_muPt,'+GroupName+'_muPt_CBpassed',title='L2muComb Efficiency '+chain+';p_{T} [GeV];Efficiency', type='TProfile', path='',xbins=30,xmin=0.0,xmax=60.0)
        histGroup.defineHistogram(GroupName+'_muPt,'+GroupName+'_muPt_EFpassed',title='EF Muon Efficiency '+chain+';p_{T} [GeV];Efficiency', type='TProfile', path='',xbins=30,xmin=0.0,xmax=60.0)


    return
