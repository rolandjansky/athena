#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

def L2OverlapRemoverMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'L2SAOR'

    monAlg = helper.addAlgorithm(CompFactory.L2OverlapRemoverMonMT,'L2OverlapRemoverMonMT')
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    monAlg.MonitoredChains = ['HLT_2mu14_L12MU10']
    monAlg.Group = GroupName


    # configration for specific chain
    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/L2OverlapRemover/'+chain)
    
    
        # basic EDM variables
        histGroup.defineHistogram(chain+'_NInMuon;L2SAOR_NInMuon',
                                  title='L2SA OverlapRemover number of input muons '+chain+';number of input muons;Events', 
                                  type='TH1I', path='',xbins=10,xmin=0,xmax=10)

        histGroup.defineHistogram(chain+'_NOutMuon;L2SAOR_NOutMuon',
                                  title='L2SA OverlapRemover number of output muons '+chain+';number of output muons;Events', 
                                  type='TH1I', path='',xbins=10,xmin=0,xmax=10)

        histGroup.defineHistogram(chain+'_NRemovedMuon;L2SAOR_NRemovedMuon',
                                  title='L2SA OverlapRemover number of removed muons '+chain+';number of removed muons;Events', 
                                  type='TH1I', path='',xbins=10,xmin=0,xmax=10)

        histGroup.defineHistogram(chain+'_dR;L2SAOR_dR',
                                  title='L2SA OverlapRemover dR '+chain+';dR;Events', 
                                  type='TH1F', path='',xbins=105,xmin=0.,xmax=3.5)

        histGroup.defineHistogram(chain+'_invMass;L2SAOR_invMass',
                                  title='L2SA OverlapRemover invariant mass '+chain+';invariant mass [GeV];Events', 
                                  type='TH1F', path='',xbins=200,xmin=0.,xmax=100.)

        histGroup.defineHistogram(chain+'_invMass,'+chain+'_dR;L2SAOR_invMass_vs_dR',
                                  title='L2SA OverlapRemover invariant mass vs dR '+chain+';invariant mass [GeV];dR', 
                                  type='TH2F', path='',xbins=100,xmin=0.,xmax=200.,ybins=105,ymin=0.,ymax=3.5)

        histGroup.defineHistogram(chain+'_dRLog10;L2SAOR_dRLog10',
                                  title='L2SA OverlapRemover dR (Log10) '+chain+';dR (Log10);Events', 
                                  type='TH1F', path='',xbins=100,xmin=-4.,xmax=1.)

        histGroup.defineHistogram(chain+'_invMass;L2SAOR_invMass',
                                  title='L2SA OverlapRemover invariant mass (Log10) '+chain+';invariant mass (Log10) [GeV];Events', 
                                  type='TH1F', path='',xbins=140,xmin=-4.,xmax=3.)

        histGroup.defineHistogram(chain+'_RemovedEta,'+chain+'_RemovedPhi;L2SAOR_RemovedEta_vs_RemovedPhi',
                                  title='L2SA OverlapRemover Eta vs Phi of removed muons '+chain+';Eta;Phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7,ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_RemovedPt;L2SAOR_RemovedPt',
                                  title='L2SA OverlapRemover Pt of removed muons '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_OverlapInsonsistent;L2SAOR_OverlapInsonsistent',
                                  title='L2SA Overlap Insonsistent '+chain+';;Events',
                                  type='TH1I', path='',xbins=2,xmin=0,xmax=2)

    return
