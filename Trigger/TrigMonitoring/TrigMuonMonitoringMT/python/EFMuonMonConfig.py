#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

def EFMuonMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    # HLT_mu6_L1MU6 is test chain for small statistics, so it will be removed.
    Chains = ['HLT_mu6_L1MU6', 'HLT_mu26_ivarmedium_L1MU20', 'HLT_mu50_L1MU20']

    for chain in Chains:

        GroupName = 'EFMuon_'+chain

        monAlg = helper.addAlgorithm(CompFactory.EFMuonMonMT,'EFMuonMonMT_'+chain)
        monAlg.MuonType = ROOT.xAOD.Muon_v1.MuonStandAlone
        monAlg.MonitoredChains = [chain]
        monAlg.Group = GroupName

        histGroup = helper.addGroup(monAlg, GroupName, 'HLT/MuonMon/EFMuon/'+chain)
    
    
        # basic EDM variables
        # EFSA
        histGroup.defineHistogram(GroupName+'_EFSA_Pt;EFSA_Pt',
                                  title='EFSA Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_EFSA_Eta;EFSA_Eta',
                                  title='EFSA Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(GroupName+'_EFSA_Phi;EFSA_Phi',
                                  title='EFSA Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_EFSA_Eta,'+GroupName+'_EFSA_Phi;EFSA_Eta_vs_Phi',
                                  title='EFSA Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # EFCB
        histGroup.defineHistogram(GroupName+'_EFCB_Pt;EFCB_Pt',
                                  title='EFCB Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_EFCB_Eta;EFCB_Eta',
                                  title='EFCB Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(GroupName+'_EFCB_Phi;EFCB_Phi',
                                  title='EFCB Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_EFCB_Eta,'+GroupName+'_EFCB_Phi;EFCB_Eta_vs_Phi',
                                  title='EFCB Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # OfflineSA
        histGroup.defineHistogram(GroupName+'_OfflineSA_Pt;OfflineSA_Pt',
                                  title='OfflineSA Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Eta;OfflineSA_Eta',
                                  title='OfflineSA Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Phi;OfflineSA_Phi',
                                  title='OfflineSA Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineSA_Eta,'+GroupName+'_OfflineSA_Phi;OfflineSA_Eta_vs_Phi',
                                  title='OfflineSA Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())


        # OfflineCB
        histGroup.defineHistogram(GroupName+'_OfflineCB_Pt;OfflineCB_Pt',
                                  title='OfflineCB Pt '+chain+';p_{T} [GeV];Events', 
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Eta;OfflineCB_Eta',
                                  title='OfflineCB Eta '+chain+';#eta;Events', 
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Phi;OfflineCB_Phi',
                                  title='OfflineCB Phi '+chain+';#phi;Events', 
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineCB_Eta,'+GroupName+'_OfflineCB_Phi;OfflineCB_Eta_vs_Phi',
                                  title='OfflineCB Eta vs Phi '+chain+';#eta;#phi', 
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())



        # correlation histograms
        # EFSA vs. OfflineSA muons
        histGroup.defineHistogram(GroupName+'_OfflineSA_Pt,'+GroupName+'_MatchedEFSA_Pt;EFSA_OfflineSA_PtCor',
                                  title='PtCor EFSA OfflineSA '+chain+';OfflineSA p_{T} [GeV];EFSA p_{T} [GeV]',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Phi,'+GroupName+'_MatchedEFSA_Phi;EFSA_OfflineSA_PhiCor',
                                  title='PhiCor EFSA OfflineSA '+chain+';OfflineSA #phi;EFSA #phi',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineSA_Eta,'+GroupName+'_MatchedEFSA_Eta;EFSA_OfflineSA_EtaCor',
                                  title='EtaCor EFSA OfflineSA '+chain+';OfflineSA #eta;EFSA #eta',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)

        histGroup.defineHistogram(GroupName+'_SAdPt;EFSA_OfflineSA_dPt',
                                  title='dPt EFSA OfflineSA '+chain+';dPt [GeV];Events', 
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=100,xmin=-10.,xmax=10.)

        histGroup.defineHistogram(GroupName+'_SAdEta;EFSA_OfflineSA_dEta',
                                  title='dEta EFSA OfflineSA '+chain+';dEta;Events', 
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=200,xmin=-5.,xmax=5.)

        histGroup.defineHistogram(GroupName+'_SAdPhi;EFSA_OfflineSA_dPhi',
                                  title='dPhi EFSA OfflineSA '+chain+';dPhi;Events', 
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_SAdR;EFSA_OfflineSA_dR',
                                  title='dR EFSA OfflineSA '+chain+';dR;Events', 
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=110,xmin=0.,xmax=11.)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Pt;OfflineSA_Pt_AwareEFSA',
                                  title='OfflineSA Pt AwareEFSA '+chain+';OfflineSA p_{T} [GeV];Events',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Phi;OfflineSA_Phi_AwareEFSA',
                                  title='OfflineSA Phi AwareEFSA '+chain+';OfflineSA #phi;Events',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineSA_Eta;OfflineSA_Eta_AwareEFSA',
                                  title='OfflineSA Eta AwareEFSA '+chain+';OfflineSA #eta;Events',
                                  cutmask=GroupName+'_matchedEFSA',
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)


        # OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA)
        histGroup.defineHistogram(GroupName+'_OfflineSA_Pt,'+GroupName+'_OfflineSAmatchedL2SA_Pt;OfflineSA_matchedEFSA_matchedL2SA_PtCor',
                                  title='PtCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA p_{T} [GeV];OfflineSA matched to L2SA p_{T} [GeV]',
                                  cutmask=GroupName+'_matchedL2SA',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineSA_Phi,'+GroupName+'_OfflineSAmatchedL2SA_Phi;OfflineSA_matchedEFSA_matchedL2SA_PhiCor',
                                  title='PhiCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA #phi;OfflineSA matched to L2SA #phi',
                                  cutmask=GroupName+'_matchedL2SA',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineSA_Eta,'+GroupName+'_OfflineSAmatchedL2SA_Eta;OfflineSA_matchedEFSA_matchedL2SA_EtaCor',
                                  title='EtaCor OfflineSA (matched to EFSA) vs. OfflineSA (matched to L2SA) '+chain+';OfflineSA matched to EFSA #eta;OfflineSA matched to L2SA #eta',
                                  cutmask=GroupName+'_matchedL2SA',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)


        # EFCB vs. OfflineCB muons
        histGroup.defineHistogram(GroupName+'_OfflineCB_Pt,'+GroupName+'_MatchedEFCB_Pt;EFCB_OfflineCB_PtCor',
                                  title='PtCor EFCB OfflineCB '+chain+';OfflineCB p_{T} [GeV];EFCB p_{T} [GeV];',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Phi,'+GroupName+'_MatchedEFCB_Phi;EFCB_OfflineCB_PhiCor',
                                  title='PhiCor EFCB OfflineCB '+chain+';OfflineCB #phi;EFCB #phi;',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineCB_Eta,'+GroupName+'_MatchedEFCB_Eta;EFCB_OfflineCB_EtaCor',
                                  title='EtaCor EFCB OfflineCB '+chain+';OfflineCB #eta;EFCB #eta;',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)

        histGroup.defineHistogram(GroupName+'_CBdPt;EFCB_OfflineCB_dPt',
                                  title='dPt EFCB OfflineCB '+chain+';dPt [GeV];Events', 
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=100,xmin=-10.,xmax=10.)

        histGroup.defineHistogram(GroupName+'_CBdEta;EFCB_OfflineCB_dEta',
                                  title='dEta EFCB OfflineCB '+chain+';dEta;Events', 
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=200,xmin=-5.,xmax=5.)

        histGroup.defineHistogram(GroupName+'_CBdPhi;EFCB_OfflineCB_dPhi',
                                  title='dPhi EFCB OfflineCB '+chain+';dPhi;Events', 
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_CBdR;EFCB_OfflineCB_dR',
                                  title='dR EFCB OfflineCB '+chain+';dR;Events', 
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=110,xmin=0.,xmax=11.)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Pt;OfflineCB_Pt_AwareEFCB',
                                  title='OfflineCB Pt AwareEFCB '+chain+';OfflineCB p_{T} [GeV];Events',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Phi;OfflineCB_Phi_AwareEFCB',
                                  title='OfflineCB Phi AwareEFCB '+chain+';OfflineCB #phi;Events',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineCB_Eta;OfflineCB_Eta_AwareEFCB',
                                  title='OfflineCB Eta AwareEFCB '+chain+';OfflineCB #eta;Events',
                                  cutmask=GroupName+'_matchedEFCB',
                                  type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)


        # OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB)
        histGroup.defineHistogram(GroupName+'_OfflineCB_Pt,'+GroupName+'_OfflineCBmatchedL2CB_Pt;OfflineCB_matchedEFCB_matchedL2CB_PtCor',
                                  title='PtCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB p_{T} [GeV];OfflineCB matched to L2CB p_{T} [GeV]',
                                  cutmask=GroupName+'_matchedL2CB',
                                  type='TH2F', path='',xbins=210,xmin=-105.,xmax=105., ybins=210,ymin=-105.,ymax=105.)

        histGroup.defineHistogram(GroupName+'_OfflineCB_Phi,'+GroupName+'_OfflineCBmatchedL2CB_Phi;OfflineCB_matchedEFCB_matchedL2CB_PhiCor',
                                  title='PhiCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB #phi;OfflineCB matched to L2CB #phi',
                                  cutmask=GroupName+'_matchedL2CB',
                                  type='TH2F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi(), ybins=96,ymin=-ROOT.TMath.Pi(),ymax=ROOT.TMath.Pi())

        histGroup.defineHistogram(GroupName+'_OfflineCB_Eta,'+GroupName+'_OfflineCBmatchedL2CB_Eta;OfflineCB_matchedEFCB_matchedL2CB_EtaCor',
                                  title='EtaCor OfflineCB (matched to EFCB) vs. OfflineCB (matched to L2CB) '+chain+';OfflineCB matched to EFCB #eta;OfflineCB matched to L2CB #eta',
                                  cutmask=GroupName+'_matchedL2CB',
                                  type='TH2F', path='',xbins=108,xmin=-2.7,xmax=2.7, ybins=108,ymin=-2.7,ymax=2.7)

    return
