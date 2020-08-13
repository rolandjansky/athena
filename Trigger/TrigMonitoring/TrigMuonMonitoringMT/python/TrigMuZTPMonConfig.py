#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
import math
import ROOT

def TrigMuZTPMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    # To do: add multi-muon chain and msonly chain
    Chains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20']

    cached_regex = {}
    def regex(pat):
        if pat not in cached_regex:
            cached_regex.update({pat:re.compile(pat)})
        return cached_regex[pat]

    
    for chain in Chains:
        monAlg = helper.addAlgorithm(CompFactory.TrigMuZTPMonMT,'TrigMuZTP_'+chain)

        monAlg.EventTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.TagTrigger = 'HLT_mu26_ivarmedium_L1MU20'
        monAlg.MuonType = ROOT.xAOD.Muon_v1.Combined
        monAlg.MonitoredChains = [chain]
        monAlg.L1Seeds = ['L1_'+chain.split('L1')[1]]
        monAlg.Thresholds = [float(regex('HLT_mu').sub('',regex('HLT_mu[0-9]+').match(chain).group()))]
        monAlg.Group = 'MuZTP_'+chain
        
        GroupName = 'MuZTP_'+chain
        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/MuZTP/'+chain)

        if "ivar" not in chain:
            monAlg.doEFIso = False

        histGroup.defineHistogram(GroupName+'_invmass',
                                  title='Invariant mass '+chain+';Invariant mass;Events',
                                  type='TH1F',path='',xbins=20,xmin=0.,xmax=120.)


        # 1D efficiency
        def defineEfficiencyHistograms(variable, xlabel, xbins, xmin, xmax):
            histGroup.defineHistogram(GroupName+'_'+variable,
                                      title='All offline combined muon '+chain+';'+xlabel+';Events',
                                      type='TH1F',path='',xbins=xbins,xmin=xmin,xmax=xmax)

            if "Eta" not in variable:
                histGroup.defineHistogram(GroupName+'_'+variable+';'+GroupName+'_'+variable+'_Endcap',
                                          title='All offline combined muon Endcap '+chain+';'+xlabel+';Events',
                                          cutmask=GroupName+'_isEndcap',
                                          type='TH1F',path='',xbins=xbins,xmin=xmin,xmax=xmax)

                histGroup.defineHistogram(GroupName+'_'+variable+';'+GroupName+'_'+variable+'_Barrel',
                                          title='All offline combined muon Barrel '+chain+';'+xlabel+';Events',
                                          cutmask=GroupName+'_isBarrel',
                                          type='TH1F',path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L1pass,'+GroupName+'_'+variable+';EffL1MU_'+variable+'_wrt_Probe',
                                      title='L1MU Efficiency '+chain+';'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+variable+';EffL2SA_'+variable+'_wrt_Upstream',
                                      title='L2MuonSA Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L1pass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+variable+';EffL2SA_'+variable+'_wrt_offlineCB',
                                      title='L2MuonSA Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+variable+';EffL2CB_'+variable+'_wrt_Upstream',
                                      title='L2muComb Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2SApass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+variable+';EffL2CB_'+variable+'_wrt_offlineCB',
                                      title='L2muComb Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+variable+';EffEFSA_'+variable+'_wrt_Upstream',
                                      title='EFSA Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2CBpass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+variable+';EffEFSA_'+variable+'_wrt_offlineCB',
                                      title='EFSA Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+variable+';EffEFSA_'+variable+'_wrt_offlineCB_passedL2SA',
                                      title='EFSA Muon Efficiency passed L2SA '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2SApass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+variable+';EffEFCB_'+variable+'_wrt_Upstream',
                                      title='EFCB Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_EFSApass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+variable+';EffEFCB_'+variable+'_wrt_offlineCB',
                                      title='EFCB Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+variable+';EffEFCB_'+variable+'_wrt_offlineCB_passedL2CB',
                                      title='EFCB Muon Efficiency passed L2CB '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                      cutmask=GroupName+'_L2CBpass',
                                      type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

            if monAlg.doEFIso:
                histGroup.defineHistogram(GroupName+'_EFIsopass,'+GroupName+'_'+variable+';EffEFIso_'+variable+'_wrt_Upstream',
                                          title='EFIso Muon Efficiency '+chain+' wrt Upstream;'+xlabel+';Efficiency',
                                          cutmask=GroupName+'_EFCBpass',
                                          type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)

                histGroup.defineHistogram(GroupName+'_EFIsopass,'+GroupName+'_'+variable+';EffEFIso_'+variable+'_wrt_offlineCB',
                                          title='EFIso Muon Efficiency '+chain+' wrt offlineCB;'+xlabel+';Efficiency',
                                          type='TEfficiency', path='',xbins=xbins,xmin=xmin,xmax=xmax)


        # 2D efficiency



        # efficiency used a few bins for small stat
        def defineEfficiencyHistogramsAFewBins(variable, xlabels):
            histGroup.defineHistogram(GroupName+'_'+variable,
                                      title='All offline combined muon '+chain+';;Events',
                                      type='TH1F',path='', xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            histGroup.defineHistogram(GroupName+'_L1pass,'+GroupName+'_'+variable+';EffL1MU_'+variable+'_'+str(len(xlabels))+'bins_wrt_Probe',
                                      title='L1MU Efficiency '+chain+';;Efficiency',
                                      type='TEfficiency', path='', xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            histGroup.defineHistogram(GroupName+'_L2SApass,'+GroupName+'_'+variable+';EffL2SA_'+variable+'_'+str(len(xlabels))+'bins_wrt_offlineCB',
                                      title='L2MuonSA Efficiency '+chain+' wrt offlineCB;;Efficiency',
                                      type='TEfficiency', path='',xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            histGroup.defineHistogram(GroupName+'_L2CBpass,'+GroupName+'_'+variable+';EffL2CB_'+variable+'_'+str(len(xlabels))+'bins_wrt_offlineCB',
                                      title='L2muComb Efficiency '+chain+' wrt offlineCB;;Efficiency',
                                      type='TEfficiency', path='',xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            histGroup.defineHistogram(GroupName+'_EFSApass,'+GroupName+'_'+variable+';EffEFSA_'+variable+'_'+str(len(xlabels))+'bins_wrt_offlineCB',
                                      title='EFSA Muon Efficiency '+chain+' wrt offlineCB;;Efficiency',
                                      type='TEfficiency', path='',xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            histGroup.defineHistogram(GroupName+'_EFCBpass,'+GroupName+'_'+variable+';EffEFCB_'+variable+'_'+str(len(xlabels))+'bins_wrt_offlineCB',
                                      title='EFCB Muon Efficiency '+chain+' wrt offlineCB;;Efficiency',
                                      type='TEfficiency', path='',xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))

            if monAlg.doEFIso:
                histGroup.defineHistogram(GroupName+'_EFIsopass,'+GroupName+'_'+variable+';EffEFIso_'+variable+'_'+str(len(xlabels))+'bins_wrt_offlineCB',
                                          title='EFIso Muon Efficiency '+chain+' wrt offlineCB;;Efficiency',
                                          type='TEfficiency', path='',xlabels=xlabels, xbins=len(xlabels), xmin=0, xmax=len(xlabels))


        defineEfficiencyHistograms('muPt', 'p_{T} [GeV]', 30, 0., 60.)
        defineEfficiencyHistograms('muEta', '#eta', 30, -3.0, 3.0)
        defineEfficiencyHistograms('muPhi', '#phi', 30, -math.pi, math.pi)
        defineEfficiencyHistogramsAFewBins('muRegion', ["Barrel", "Endcap"])


    return
