#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


def L1MuonMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    monAlg = helper.addAlgorithm(CompFactory.L1MuonMonMT,'L1MuonMonMT')


    histGroup = helper.addGroup(monAlg, 'L1MuonMonMT', 'HLT/MuonMon/')


    import ROOT
    histGroup.defineHistogram('l1Thres',title='L1Muon Thresholds;L1 threshold number;Events', type='TH1F', path='L1Muon',xbins=6,xmin=1.0,xmax=7.0)
    histGroup.defineHistogram('l1Eta',title='L1Muon Eta;#eta;Events', type='TH1F', path='L1Muon',xbins=25,xmin=-3.0,xmax=3.0)
    histGroup.defineHistogram('l1Phi',title='L1Muon Phi;#phi;Events', type='TH1F', path='L1Muon',xbins=25,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())
    histGroup.defineHistogram('l1Eta,l1Phi;L1Muon_Eta_vs_Phi',title='L1Muon Eta vs Phi;#eta;#phi;Events', type='TH2F', path='etaphi2D',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

    return
