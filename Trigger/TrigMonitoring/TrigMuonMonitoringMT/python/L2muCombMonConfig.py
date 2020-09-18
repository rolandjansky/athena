#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

def L2muCombMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'L2muComb'

    monAlg = helper.addAlgorithm(CompFactory.L2muCombMonMT,'L2muCombMonMT')
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    monAlg.MonitoredChains =  ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20']
    monAlg.Group = GroupName

    # configuration of etaphi2D and Ratio plots for non-specific chain
    histGroupNonSpecificChain = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/')

    histGroupNonSpecificChain.defineHistogram('L2CBEta,L2CBPhi;L2muComb_Eta_vs_Phi',
                                              title='L2muComb Eta vs Phi ;#eta;#phi',
                                              type='TH2F', path='etaphi2D',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

    histGroupNonSpecificChain.defineHistogram('L2CBAboveCut,LB;L2muComb_Over_OfflineCB_4GeV_Cut',
                                              title='L2muComb Over OfflineCB 4GeV Cut;LB;Ratio',
                                              type='TEfficiency', path='Ratio',xbins=400,xmin=1.,xmax=801.)


    # configration for specific chain
    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/L2muComb/'+chain)
    

        # basic EDM variables
        histGroup.defineHistogram(chain+'_Pt;L2muComb_Pt',
                                  title='L2muComb Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_Eta;L2muComb_Eta',
                                  title='L2muComb Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_Phi;L2muComb_Phi',
                                  title='L2muComb Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_Eta,'+chain+'_Phi;L2muComb_Eta_vs_Phi',
                                  title='L2muComb Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        #  eta vs. phi Combine success or failure, and matched or not offline muon
        histGroup.defineHistogram(chain+'_Eta,'+chain+'_Phi;L2muComb_Eta_vs_Phi_L2CB_success',
                                  title='L2muComb Eta vs Phi (L2CB reconstruction success) '+chain+';#eta;#phi', 
                                  cutmask=chain+'_L2CB_success',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_Eta,'+chain+'_Phi;L2muComb_Eta_vs_Phi_L2CBOFFmatching_failure',
                                  title='L2muComb Eta vs Phi (L2CB reconstruction success Off match failure) '+chain+';#eta;#phi', 
                                  cutmask=chain+'_L2CBOFFmatching_failure',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_saEta,'+chain+'_saPhi;L2MuonSA_Eta_vs_Phi_L2CB_failure',
                                  title='L2MuonSA Eta vs Phi (L2CB reconstruction failure L2SA Off match success) '+chain+';#eta;#phi', 
                                  cutmask=chain+'_L2CB_failure',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # L2MuonSA EDM variables in case of L2muComb failure
        histGroup.defineHistogram(chain+'_saPt;L2muComb_failed_saPt',
                                  title='L2MuonSA Pt (GeV/c) for muComb fails '+chain+';p_{T} [GeV];Events', 
                                  cutmask=chain+'_L2SA_success',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_saEta;L2muComb_failed_saEta',
                                  title='L2MuonSA Eta for muComb fails '+chain+';#eta;Events', 
                                  cutmask=chain+'_L2SA_success',
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_saPhi;L2muComb_failed_saPhi',
                                  title='L2MuonSA Phi for muComb fails '+chain+';#phi;Events', 
                                  cutmask=chain+'_L2SA_success',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())


        # comparison L2muComb vs L2MuonSA
        histGroup.defineHistogram(chain+'_ptratio_toSA;L2muComb_ptratio_toSA',
                                  title='L2muComb pt / L2MuonSA pt '+chain+';p_{T} ratio;Events', 
                                  type='TH1F', path='',xbins=140,xmin=-0.5,xmax=3.)

        histGroup.defineHistogram(chain+'_dR_toSA;L2muComb_dR_toSA',
                                  title='L2muComb delta R to L2MuonSA '+chain+';#DeltaR;Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)

        histGroup.defineHistogram(chain+'_dEta_toSA;L2muComb_dEta_toSA',
                                  title='L2muComb delta eta to L2MuonSA '+chain+';#Delta#eta;Events', 
                                  type='TH1F', path='',xbins=120,xmin=-0.2,xmax=0.2)

        histGroup.defineHistogram(chain+'_dPhi_toSA;L2muComb_dPhi_toSA',
                                  title='L2muComb delta phi to L2MuonSA '+chain+';#Delta#phi;Events', 
                                  type='TH1F', path='',xbins=120,xmin=-0.2,xmax=0.2)

        histGroup.defineHistogram(chain+'_MF_error;L2muComb_MF_error',
                                  title='L2muComb pointer to MuonFeature Error '+chain+';;Events',
                                  type='TH1I', path='',xbins=4,xmin=0,xmax=4,
                                  xlabels=["No error","Missing muSATrackLink","TrackLink and ElementLinnk matching error","SA reconstruction Error"])


        # ID variables
        histGroup.defineHistogram(chain+'_trkPt;L2muComb_trkPt',
                                  title='L2muComb Trk Pt (GeV/c) '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_trkEta;L2muComb_trkEta',
                                  title='L2muComb Trk Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_trkPhi;L2muComb_trkPhi',
                                  title='L2muComb Trk Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_trkZ0;L2muComb_trkZ0',
                                  title='L2muComb Trk z0 (mm) '+chain+';z0[mm];Events', 
                                  type='TH1F', path='',xbins=150,xmin=-300.,xmax=300.)

        histGroup.defineHistogram(chain+'_trkChi2;L2muComb_trkChi2',
                                  title='L2muComb Trk chi2 '+chain+';#chi^{2};Events', 
                                  type='TH1F', path='',xbins=50,xmin=0.,xmax=10.)


        # comparison L2muComb (ID track) vs L2MuonSA
        histGroup.defineHistogram(chain+'_ptratio_TrktoSA;L2muComb_ptratio_TrktoSA',
                                  title='L2muComb Trk  pt / L2MuonSA pt '+chain+';p_{T} ratio;Events', 
                                  type='TH1F', path='',xbins=140,xmin=-0.5,xmax=3.)

        histGroup.defineHistogram(chain+'_dR_TrktoSA;L2muComb_dR_TrktoSA',
                                  title='L2muComb delta R Trk to L2MuonSA '+chain+';#DeltaR;Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)

        histGroup.defineHistogram(chain+'_dEta_TrktoSA;L2muComb_dEta_TrktoSA',
                                  title='L2muComb delta eta Trk to L2MuonSA '+chain+';#Delta#eta;Events', 
                                  type='TH1F', path='',xbins=120,xmin=-0.2,xmax=0.2)

        histGroup.defineHistogram(chain+'_dPhi_TrktoSA;L2muComb_dPhi_TrktoSA',
                                  title='L2muComb delta phi Trk to L2MuonSA '+chain+';#Delta#phi;Events', 
                                  type='TH1F', path='',xbins=120,xmin=-0.2,xmax=0.2)


        # Comparison to Offline
        histGroup.defineHistogram(chain+'_dRmin;L2muComb_dR_toOffl',
                                  title='dR between L2muComb and Offline '+chain+';#DeltaR;Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=2.)

        histGroup.defineHistogram(chain+'_ptresol;L2muComb_ptresol_toOffl',
                                  title='L2muComb pT resolution wrt Offline '+chain+';p_{T} resol;Events', 
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(chain+'_offEta,'+chain+'_ptresol;L2muComb_ptresol_toOffl_eta',
                                  title='L2muComb pT resolution wrt Offline in eta '+chain+';#eta;p_{T} resol', 
                                  type='TH2F', path='',xbins=16,xmin=-3.2,xmax=3.2, ybins=100,ymin=-2.,ymax=2.)


        # Comparison HLT_Roi_L2SAMuon variables to Offline
        histGroup.defineHistogram(chain+'_L2SARoI_dR;L2MuonSA_L2SARoI_dR_toRecMuonCB',
                                  title='L2MuonSA L2SARoI dR wrt offline CB '+chain+';dR(L2SARoI vs offl CB);Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)

        histGroup.defineHistogram(chain+'_offEta,'+chain+'_L2SARoI_dEta;L2MuonSA_L2SARoI_dEta_vs_Eta_toRecMuonCB',
                                  title='L2MuonSA L2SARoI wrt Offline CB muon, d#eta '+chain+';offl CB #eta;d#eta(L2SARoI vs offl CB)', 
                                  type='TH2F', path='',xbins=54,xmin=-2.7,xmax=2.7, ybins=60,ymin=-0.3,ymax=0.3)

        histGroup.defineHistogram(chain+'_offEta,'+chain+'_L2SARoI_dPhi;L2MuonSA_L2SARoI_dPhi_vs_Eta_toRecMuonCB',
                                  title='L2MuonSA L2SARoI wrt Offline CB muon, d#phi '+chain+';offl CB #eta;d#phi(L2SARoI vs offl CB)', 
                                  type='TH2F', path='',xbins=54,xmin=-2.7,xmax=2.7, ybins=44,ymin=-0.2,ymax=0.2)

    return
