# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: tabstop=4:shiftwidth=4:expandtab
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' No_EventInfo : Not_2_InputTEs : No_RoIs : No_MuonEFInfoContainer : No_MuonCandidate : SameMuon : DiMuVtxFitFails : DiMuVtxMass_Fails : No_TrackColl : BplusVtxFit_Fails : BplusVtxMass_Fails : WrongDecayID : XVtxFit_Fails : XVtxMass_Fails : XMuMuVtxFit_Fails : XMuMuVtxMass_Fails : BphysCollStore_Fails : No_MuonTrackMatch : TooManyCom_Acc : TooManyComb_Rej'
ERROR_number = 20

#ACCEPTANCE_labels = ' Input : AcceptAll : Got_RoIs : Got_Muons : Got_TrackColl : Full_TrackColl : Full_IDTracks : Pass_OppChargeC : MuonTracks_Added : Muon_Vertexing : CalcInvMass : MuonVtx_Part : MuMu_mass : MotherVtxCreated : BphysCollParticle '
#ACCEPTANCE_number = 15

class EFBMuMuXFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFBMuMuXFexValidationMonitoring"):
        super(EFBMuMuXFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        # General
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                              xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                              labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                              xbins = 1000, xmin = 0., xmax = 100000. ) ]
        #self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                              #title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                              #xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                              #labels = ACCEPTANCE_labels ) ]
        # Timing
        self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50. ) ]
        self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50. ) ]
        self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50.,
                                              ybins = 100, ymin = 0., ymax = 50. ) ]
        # RoIs
        self.Histograms += [ defineHistogram ( 'RoI_RoI1Eta, RoI_RoI1Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROI1 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_RoI2Eta, RoI_RoI2Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROI2 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_dEtaRoI, RoI_dPhiRoI', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROIs direction difference ; d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        # Muons
        self.Histograms += [ defineHistogram ( 'DiMu_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of muons in RoIs; n(#mu) ; # Events',
                                              xbins = 10, xmin = -0.5, xmax = 10-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Pt_Mu1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Pt_Mu2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_pTsumMuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - sum of two muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Eta_Mu1, DiMu_Phi_Mu1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of muon1 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Eta_Mu2, DiMu_Phi_Mu2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of muon2 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_dEtaMuMu, DiMu_dPhiMuMu', type = 'TH2F',
                                              title = 'EFBMuMuXFex - dEta vs. dPhi between muon tracks (no vertexing); d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_InvMassMuMu, DiMu_VtxMassMuMu', type = 'TH2F',
                                              title = 'EFBMuMuXFex - InvMass vs. VtxMass of muon pair ; InvMass [GeV] ; VtxMass [GeV] ; # Events',
                                              xbins = 80, xmin = 0., xmax = 20.,
                                              ybins = 80, ymin = 0., ymax = 20. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_InvMassMuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of muon pair ; InvMass [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Chi2MuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - dimuon vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Tracks
        self.Histograms += [ defineHistogram ( 'Tracks_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of tracks in event ; n(trks) ; # Events',
                                              xbins = 200, xmin = -0.5, xmax = 200-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Eta, Tracks_Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Pt', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of all tracks; p_{T} [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        # B+
        self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B^{+} candidates in event ; n(B^{+}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Pt_K', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of K in B^{+} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Eta_K, BMuMuK_Phi_K', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of K in B^{+} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B, BMuMuK_VtxMass_B', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B^{+} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Chi2_B', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B^{+} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bd
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{d} candidates in event ; n(B_{d}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Pt_K', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of K in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Eta_K, BdMuMuKs_Phi_K', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of K in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Pt_Pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Eta_Pi, BdMuMuKs_Phi_Pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Kstar, BdMuMuKs_VtxMass_Kstar', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of K^{*} in B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Kstar', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd, BdMuMuKs_VtxMass_Bd', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Chi2_Kstar', type = 'TH1F',
                                              title = 'EFBMuMuXFex - K^{*} vertex chi2 in B_{d}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Chi2_Bd', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{d} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bs
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{s} candidates in event ; n(B_{s}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Pt_K1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of first K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Eta_K1, BsMuMuPhi_Phi_K1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of first K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Pt_K2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of second K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Eta_K2, BsMuMuPhi_Phi_K2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of second K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Phi1020, BsMuMuPhi_VtxMass_Phi1020', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #phi(1020) in B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Phi1020', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs, BsMuMuPhi_VtxMass_Bs', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Chi2_Phi1020', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #phi(1020) vertex chi2 in B_{s}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Chi2_Bs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{s} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Lb
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of #Lambda_{b} candidates in event ; n(#Lambda_{b}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Pt_P', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of proton in #Lambda_{b} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Eta_P, LbMuMuLambda_Phi_P', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of proton in #Lambda_{b} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Pt_Pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in #Lambda_{b} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Eta_Pi, LbMuMuLambda_Phi_Pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in #Lambda_{b} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lambda, LbMuMuLambda_VtxMass_Lambda', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lambda', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lb, LbMuMuLambda_VtxMass_Lb', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #Lambda_{b} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lb', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Chi2_Lambda', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #Lambda vertex chi2 in #Lambda_{b}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Chi2_Lb', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #Lambda_{b} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bc
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{c} candidates in event ; n(B_{c}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_K1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of first K in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_K1, BcMuMuDs_Phi_K1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of first K in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_K2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of second K in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_K2, BcMuMuDs_Phi_K2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of second K in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_pi, BcMuMuDs_Phi_pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_PhiDs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Ds, BcMuMuDs_VtxMass_Ds', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of D_{s} in B_{c} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 1.4, xmax = 2.6,
                                              #ybins = 60, ymin = 1.4, ymax = 2.6 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Ds', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
        #self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc, BcMuMuDs_VtxMass_Bc', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{c} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 120, xmin = 1.5, xmax = 7.5,
                                              #ybins = 120, ymin = 1.5, ymax = 7.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Chi2_Ds', type = 'TH1F',
                                              title = 'EFBMuMuXFex - D_{s} vertex chi2 in B_{c}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Chi2_Bc', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{c} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        

class EFBMuMuXFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFBMuMuXFexOnlineMonitoring"):
        super(EFBMuMuXFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        # General
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                              xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                              labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                              xbins = 1000, xmin = 0., xmax = 100000. ) ]
        #self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                              #title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                              #xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                              #labels = ACCEPTANCE_labels ) ]
        ## Timing
        #self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                              #title = 'EFBMuMuXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50. ) ]
        #self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                              #title = 'EFBMuMuXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50. ) ]
        #self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50.,
                                              #ybins = 100, ymin = 0., ymax = 50. ) ]
        # RoIs
        self.Histograms += [ defineHistogram ( 'RoI_RoI1Eta, RoI_RoI1Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROI1 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_RoI2Eta, RoI_RoI2Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROI2 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_dEtaRoI, RoI_dPhiRoI', type = 'TH2F',
                                              title = 'EFBMuMuXFex - ROIs direction difference ; d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        # Muons
        self.Histograms += [ defineHistogram ( 'DiMu_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of muons in RoIs; n(#mu) ; # Events',
                                              xbins = 10, xmin = -0.5, xmax = 10-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Pt_Mu1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Pt_Mu2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_pTsumMuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - sum of two muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Eta_Mu1, DiMu_Phi_Mu1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of muon1 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Eta_Mu2, DiMu_Phi_Mu2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of muon2 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_dEtaMuMu, DiMu_dPhiMuMu', type = 'TH2F',
                                              title = 'EFBMuMuXFex - dEta vs. dPhi between muon tracks (no vertexing); d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_InvMassMuMu, DiMu_VtxMassMuMu', type = 'TH2F',
                                              title = 'EFBMuMuXFex - InvMass vs. VtxMass of muon pair ; InvMass [GeV] ; VtxMass [GeV] ; # Events',
                                              xbins = 80, xmin = 0., xmax = 20.,
                                              ybins = 80, ymin = 0., ymax = 20. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_InvMassMuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of muon pair ; InvMass [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'DiMu_Chi2MuMu', type = 'TH1F',
                                              title = 'EFBMuMuXFex - dimuon vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Tracks
        self.Histograms += [ defineHistogram ( 'Tracks_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of tracks in event ; n(trks) ; # Events',
                                              xbins = 200, xmin = -0.5, xmax = 200-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Eta, Tracks_Phi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Pt', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of all tracks; p_{T} [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        # B+
        self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B^{+} candidates in event ; n(B^{+}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Pt_K', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of K in B^{+} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Eta_K, BMuMuK_Phi_K', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of K in B^{+} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B, BMuMuK_VtxMass_B', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B^{+} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BMuMuK_Chi2_B', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B^{+} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bd
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{d} candidates in event ; n(B_{d}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Pt_K', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of K in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Eta_K, BdMuMuKs_Phi_K', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of K in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Pt_Pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Eta_Pi, BdMuMuKs_Phi_Pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Kstar, BdMuMuKs_VtxMass_Kstar', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of K^{*} in B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Kstar', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd, BdMuMuKs_VtxMass_Bd', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Chi2_Kstar', type = 'TH1F',
                                              title = 'EFBMuMuXFex - K^{*} vertex chi2 in B_{d}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BdMuMuKs_Chi2_Bd', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{d} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bs
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{s} candidates in event ; n(B_{s}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Pt_K1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of first K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Eta_K1, BsMuMuPhi_Phi_K1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of first K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Pt_K2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of second K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Eta_K2, BsMuMuPhi_Phi_K2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of second K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Phi1020, BsMuMuPhi_VtxMass_Phi1020', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #phi(1020) in B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Phi1020', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs, BsMuMuPhi_VtxMass_Bs', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Chi2_Phi1020', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #phi(1020) vertex chi2 in B_{s}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BsMuMuPhi_Chi2_Bs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{s} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Lb
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of #Lambda_{b} candidates in event ; n(#Lambda_{b}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Pt_P', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of proton in #Lambda_{b} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Eta_P, LbMuMuLambda_Phi_P', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of proton in #Lambda_{b} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Pt_Pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in #Lambda_{b} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Eta_Pi, LbMuMuLambda_Phi_Pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in #Lambda_{b} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lambda, LbMuMuLambda_VtxMass_Lambda', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lambda', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lb, LbMuMuLambda_VtxMass_Lb', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of #Lambda_{b} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_InvMass_Lb', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Chi2_Lambda', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #Lambda vertex chi2 in #Lambda_{b}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'LbMuMuLambda_Chi2_Lb', type = 'TH1F',
                                              title = 'EFBMuMuXFex - #Lambda_{b} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bc
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                              title = 'EFBMuMuXFex - number of B_{c} candidates in event ; n(B_{c}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_K1', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of first K in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_K1, BcMuMuDs_Phi_K1', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of first K in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_K2', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of second K in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_K2, BcMuMuDs_Phi_K2', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of second K in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Pt_pi', type = 'TH1F',
                                              title = 'EFBMuMuXFex - p_{T} of #pi in B_{c} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Eta_pi, BcMuMuDs_Phi_pi', type = 'TH2F',
                                              title = 'EFBMuMuXFex - Eta vs. phi of #pi in B_{c} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_PhiDs', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Ds, BcMuMuDs_VtxMass_Ds', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of D_{s} in B_{c} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 1.4, xmax = 2.6,
                                              #ybins = 60, ymin = 1.4, ymax = 2.6 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Ds', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
        #self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc, BcMuMuDs_VtxMass_Bc', type = 'TH2F',
                                              #title = 'EFBMuMuXFex - InvMass vs. VtxMass of B_{c} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 120, xmin = 1.5, xmax = 7.5,
                                              #ybins = 120, ymin = 1.5, ymax = 7.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'EFBMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Chi2_Ds', type = 'TH1F',
                                              title = 'EFBMuMuXFex - D_{s} vertex chi2 in B_{c}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BcMuMuDs_Chi2_Bc', type = 'TH1F',
                                              title = 'EFBMuMuXFex - B_{c} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
