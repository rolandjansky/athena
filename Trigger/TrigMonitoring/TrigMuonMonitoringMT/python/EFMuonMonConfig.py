#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

def EFMuonMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'EFMuon'

    monAlg = helper.addAlgorithm(CompFactory.EFMuonMonMT,'EFMuonMonMT')
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    monAlg.MonitoredChains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20', 'HLT_mu60_0eta105_msonly_L1MU20', 'HLT_2mu14_L12MU10']
    monAlg.Group = GroupName

    # configuration of etaphi2D and Ratio plots for non-specific chain
    histGroupNonSpecificChain = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/')

    histGroupNonSpecificChain.defineHistogram('EFSAEta,EFSAPhi;EFSA_Eta_vs_Phi',
                                              title='EFSA Eta vs Phi ;#eta;#phi',
                                              type='TH2F', path='etaphi2D',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

    histGroupNonSpecificChain.defineHistogram('EFCBEta,EFCBPhi;EFCB_Eta_vs_Phi',
                                              title='EFCB Eta vs Phi ;#eta;#phi',
                                              type='TH2F', path='etaphi2D',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

    histGroupNonSpecificChain.defineHistogram('EFSAAboveCut,LB;EFSA_Over_OfflineSA_4GeV_Cut',
                                              title='EFSA Over OfflineSA 4GeV Cut;LB;Ratio',
                                              type='TEfficiency', path='Ratio',xbins=400,xmin=1.,xmax=801.)

    histGroupNonSpecificChain.defineHistogram('EFCBAboveCut,LB;EFCB_Over_OfflineCB_4GeV_Cut',
                                              title='EFCB Over OfflineCB 4GeV Cut;LB;Ratio',
                                              type='TEfficiency', path='Ratio',xbins=400,xmin=1.,xmax=801.)


    # configration for specific chain
    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/EFMuon/'+chain)

        # basic EDM variables
        # EFSA
        histGroup.defineHistogram(chain+'_EFSA_Pt;EFSA_Pt',
                                  title='EFSA Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_EFSA_Eta;EFSA_Eta',
                                  title='EFSA Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_EFSA_Phi;EFSA_Phi',
                                  title='EFSA Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_EFSA_Eta,'+chain+'_EFSA_Phi;EFSA_Eta_vs_Phi',
                                  title='EFSA Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # EFCB
        histGroup.defineHistogram(chain+'_EFCB_Pt;EFCB_Pt',
                                  title='EFCB Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_EFCB_Eta;EFCB_Eta',
                                  title='EFCB Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_EFCB_Phi;EFCB_Phi',
                                  title='EFCB Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_EFCB_Eta,'+chain+'_EFCB_Phi;EFCB_Eta_vs_Phi',
                                  title='EFCB Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # EFIso
        histGroup.defineHistogram(chain+'_PtCone03;EFIso_PtCone03',
                                  title='EFIso sum Pt in 0.3 cone '+chain+';Sum p_{T} [GeV];Events',
                                  type='TH1F', path='',xbins=50, xmin=0., xmax=25.)

        histGroup.defineHistogram(chain+'_PtCone03overMuonPt;EFIso_PtCone03overMuonPt',
                                  title='EFIso sum Pt in 0.3 cone / muon Pt '+chain+';Sum p_{T} in 0.3 cone / muon p_{T};Events',
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)


        # OfflineSA
        histGroup.defineHistogram(chain+'_OfflineSA_Pt;OfflineSA_Pt',
                                  title='OfflineSA Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_OfflineSA_Eta;OfflineSA_Eta',
                                  title='OfflineSA Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_OfflineSA_Phi;OfflineSA_Phi',
                                  title='OfflineSA Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineSA_Eta,'+chain+'_OfflineSA_Phi;OfflineSA_Eta_vs_Phi',
                                  title='OfflineSA Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # OfflineCB
        histGroup.defineHistogram(chain+'_OfflineCB_Pt;OfflineCB_Pt',
                                  title='OfflineCB Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_OfflineCB_Eta;OfflineCB_Eta',
                                  title='OfflineCB Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(chain+'_OfflineCB_Phi;OfflineCB_Phi',
                                  title='OfflineCB Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineCB_Eta,'+chain+'_OfflineCB_Phi;OfflineCB_Eta_vs_Phi',
                                  title='OfflineCB Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())



        # correlation histograms
        # EFSA vs. OfflineSA muons
        histGroup.defineHistogram(chain+'_OfflineSA_Pt,'+chain+'_MatchedEFSA_Pt;EFSA_OfflineSA_PtCor',
                                  title='PtCor EFSA OfflineSA '+chain+';OfflineSA p_{T} [GeV];EFSA p_{T} [GeV]',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(chain+'_OfflineSA_Phi,'+chain+'_MatchedEFSA_Phi;EFSA_OfflineSA_PhiCor',
                                  title='PhiCor EFSA OfflineSA '+chain+';OfflineSA #phi;EFSA #phi',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineSA_Eta,'+chain+'_MatchedEFSA_Eta;EFSA_OfflineSA_EtaCor',
                                  title='EtaCor EFSA OfflineSA '+chain+';OfflineSA #eta;EFSA #eta',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)

        histGroup.defineHistogram(chain+'_SAdPt;EFSA_OfflineSA_dPt',
                                  title='dPt EFSA OfflineSA '+chain+';dPt [GeV];Events', 
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=100,xmin=-10.,xmax=10.)

        histGroup.defineHistogram(chain+'_SAdEta;EFSA_OfflineSA_dEta',
                                  title='dEta EFSA OfflineSA '+chain+';dEta;Events', 
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=200,xmin=-5.,xmax=5.)

        histGroup.defineHistogram(chain+'_SAdPhi;EFSA_OfflineSA_dPhi',
                                  title='dPhi EFSA OfflineSA '+chain+';dPhi;Events', 
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_SAdR;EFSA_OfflineSA_dR',
                                  title='dR EFSA OfflineSA '+chain+';dR;Events', 
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=110,xmin=0.,xmax=11.)

        histGroup.defineHistogram(chain+'_OfflineSA_Pt;OfflineSA_Pt_AwareEFSA',
                                  title='OfflineSA Pt AwareEFSA '+chain+';OfflineSA p_{T} [GeV];Events',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_OfflineSA_Phi;OfflineSA_Phi_AwareEFSA',
                                  title='OfflineSA Phi AwareEFSA '+chain+';OfflineSA #phi;Events',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineSA_Eta;OfflineSA_Eta_AwareEFSA',
                                  title='OfflineSA Eta AwareEFSA '+chain+';OfflineSA #eta;Events',
                                  cutmask=chain+'_matchedEFSA',
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)


        # OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA)
        histGroup.defineHistogram(chain+'_OfflineSA_Pt,'+chain+'_OfflineSAmatchedL2SA_Pt;OfflineSA_matchedEFSA_matchedL2SA_PtCor',
                                  title='PtCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA p_{T} [GeV];OfflineSA matched to L2SA p_{T} [GeV]',
                                  cutmask=chain+'_matchedL2SA',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(chain+'_OfflineSA_Phi,'+chain+'_OfflineSAmatchedL2SA_Phi;OfflineSA_matchedEFSA_matchedL2SA_PhiCor',
                                  title='PhiCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA #phi;OfflineSA matched to L2SA #phi',
                                  cutmask=chain+'_matchedL2SA',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineSA_Eta,'+chain+'_OfflineSAmatchedL2SA_Eta;OfflineSA_matchedEFSA_matchedL2SA_EtaCor',
                                  title='EtaCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA #eta;OfflineSA matched to L2SA #eta',
                                  cutmask=chain+'_matchedL2SA',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)


        # EFCB vs. OfflineCB muons
        histGroup.defineHistogram(chain+'_OfflineCB_Pt,'+chain+'_MatchedEFCB_Pt;EFCB_OfflineCB_PtCor',
                                  title='PtCor EFCB OfflineCB '+chain+';OfflineCB p_{T} [GeV];EFCB p_{T} [GeV];',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(chain+'_OfflineCB_Phi,'+chain+'_MatchedEFCB_Phi;EFCB_OfflineCB_PhiCor',
                                  title='PhiCor EFCB OfflineCB '+chain+';OfflineCB #phi;EFCB #phi;',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineCB_Eta,'+chain+'_MatchedEFCB_Eta;EFCB_OfflineCB_EtaCor',
                                  title='EtaCor EFCB OfflineCB '+chain+';OfflineCB #eta;EFCB #eta;',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)

        histGroup.defineHistogram(chain+'_CBdPt;EFCB_OfflineCB_dPt',
                                  title='dPt EFCB OfflineCB '+chain+';dPt [GeV];Events', 
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=100,xmin=-10.,xmax=10.)

        histGroup.defineHistogram(chain+'_CBdEta;EFCB_OfflineCB_dEta',
                                  title='dEta EFCB OfflineCB '+chain+';dEta;Events', 
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=200,xmin=-5.,xmax=5.)

        histGroup.defineHistogram(chain+'_CBdPhi;EFCB_OfflineCB_dPhi',
                                  title='dPhi EFCB OfflineCB '+chain+';dPhi;Events', 
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_CBdR;EFCB_OfflineCB_dR',
                                  title='dR EFCB OfflineCB '+chain+';dR;Events', 
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=110,xmin=0.,xmax=11.)

        histGroup.defineHistogram(chain+'_OfflineCB_Pt;OfflineCB_Pt_AwareEFCB',
                                  title='OfflineCB Pt AwareEFCB '+chain+';OfflineCB p_{T} [GeV];Events',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(chain+'_OfflineCB_Phi;OfflineCB_Phi_AwareEFCB',
                                  title='OfflineCB Phi AwareEFCB '+chain+';OfflineCB #phi;Events',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineCB_Eta;OfflineCB_Eta_AwareEFCB',
                                  title='OfflineCB Eta AwareEFCB '+chain+';OfflineCB #eta;Events',
                                  cutmask=chain+'_matchedEFCB',
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)


        # OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB)
        histGroup.defineHistogram(chain+'_OfflineCB_Pt,'+chain+'_OfflineCBmatchedL2CB_Pt;OfflineCB_matchedEFCB_matchedL2CB_PtCor',
                                  title='PtCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB p_{T} [GeV];OfflineCB matched to L2CB p_{T} [GeV]',
                                  cutmask=chain+'_matchedL2CB',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(chain+'_OfflineCB_Phi,'+chain+'_OfflineCBmatchedL2CB_Phi;OfflineCB_matchedEFCB_matchedL2CB_PhiCor',
                                  title='PhiCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB #phi;OfflineCB matched to L2CB #phi',
                                  cutmask=chain+'_matchedL2CB',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(chain+'_OfflineCB_Eta,'+chain+'_OfflineCBmatchedL2CB_Eta;OfflineCB_matchedEFCB_matchedL2CB_EtaCor',
                                  title='EtaCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB #eta;OfflineCB matched to L2CB #eta',
                                  cutmask=chain+'_matchedL2CB',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)


        # EFIso vs. OfflineIso muons
        histGroup.defineHistogram(chain+'_MatchedEFPIsotCone03,'+chain+'_OfflineIsoPtCone03;EFIso_OfflineIso_CorPtCone03',
                                  title='sum Pt in 0.3 cone correlation EFIso vs. OfflineIso '+chain+';EFIso Sum p_{T} [GeV];OfflineIso Sum p_{T} [GeV];Events',
                                  type='TH2F', path='',xbins=50, xmin=0., xmax=25., ybins=50, ymin=0., ymax=25.)

        histGroup.defineHistogram(chain+'_MatchedEFIsoPtCone03overMuonPt,'+chain+'_OfflineIsoPtCone03overMuonPt;EFIso_OfflineIso_CorPtCone03overMuonPt',
                                  title='sum Pt in 0.3 cone / muon Pt correlation EFIso vs. OfflineIso '+chain+';EFIso Sum p_{T} in 0.3 cone / muon p_{T};OfflineIso Sum p_{T} in 0.3 cone / muon p_{T};Events',
                                  type='TH2F', path='',xbins=100,xmin=0.,xmax=0.5, ybins=100,ymin=0.,ymax=0.5)


    return
