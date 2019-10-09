#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


def L2MuonSAMonConfig(helper):
    
    from TrigMuonMonitoringMT.TrigMuonMonitoringMTConf import L2MuonSAMonMT
    monAlg = helper.addAlgorithm(L2MuonSAMonMT,'L2MuonSAMonMT')


    histGroup = helper.addGroup(monAlg, 'L2MuonSAMonMT', 'HLT/MuonMon/L2MuonSA')


    import ROOT
    histGroup.defineHistogram('saPt',title='L2MuonSA Pt;p_{T} [GeV];Events', type='TH1F', path='',xbins=25,xmin=-60.0,xmax=60.0)
    histGroup.defineHistogram('saEta',title='L2MuonSA Eta;#eta;Events', type='TH1F', path='',xbins=25,xmin=-3.0,xmax=3.0)
    histGroup.defineHistogram('saPhi',title='L2MuonSA Phi;#phi;Events', type='TH1F', path='',xbins=25,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

    return
