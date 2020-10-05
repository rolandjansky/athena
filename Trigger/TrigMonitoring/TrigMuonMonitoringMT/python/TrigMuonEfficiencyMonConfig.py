#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
import math
import ROOT


def regex(pat):
    if 'cached_regex' not in globals():
        global cached_regex
        cached_regex = {}
    if pat not in cached_regex:
        cached_regex.update({pat:re.compile(pat)})
    return cached_regex[pat]


##
# @brief A special configuration for ttbar samples, where the cut on m_mumu is loosened to improve the acceptance.
# This is useless for data samples because the ttbar process has very small contribution.
def TrigMuonEfficiencyMonTTbarConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    # To do: add multi-muon chain and msonly chain
    Chains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20']

    for chain in Chains:
        monAlg = helper.addAlgorithm(CompFactory.TrigMuonEfficiencyMonMT,'TrigMuEff_ttbar_'+chain)

        monAlg.EventTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.TagTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.Method = 'TTbarTagAndProbe'
        monAlg.MuonType = ROOT.xAOD.Muon_v1.Combined
        monAlg.MonitoredChains = [chain]
        threshold, level1 = regex('HLT_mu([0-9]+).*_(L1MU[0-9]+)').match(chain).groups()
        monAlg.L1Seeds = [regex('L1MU').sub('L1_MU', level1)]
        monAlg.Thresholds = [float(threshold)]
        monAlg.Group = 'Eff_ttbar_'+chain
        
        GroupName = 'Eff_ttbar_'+chain
        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/Efficiency/ttbar/'+chain)

        if "ivar" not in chain:
            monAlg.doEFIso = False

        defineEfficiencyHistograms(monAlg, histGroup, GroupName, chain)

    return


def TrigMuonEfficiencyMonZTPConfig(helper):

    from AthenaConfiguration.ComponentFactory import CompFactory

    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    # To do: add multi-muon chain and msonly chain
    Chains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20']

    for chain in Chains:
        monAlg = helper.addAlgorithm(CompFactory.TrigMuonEfficiencyMonMT,'TrigMuEff_ZTP_'+chain)

        monAlg.EventTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.TagTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.Method = 'ZTagAndProbe'
        monAlg.MuonType = ROOT.xAOD.Muon_v1.Combined
        monAlg.MonitoredChains = [chain]
        threshold, level1 = regex('HLT_mu([0-9]+).*_(L1MU[0-9]+)').match(chain).groups()
        monAlg.L1Seeds = [regex('L1MU').sub('L1_MU', level1)]
        monAlg.Thresholds = [float(threshold)]
        monAlg.Group = 'Eff_ZTP_'+chain
        
        GroupName = 'Eff_ZTP_'+chain
        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/Efficiency/ZTP/'+chain)

        if "ivar" not in chain:
            monAlg.doEFIso = False

        defineEfficiencyHistograms(monAlg, histGroup, GroupName, chain)

    return



def defineEfficiencyHistograms(monAlg, histGroup, GroupName, chain):

    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):
        histGroup.defineHistogram(GroupName+'_'+xvariable+';'+xvariable,
                                  title='All offline combined muon '+chain+';'+xlabel+';Events',
                                  type='TH1F',path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_L1pass,'+GroupName+'_'+xvariable+';EffL1MU_'+xvariable+'_wrt_Probe',
                                  title='L1MU Efficiency '+chain+';'+xlabel+';Efficiency',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+xvariable+';EffL2SA_'+xvariable+'_wrt_Upstream',
                                  title='L2MuonSA Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_L1pass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+xvariable+';EffL2SA_'+xvariable+'_wrt_offlineCB',
                                  title='L2MuonSA Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+xvariable+';EffL2CB_'+xvariable+'_wrt_Upstream',
                                  title='L2muComb Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_L2SApass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+xvariable+';EffL2CB_'+xvariable+'_wrt_offlineCB',
                                  title='L2muComb Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+xvariable+';EffEFSA_'+xvariable+'_wrt_Upstream',
                                  title='EFSA Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_L2CBpass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+xvariable+';EffEFSA_'+xvariable+'_wrt_offlineCB',
                                  title='EFSA Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+xvariable+';EffEFSA_'+xvariable+'_wrt_offlineCB_passedL2SA',
                                  title='EFSA Muon Efficiency passed L2SA '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_L2SApass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+xvariable+';EffEFCB_'+xvariable+'_wrt_Upstream',
                                  title='EFCB Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_EFSApass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+xvariable+';EffEFCB_'+xvariable+'_wrt_offlineCB',
                                  title='EFCB Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
        histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+xvariable+';EffEFCB_'+xvariable+'_wrt_offlineCB_passedL2CB',
                                  title='EFCB Muon Efficiency passed L2CB '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                  cutmask=GroupName+'_L2CBpass',
                                  type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
    
        if monAlg.doEFIso:
            histGroup.defineHistogram(GroupName+'_EFIsopass,'+GroupName+'_'+xvariable+';EffEFIso_'+xvariable+'_wrt_Upstream',
                                      title='EFIso Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_EFCBpass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)
    
            histGroup.defineHistogram(GroupName+'_EFIsopass,'+GroupName+'_'+xvariable+';EffEFIso_'+xvariable+'_wrt_offlineCB',
                                      title='EFIso Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)


    defineEachStepHistograms('muPt', 'p_{T} [GeV]', 50, 0.0, 100.)
    defineEachStepHistograms('muEta', '#eta', 30, -3.0, 3.0)
    defineEachStepHistograms('muPhi', '#phi', 30, -math.pi, math.pi)
    defineEachStepHistograms('averageMu', 'average pileup', 4, 0., 80.)


    histGroup.defineHistogram(GroupName+'_invmass;invmass',
                              title='invariant mass of tag & probe muon '+chain+';inv mass [GeV];Events',
                              type='TH1F',path='',xbins=40,xmin=0.,xmax=200.)
    
