#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


def L2MuonSAMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    Chains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20']

    for chain in Chains:

        GroupName = 'L2MuonSA_'+chain

        monAlg = helper.addAlgorithm(CompFactory.L2MuonSAMonMT,'L2MuonSAMonMT_'+chain)
        monAlg.MonitoredChains = [chain]
        monAlg.Group = GroupName

        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/L2MuonSA/'+chain)
    
    
        import ROOT
        # basic EDM variables
        histGroup.defineHistogram(GroupName+'_Pt;L2MuonSA_Pt',
                                  title='L2MuonSA Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_Pt;L2MuonSA_Pt_Barrel',
                                  title='L2MuonSA Pt Barrel '+chain+';p_{T} [GeV];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_Pt;L2MuonSA_Pt_Endcap',
                                  title='L2MuonSA Pt Endcap '+chain+';p_{T} [GeV];Events',
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_Eta;L2MuonSA_Eta',
                                  title='L2MuonSA Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(GroupName+'_Phi;L2MuonSA_Phi',
                                  title='L2MuonSA Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_Phi;L2MuonSA_Phi_Barrel',
                                  title='L2MuonSA Phi Barrel '+chain+';#phi;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_Phi;L2MuonSA_Phi_Endcap',
                                  title='L2MuonSA Phi Endcap '+chain+';#phi;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_Eta,'+GroupName+'_Phi;L2MuonSA_Eta_vs_Phi',
                                  title='L2MuonSA Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())



        # position and superpoint
        histGroup.defineHistogram(GroupName+'_saddr;L2MuonSA_saddr',
                                  title='L2MuonSA station address '+chain+';address;Events',
                                  type='TH1I', path='',xbins=6,xmin=-1,xmax=5)

        histGroup.defineHistogram(GroupName+'_MDTpoints_z,'+GroupName+'_MDTpoints_r;L2MuonSA_MDTpoints_z_vs_r',
                                  title='L2MuonSA MDT superpoint Z vs R (mm) '+chain+';Z[mm];R[mm]', 
                                  type='TH2F', path='',xbins=200,xmin=-24000,xmax=24000, ybins=200,ymin=-14000,ymax=14000)



        # L1 RoI eta vs. phi in case mF failed
        histGroup.defineHistogram(GroupName+'_roiEta,'+GroupName+'_roiPhi;L2MuonSA_failed_L1_eta_vs_phi',
                                  title='L1 RoI Eta vs. Phi in case of L2MuonSA failure '+chain+';#eta;#phi', 
                                  cutmask=GroupName+'_mf_failure',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())



        # MuonFeatureDetails
        # process floe
        histGroup.defineHistogram(GroupName+'_proc_flow;L2MuonSA_proc_flow',
                                  title='L2MuonSA process flow '+chain+';;Events', 
                                  type='TH1I', path='',xbins=6,xmin=1,xmax=7,
                                  xlabels=["input","n L1 hits > 0","L1 emu ok at trigger layer","n MDT hits > 0 at middle layer","MDT fit ok at middle layer","MDT fit ok at >= 2 layers"])


        # RPC
        histGroup.defineHistogram(GroupName+'_RPC_Pad_N;L2MuonSA_RPC_Pad_N',
                                  title='L2MuonSA RPC number of hits '+chain+';RPC number of hits;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1I', path='',xbins=20,xmin=0,xmax=20)


        # TGC
        histGroup.defineHistogram(GroupName+'_TGC_Mid_rho_chi2;L2MuonSA_TGC_Mid_rho_chi2',
                                  title='L2MuonSA TGC big wheel rho fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_TGC_Mid_phi_chi2;L2MuonSA_TGC_Mid_phi_chi2',
                                  title='L2MuonSA TGC big wheel phi fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_TGC_Mid_rho_N;L2MuonSA_TGC_Mid_rho_N',
                                  title='L2MuonSA TGC big wheel number of hits in rho '+chain+';TGC BW rho nhits;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=20,xmin=0,xmax=20)

        histGroup.defineHistogram(GroupName+'_TGC_Mid_phi_N;L2MuonSA_TGC_Mid_phi_N',
                                  title='L2MuonSA TGC big wheel number of hits in phi '+chain+';TGC BW phi nhits;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=20,xmin=0,xmax=20)


        # MDT
        histGroup.defineHistogram(GroupName+'_MDT_Inn_fit_chi2;L2MuonSA_MDT_Inn_fit_chi2_barrel',
                                  title='L2MuonSA barrel MDT Inner station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_fit_chi2;L2MuonSA_MDT_Mid_fit_chi2_barrel',
                                  title='L2MuonSA barrel MDT Middle station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_Out_fit_chi2;L2MuonSA_MDT_Out_fit_chi2_barrel',
                                  title='L2MuonSA barrel MDT Outer station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_fit_chi2;L2MuonSA_MDT_Inn_fit_chi2_endcap',
                                  title='L2MuonSA endcap MDT Inner station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_fit_chi2;L2MuonSA_MDT_Mid_fit_chi2_endcap',
                                  title='L2MuonSA endcap MDT Middle station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_Out_fit_chi2;L2MuonSA_MDT_Out_fit_chi2_endcap',
                                  title='L2MuonSA endcap MDT Outer station fit chi2 '+chain+';chi2;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=10)

        histGroup.defineHistogram(GroupName+'_MDT_N;L2MuonSA_MDT_N_barrel',
                                  title='L2MuonSA barrel MDT number of hits '+chain+';MDT nhits;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_N;L2MuonSA_MDT_Inn_N_barrel',
                                  title='L2MuonSA barrel MDT Inner number of hits '+chain+';MDT nhits Inner;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_N;L2MuonSA_MDT_Mid_N_barrel',
                                  title='L2MuonSA barrel MDT Middle number of hits '+chain+';MDT nhits Middle;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Out_N;L2MuonSA_MDT_Out_N_barrel',
                                  title='L2MuonSA barrel MDT Outer number of hits '+chain+';MDT nhits Outer;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_N;L2MuonSA_MDT_N_endcap',
                                  title='L2MuonSA endcap MDT number of hits '+chain+';MDT nhits;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_N;L2MuonSA_MDT_Inn_N_endcap',
                                  title='L2MuonSA endcap MDT Inner number of hits '+chain+';MDT nhits Inner;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_N;L2MuonSA_MDT_Mid_N_endcap',
                                  title='L2MuonSA endcap MDT Middle number of hits '+chain+';MDT nhits Middle;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Out_N;L2MuonSA_MDT_Out_N_endcap',
                                  title='L2MuonSA endcap MDT Outer number of hits '+chain+';MDT nhits Outer;Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1I', path='',xbins=40,xmin=0,xmax=40)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_residual;L2MuonSA_MDT_Inn_residual_barrel',
                                  title='L2MuonSA barrel MDT Inner station residual '+chain+';MDT Inner barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_residual;L2MuonSA_MDT_Mid_residual_barrel',
                                  title='L2MuonSA barrel MDT Middle station residual '+chain+';MDT Middle barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Out_residual;L2MuonSA_MDT_Out_residual_barrel',
                                  title='L2MuonSA barrel MDT Outer station residual '+chain+';MDT Outer barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_residual_OffMatch;L2MuonSA_MDT_Inn_residual_barrel_OffMatch',
                                  title='L2MuonSA barrel MDT Inner station residual matched with Offline '+chain+';MDT Inner barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_residual_OffMatch;L2MuonSA_MDT_Mid_residual_barrel_OffMatch',
                                  title='L2MuonSA barrel MDT Middle station residual matched with Offline '+chain+';MDT Middle barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Out_residual_OffMatch;L2MuonSA_MDT_Out_residual_barrel_OffMatch',
                                  title='L2MuonSA barrel MDT Outer station residual matched with Offline '+chain+';MDT Outer barrel residual [cm];Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Inn_residual;L2MuonSA_MDT_Inn_residual_barrel_vs_LB',
                                  title='L2MuonSA barrel MDT Inner station residual vs LB '+chain+';LB;MDT Inner barrel residual [cm]', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Mid_residual;L2MuonSA_MDT_Mid_residual_barrel_vs_LB',
                                  title='L2MuonSA barrel MDT Middle station residual vs LB '+chain+';LB;MDT Middle barrel residual [cm]', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Out_residual;L2MuonSA_MDT_Out_residual_barrel_vs_LB',
                                  title='L2MuonSA barrel MDT Outer station residual vs LB '+chain+';LB;MDT Outer barrel residual [cm]', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_residual;L2MuonSA_MDT_Inn_residual_endcap',
                                  title='L2MuonSA endcap MDT Inner station residual '+chain+';MDT Inner endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_residual;L2MuonSA_MDT_Mid_residual_endcap',
                                  title='L2MuonSA endcap MDT Middle station residual '+chain+';MDT Middle endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Out_residual;L2MuonSA_MDT_Out_residual_endcap',
                                  title='L2MuonSA endcap MDT Outer station residual '+chain+';MDT Outer endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Inn_residual_OffMatch;L2MuonSA_MDT_Inn_residual_endcap_OffMatch',
                                  title='L2MuonSA endcap MDT Inner station residual matched with Offline '+chain+';MDT Inner endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Mid_residual_OffMatch;L2MuonSA_MDT_Mid_residual_endcap_OffMatch',
                                  title='L2MuonSA endcap MDT Middle station residual matched with Offline '+chain+';MDT Middle endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_MDT_Out_residual_OffMatch;L2MuonSA_MDT_Out_residual_endcap_OffMatch',
                                  title='L2MuonSA endcap MDT Outer station residual matched with Offline '+chain+';MDT Outer endcap residual [cm];Events', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH1F', path='',xbins=100,xmin=-20,xmax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Inn_residual;L2MuonSA_MDT_Inn_residual_endcap_vs_LB',
                                  title='L2MuonSA endcap MDT Inner station residual vs LB '+chain+';LB;MDT Inner endcap residual [cm]', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Mid_residual;L2MuonSA_MDT_Mid_residual_endcap_vs_LB',
                                  title='L2MuonSA endcap MDT Middle station residual vs LB '+chain+';LB;MDT Middle endcap residual [cm]', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)

        histGroup.defineHistogram(GroupName+'_LB,'+GroupName+'_MDT_Out_residual;L2MuonSA_MDT_Out_residual_endcap_vs_LB',
                                  title='L2MuonSA endcap MDT Outer station residual vs LB '+chain+';LB;MDT Outer endcap residual [cm]', 
                                  cutmask=GroupName+'_isEndcap',
                                  type='TH2F', path='',xbins=750,xmin=1.,xmax=1501.,ybins=80,ymin=-20,ymax=20)



        # Comparison to Offline
        # dR wrt Offline
        histGroup.defineHistogram(GroupName+'_dRmin;L2MuonSA_dR_toRecMuonCB',
                                  title='dR between L2MuonSA and Offline '+chain+';#DeltaR;Events', 
                                  type='TH1F', path='',xbins=100,xmin=0,xmax=2)


        # L1 RoI wrt offline
        histGroup.defineHistogram(GroupName+'_initialRoI_dR;L2MuonSA_initialRoI_dR_toRecMuonCB',
                                  title='L2MuonSA initialRoI dR wrt offline CB '+chain+';dR(initialRoI vs offl CB);Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)

        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_initialRoI_dEta;L2MuonSA_initialRoI_dEta_vs_Eta_toRecMuonCB',
                                  title='L2MuonSA initialRoI wrt Offline CB muon, d#eta '+chain+';offl CB #eta;d#eta(initialRoI vs offl CB)', 
                                  type='TH2F', path='',xbins=54,xmin=-2.7,xmax=2.7, ybins=60,ymin=-0.3,ymax=0.3)

        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_initialRoI_dPhi;L2MuonSA_initialRoI_dPhi_vs_Eta_toRecMuonCB',
                                  title='L2MuonSA initialRoI wrt Offline CB muon, d#phi '+chain+';offl CB #eta;d#phi(initialRoI vs offl CB)', 
                                  type='TH2F', path='',xbins=54,xmin=-2.7,xmax=2.7, ybins=44,ymin=-0.2,ymax=0.2)


        # pt resolution (barrel, endcap1, endcap2, or endcap3) (A-side or C-side)
        histGroup.defineHistogram(GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB',
                                  title='L2MuonSA pT resolution wrt Offline '+chain+';p_{T} resol;Events', 
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_BR',
                                  title='L2MuonSA pT resolution wrt Offline Barrel '+chain+';p_{T} resol;Events', 
                                  cutmask=GroupName+'_isBarrel',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_EC1',
                                  title='L2MuonSA pT resolution wrt Offline Endcap1 '+chain+';p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap1',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_EC2',
                                  title='L2MuonSA pT resolution wrt Offline Endcap2 '+chain+';p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap2',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_EC3',
                                  title='L2MuonSA pT resolution wrt Offline Endcap3 '+chain+';p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap3',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)


        histGroup.defineHistogram(GroupName+'_offPt_signed,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_pt_barrel_A',
                                  title='L2MuonSA barrel A pT resolution wrt Offline in pt '+chain+';p_{T} [GeV/c];p_{T} resol', 
                                  cutmask=GroupName+'_isBarrelA',
                                  type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offPt_signed,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_pt_barrel_C',
                                  title='L2MuonSA barrel C pT resolution wrt Offline in pt '+chain+';p_{T} [GeV/c];p_{T} resol', 
                                  cutmask=GroupName+'_isBarrelC',
                                  type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offPt_signed,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_pt_endcap_A',
                                  title='L2MuonSA endcap A pT resolution wrt Offline in pt '+chain+';p_{T} [GeV/c];p_{T} resol', 
                                  cutmask=GroupName+'_isEndcapA',
                                  type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offPt_signed,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_pt_endcap_C',
                                  title='L2MuonSA endcap C pT resolution wrt Offline in pt '+chain+';p_{T} [GeV/c];p_{T} resol', 
                                  cutmask=GroupName+'_isEndcapC',
                                  type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-2.,ymax=2.)


        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_eta',
                                  title='L2MuonSA pT resolution wrt Offline in eta '+chain+';#eta;p_{T} resol', 
                                  type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_eta_pT4_6',
                                  title='L2MuonSA pT resolution wrt Offline in eta (pT4GeV-6GeV) '+chain+';#eta;p_{T} resol', 
                                  cutmask=GroupName+'_pt4to6',
                                  type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_eta_pT6_8',
                                  title='L2MuonSA pT resolution wrt Offline in eta (pT6GeV-8GeV) '+chain+';#eta;p_{T} resol', 
                                  cutmask=GroupName+'_pt6to8',
                                  type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-2.,ymax=2.)

        histGroup.defineHistogram(GroupName+'_offEta,'+GroupName+'_ptresol;L2MuonSA_ptresol_toRecMuonCB_eta_pT8_x',
                                  title='L2MuonSA pT resolution wrt Offline in eta (pT over 8GeV) '+chain+';#eta;p_{T} resol', 
                                  cutmask=GroupName+'_ptover8',
                                  type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-2.,ymax=2.)


        # inverse pt resolution (positive or negative muon) (A-side or C-side) (barrel, endcap1, endcap2, or endcap3)
        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos',
                                  title='L2MuonSA pT resolution wrt Offline positive muons '+chain+';1/p_{T} resol;Events', 
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg',
                                  title='L2MuonSA pT resolution wrt Offline negative muons '+chain+';1/p_{T} resol;Events', 
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)


        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_BR_A',
                                  title='L2MuonSA pT resolution wrt Offline pos muons Barrel A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isBarrelA',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_BR_A',
                                  title='L2MuonSA pT resolution wrt Offline neg muons Barrel A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isBarrelA',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC1_A',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap1 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap1A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC1_A',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap1 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap1A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC2_A',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap2 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap2A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC2_A',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap2 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap2A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC3_A',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap3 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap3A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC3_A',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap3 A-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap3A',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)


        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_BR_C',
                                  title='L2MuonSA pT resolution wrt Offline pos muons Barrel C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isBarrelC',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_BR_C',
                                  title='L2MuonSA pT resolution wrt Offline neg muons Barrel C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isBarrelC',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC1_C',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap1 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap1C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC1_C',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap1 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap1C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC2_C',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap2 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap2C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC2_C',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap2 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap2C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_pos;L2MuonSA_invptresol_toRecMuonCB_pos_EC3_C',
                                  title='L2MuonSA pT resolution wrt Offline pos muons EndCap3 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap3C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

        histGroup.defineHistogram(GroupName+'_invptresol_neg;L2MuonSA_invptresol_toRecMuonCB_neg_EC3_C',
                                  title='L2MuonSA pT resolution wrt Offline neg muons EndCap3 C-side '+chain+';1/p_{T} resol;Events', 
                                  cutmask=GroupName+'_isEndcap3C',
                                  type='TH1F', path='',xbins=100,xmin=-2.,xmax=2.)

    return
