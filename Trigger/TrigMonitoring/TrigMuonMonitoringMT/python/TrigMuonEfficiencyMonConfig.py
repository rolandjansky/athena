#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re
import math
import ROOT

def TrigMuonEfficiencyMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    Chains = ['HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20', 'HLT_mu6_L1MU6']

    cached_regex = {}
    def regex(pat):
        if pat not in cached_regex:
            cached_regex.update({pat:re.compile(pat)})
        return cached_regex[pat]

    
    for chain in Chains:
        monAlg = helper.addAlgorithm(CompFactory.TrigMuonEfficiencyMonMT,'TrigMuEff_'+chain)

        monAlg.EventTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.TagTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.Method = 'TagAndProbe'
        monAlg.MuonType = ROOT.xAOD.Muon_v1.Combined
        monAlg.MonitoredChains = [chain]
        monAlg.L1Seeds = ['L1_'+chain.split('L1')[1]]
        monAlg.Thresholds = [float(regex('HLT_mu').sub('',regex('HLT_mu[0-9]+').match(chain).group()))]
        monAlg.Group = 'Eff_'+chain
        
        GroupName = 'Eff_'+chain
        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/Efficiency/'+chain)

        def defineEfficiencyHistograms(variable, xlabel, xbins, xmin, xmax):
            histGroup.defineHistogram(GroupName+'_'+variable,
                                      title='All offline combined muon '+chain+';'+xlabel+';Events',
                                      type='TH1F',path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L1pass,'+GroupName+'_'+variable+';'+'EffL1MU_'+variable+'_wrt_Probe',
                                      title='L1MU Efficiency '+chain+';'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+variable+';EffL2SA_'+variable+'_wrt_Upstream',
                                      title='L2MuonSA Efficiency '+chain+';'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L1pass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+variable+';EffL2CB_'+variable+'_wrt_Upstream',
                                      title='L2muComb Efficiency '+chain+';'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2SApass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFpass,'+GroupName+'_'+variable+';EffEF_'+variable+'_wrt_Upstream',
                                      title='EF Muon Efficiency '+chain+';'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2CBpass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)


        defineEfficiencyHistograms('muPt', 'p_{T} [GeV]', 30, 0.0, 60)
        defineEfficiencyHistograms('muEta', '#eta', 30, -3.0, 3.0)
        defineEfficiencyHistograms('muPhi', '#phi', 30, -math.pi, math.pi)


    return
