#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

def L2OverlapRemoverMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'OR'

    monAlg = helper.addAlgorithm(CompFactory.L2OverlapRemoverMonMT,'L2OverlapRemoverMonMT')
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    monAlg.MonitoredChains = ['HLT_2mu14_L12MU10']
    monAlg.Group = GroupName


    # configration for specific chain
    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/L2OverlapRemover/'+chain)
        defineOverlapRemoverHistograms(histGroup, GroupName, chain, 'L2SA')
        defineOverlapRemoverHistograms(histGroup, GroupName, chain, 'L2CB')
    
    return


def defineOverlapRemoverHistograms(histGroup, GroupName, chain, trigstep):    

    histGroup.defineHistogram(trigstep+'_'+chain+'_NInMuon;'+trigstep+GroupName+'_NInMuon',
                              title=trigstep+' OverlapRemover number of input muons '+chain+';number of input muons;Events', 
                              type='TH1I', path='',xbins=10,xmin=0,xmax=10)

    histGroup.defineHistogram(trigstep+'_'+chain+'_NOutMuon;'+trigstep+GroupName+'_NOutMuon',
                              title=trigstep+' OverlapRemover number of output muons '+chain+';number of output muons;Events', 
                              type='TH1I', path='',xbins=10,xmin=0,xmax=10)

    histGroup.defineHistogram(trigstep+'_'+chain+'_NRemovedMuon;'+trigstep+GroupName+'_NRemovedMuon',
                              title=trigstep+' OverlapRemover number of removed muons '+chain+';number of removed muons;Events', 
                              type='TH1I', path='',xbins=10,xmin=0,xmax=10)

    histGroup.defineHistogram(trigstep+'_'+chain+'_dR;'+trigstep+GroupName+'_dR',
                              title=trigstep+' OverlapRemover dR '+chain+';dR;Events', 
                              type='TH1F', path='',xbins=105,xmin=0.,xmax=3.5)

    histGroup.defineHistogram(trigstep+'_'+chain+'_invMass;'+trigstep+GroupName+'_invMass',
                              title=trigstep+' OverlapRemover invariant mass '+chain+';invariant mass [GeV];Events', 
                              type='TH1F', path='',xbins=200,xmin=0.,xmax=100.)

    histGroup.defineHistogram(trigstep+'_'+chain+'_invMass,'+trigstep+'_'+chain+'_dR;'+trigstep+GroupName+'_invMass_vs_dR',
                              title=trigstep+' OverlapRemover invariant mass vs dR '+chain+';invariant mass [GeV];dR', 
                              type='TH2F', path='',xbins=100,xmin=0.,xmax=200.,ybins=105,ymin=0.,ymax=3.5)

    histGroup.defineHistogram(trigstep+'_'+chain+'_dRLog10;'+trigstep+GroupName+'_dRLog10',
                              title=trigstep+' OverlapRemover dR (Log10) '+chain+';dR (Log10);Events', 
                              type='TH1F', path='',xbins=100,xmin=-4.,xmax=1.)

    histGroup.defineHistogram(trigstep+'_'+chain+'_invMassLog10;'+trigstep+GroupName+'_invMassLog10',
                              title=trigstep+' OverlapRemover invariant mass (Log10) '+chain+';invariant mass (Log10) [GeV];Events', 
                              type='TH1F', path='',xbins=140,xmin=-4.,xmax=3.)

    histGroup.defineHistogram(trigstep+'_'+chain+'_RemovedEta,'+trigstep+'_'+chain+'_RemovedPhi;'+trigstep+GroupName+'_RemovedEta_vs_RemovedPhi',
                              title=trigstep+' OverlapRemover Eta vs Phi of removed muons '+chain+';Eta;Phi', 
                              type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7,ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

    histGroup.defineHistogram(trigstep+'_'+chain+'_RemovedPt;'+trigstep+GroupName+'_RemovedPt',
                              title=trigstep+' OverlapRemover Pt of removed muons '+chain+';p_{T} [GeV];Events', 
                              type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

    histGroup.defineHistogram(trigstep+'_'+chain+'_OverlapInsonsistent;'+trigstep+GroupName+'_OverlapInsonsistent',
                              title=trigstep+' Overlap Insonsistent '+chain+';;Events',
                              type='TH1I', path='',xbins=2,xmin=0,xmax=2)
