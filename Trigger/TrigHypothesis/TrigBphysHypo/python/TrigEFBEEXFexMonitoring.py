# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: tabstop=4:shiftwidth=4:expandtab
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' No_EventInfo : Not_2_InputTEs : No_RoIs : No_ElectronEFInfoContainer : No_ElectronCandidate : SameElectron : DiEVtxFitFails : DiEVtxMass_Fails : No_TrackColl : BplusVtxFit_Fails : BplusVtxMass_Fails : WrongDecayID : XVtxFit_Fails : XVtxMass_Fails : XEEVtxFit_Fails : XEEVtxMass_Fails : BphysCollStore_Fails : No_ElectronTrackMatch : TooManyCom_Acc : TooManyComb_Rej : MaxNumBpReached : MaxNumBdReached : MaxNumBsReached : MaxNumLbReached : MaxNumBcReached'
ERROR_number = 25

#ACCEPTANCE_labels = ' Input : AcceptAll : Got_RoIs : Got_Electrons : Got_TrackColl : Full_TrackColl : Full_IDTracks : Pass_OppChargeC : ElectronTracks_Added : Electron_Vertexing : CalcInvMass : ElectronVtx_Part : EE_mass : MotherVtxCreated : BphysCollParticle '
#ACCEPTANCE_number = 15

class EFBEEXFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFBEEXFexValidationMonitoring"):
        super(EFBEEXFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        # General
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                              title = 'EFBEEXFex - Algorithm errors ; ; # Events / Candidates',
                                              xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                              labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                              title = 'EFBEEXFex - Number of track combinations tried ; # combinations ; # Events',
                                              xbins = 1000, xmin = 0., xmax = 100000. ) ]
        #self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                              #title = 'L2BEEXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                              #xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                              #labels = ACCEPTANCE_labels ) ]
        # Timing
        self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                              title = 'EFBEEXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50. ) ]
        self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                              title = 'EFBEEXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50. ) ]
        self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                              title = 'EFBEEXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 50.,
                                              ybins = 100, ymin = 0., ymax = 50. ) ]
        # RoIs
        self.Histograms += [ defineHistogram ( 'RoI_RoI1Eta, RoI_RoI1Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - ROI1 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_RoI2Eta, RoI_RoI2Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - ROI2 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_dEtaRoI, RoI_dPhiRoI', type = 'TH2F',
                                              title = 'EFBEEXFex - ROIs direction difference ; d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        # Electrons
        self.Histograms += [ defineHistogram ( 'DiE_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of electrons in RoIs; n(#mu) ; # Events',
                                              xbins = 10, xmin = -0.5, xmax = 10-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Pt_Mu1', type = 'TH1F',
                                              title = 'EFBEEXFex - Positive electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Pt_Mu2', type = 'TH1F',
                                              title = 'EFBEEXFex - Negative electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_pTsumEE', type = 'TH1F',
                                              title = 'EFBEEXFex - sum of two electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Eta_Mu1, DiE_Phi_Mu1', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of electron1 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Eta_Mu2, DiE_Phi_Mu2', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of electron2 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_dEtaEE, DiE_dPhiEE', type = 'TH2F',
                                              title = 'EFBEEXFex - dEta vs. dPhi between electron tracks (no vertexing); d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_InvMassEE, DiE_VtxMassEE', type = 'TH2F',
                                              title = 'EFBEEXFex - InvMass vs. VtxMass of electron pair ; InvMass [GeV] ; VtxMass [GeV] ; # Events',
                                              xbins = 80, xmin = 0., xmax = 20.,
                                              ybins = 80, ymin = 0., ymax = 20. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_InvMassEE', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of electron pair ; InvMass [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Chi2EE', type = 'TH1F',
                                              title = 'EFBEEXFex - dielectron vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Tracks
        self.Histograms += [ defineHistogram ( 'Tracks_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of tracks in event ; n(trks) ; # Events',
                                              xbins = 200, xmin = -0.5, xmax = 200-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Eta, Tracks_Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Pt', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of all tracks; p_{T} [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        # B+
        self.Histograms += [ defineHistogram ( 'BEEK_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B^{+} candidates in event ; n(B^{+}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Pt_K', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of K in B^{+} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Eta_K, BEEK_Phi_K', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of K in B^{+} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BEEK_InvMass_B, BEEK_VtxMass_B', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B^{+} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_InvMass_B', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Chi2_B', type = 'TH1F',
                                              title = 'EFBEEXFex - B^{+} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bd
        self.Histograms += [ defineHistogram ( 'BdEEKs_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B_{d} candidates in event ; n(B_{d}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Pt_K', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of K in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Eta_K, BdEEKs_Phi_K', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of K in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Pt_Pi', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of #pi in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Eta_Pi, BdEEKs_Phi_Pi', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of #pi in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Kstar, BdEEKs_VtxMass_Kstar', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of K^{*} in B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Kstar', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Bd, BdEEKs_VtxMass_Bd', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Bd', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Chi2_Kstar', type = 'TH1F',
                                              title = 'EFBEEXFex - K^{*} vertex chi2 in B_{d}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Chi2_Bd', type = 'TH1F',
                                              title = 'EFBEEXFex - B_{d} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bs
        self.Histograms += [ defineHistogram ( 'BsEEPhi_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B_{s} candidates in event ; n(B_{s}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Pt_K1', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of first K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Eta_K1, BsEEPhi_Phi_K1', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of first K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Pt_K2', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of second K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Eta_K2, BsEEPhi_Phi_K2', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of second K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Phi1020, BsEEPhi_VtxMass_Phi1020', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of #phi(1020) in B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Phi1020', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Bs, BsEEPhi_VtxMass_Bs', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Chi2_Phi1020', type = 'TH1F',
                                              title = 'EFBEEXFex - #phi(1020) vertex chi2 in B_{s}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Chi2_Bs', type = 'TH1F',
                                              title = 'EFBEEXFex - B_{s} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        
        

class EFBEEXFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFBEEXFexOnlineMonitoring"):
        super(EFBEEXFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        # General
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                              title = 'EFBEEXFex - Algorithm errors ; ; # Events / Candidates',
                                              xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                              labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                              title = 'EFBEEXFex - Number of track combinations tried ; # combinations ; # Events',
                                              xbins = 1000, xmin = 0., xmax = 100000. ) ]
        #self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                              #title = 'L2BEEXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                              #xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                              #labels = ACCEPTANCE_labels ) ]
        ## Timing
        #self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                              #title = 'EFBEEXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50. ) ]
        #self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                              #title = 'EFBEEXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50. ) ]
        #self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                              #title = 'EFBEEXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                              #xbins = 100, xmin = 0., xmax = 50.,
                                              #ybins = 100, ymin = 0., ymax = 50. ) ]
        # RoIs
        self.Histograms += [ defineHistogram ( 'RoI_RoI1Eta, RoI_RoI1Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - ROI1 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_RoI2Eta, RoI_RoI2Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - ROI2 direction ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'RoI_dEtaRoI, RoI_dPhiRoI', type = 'TH2F',
                                              title = 'EFBEEXFex - ROIs direction difference ; d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        # Electrons
        self.Histograms += [ defineHistogram ( 'DiE_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of electrons in RoIs; n(#mu) ; # Events',
                                              xbins = 10, xmin = -0.5, xmax = 10-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Pt_Mu1', type = 'TH1F',
                                              title = 'EFBEEXFex - Positive electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Pt_Mu2', type = 'TH1F',
                                              title = 'EFBEEXFex - Negative electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_pTsumEE', type = 'TH1F',
                                              title = 'EFBEEXFex - sum of two electrons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Eta_Mu1, DiE_Phi_Mu1', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of electron1 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Eta_Mu2, DiE_Phi_Mu2', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of electron2 tracks (no vertexing); #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_dEtaEE, DiE_dPhiEE', type = 'TH2F',
                                              title = 'EFBEEXFex - dEta vs. dPhi between electron tracks (no vertexing); d#eta ; d#phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'DiE_InvMassEE, DiE_VtxMassEE', type = 'TH2F',
                                              title = 'EFBEEXFex - InvMass vs. VtxMass of electron pair ; InvMass [GeV] ; VtxMass [GeV] ; # Events',
                                              xbins = 80, xmin = 0., xmax = 20.,
                                              ybins = 80, ymin = 0., ymax = 20. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_InvMassEE', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of electron pair ; InvMass [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'DiE_Chi2EE', type = 'TH1F',
                                              title = 'EFBEEXFex - dielectron vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Tracks
        self.Histograms += [ defineHistogram ( 'Tracks_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of tracks in event ; n(trks) ; # Events',
                                              xbins = 200, xmin = -0.5, xmax = 200-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Eta, Tracks_Phi', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Tracks_Pt', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of all tracks; p_{T} [GeV] ; # Events',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        # B+
        self.Histograms += [ defineHistogram ( 'BEEK_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B^{+} candidates in event ; n(B^{+}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Pt_K', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of K in B^{+} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Eta_K, BEEK_Phi_K', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of K in B^{+} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BEEK_InvMass_B, BEEK_VtxMass_B', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B^{+} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_InvMass_B', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BEEK_Chi2_B', type = 'TH1F',
                                              title = 'EFBEEXFex - B^{+} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bd
        self.Histograms += [ defineHistogram ( 'BdEEKs_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B_{d} candidates in event ; n(B_{d}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Pt_K', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of K in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Eta_K, BdEEKs_Phi_K', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of K in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Pt_Pi', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of #pi in B_{d} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Eta_Pi, BdEEKs_Phi_Pi', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of #pi in B_{d} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Kstar, BdEEKs_VtxMass_Kstar', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of K^{*} in B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Kstar', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
        #self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Bd, BdEEKs_VtxMass_Bd', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B_{d} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_InvMass_Bd', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Chi2_Kstar', type = 'TH1F',
                                              title = 'EFBEEXFex - K^{*} vertex chi2 in B_{d}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BdEEKs_Chi2_Bd', type = 'TH1F',
                                              title = 'EFBEEXFex - B_{d} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        # Bs
        self.Histograms += [ defineHistogram ( 'BsEEPhi_n', type = 'TH1F',
                                              title = 'EFBEEXFex - number of B_{s} candidates in event ; n(B_{s}) ; # Events',
                                              xbins = 30, xmin = -0.5, xmax = 30-0.5 ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Pt_K1', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of first K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Eta_K1, BsEEPhi_Phi_K1', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of first K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Pt_K2', type = 'TH1F',
                                              title = 'EFBEEXFex - p_{T} of second K in B_{s} ; p_{T} [GeV] ; # Candidates',
                                              xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Eta_K2, BsEEPhi_Phi_K2', type = 'TH2F',
                                              title = 'EFBEEXFex - Eta vs. phi of second K in B_{s} ; #eta ; #phi ; # Candidates',
                                              xbins = 60, xmin = -3.0, xmax = 3.0,
                                              ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Phi1020, BsEEPhi_VtxMass_Phi1020', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of #phi(1020) in B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Phi1020', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
        #self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Bs, BsEEPhi_VtxMass_Bs', type = 'TH2F',
                                              #title = 'EFBEEXFex - InvMass vs. VtxMass of B_{s} ; InvMass [GeV] ; VtxMass [GeV] ; # Candidates',
                                              #xbins = 60, xmin = 4., xmax = 7.,
                                              #ybins = 60, ymin = 4., ymax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'EFBEEXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Chi2_Phi1020', type = 'TH1F',
                                              title = 'EFBEEXFex - #phi(1020) vertex chi2 in B_{s}; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
        self.Histograms += [ defineHistogram ( 'BsEEPhi_Chi2_Bs', type = 'TH1F',
                                              title = 'EFBEEXFex - B_{s} vertex chi2 ; #chi^{2} ; # Candidates',
                                              xbins = 300, xmin = 0., xmax = 300. ) ]
