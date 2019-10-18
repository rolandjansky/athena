#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


def L1MuonMonConfig(helper):
    
    from TrigMuonMonitoringMT.TrigMuonMonitoringMTConf import L1MuonMonMT
    monAlg = helper.addAlgorithm(L1MuonMonMT,'L1MuonMonMT')


    histGroup = helper.addGroup(monAlg, 'L1MuonMonMT', 'HLT/MuonMon/L1Muon')


    import ROOT
    histGroup.defineHistogram('l1Thres',title='L1Muon Thresholds;L1 threshold number;Events', type='TH1F', path='',xbins=6,xmin=1.0,xmax=7.0)
    histGroup.defineHistogram('l1Eta',title='L1Muon Eta;#eta;Events', type='TH1F', path='',xbins=25,xmin=-3.0,xmax=3.0)
    histGroup.defineHistogram('l1Phi',title='L1Muon Phi;#phi;Events', type='TH1F', path='',xbins=25,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

    return
