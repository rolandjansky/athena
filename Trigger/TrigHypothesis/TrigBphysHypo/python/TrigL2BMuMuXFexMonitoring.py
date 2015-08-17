# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' No_EventInfo : No_RoIs : No_CombineMuon : No_TrackColl : Charge_Check : MuMu_MassCut : AddMuonTrack_Fails : MuonVtxFit_Fails : CalcMuonVtxMass_Fails : MuonVtx_MassCut : AddTrack_Fails : VtxFitFailed : MotherVtxFitFailed : TooManyComb_Acc : TooManyComb_Rej'
ERROR_number = 15

ACCEPTANCE_labels = ' Input : AcceptAll : Got_RoIs : Got_Muons :Got_Mu1Track : GotMu2Track : Got_TrackColl : Got_FirstTrackCol: Full_TrackColl : Full_IDTracks : Pass_OppChargeC : MuonTracks_Added : Muon_Vertexing : CalcInvMass : MuonVtx_Part : MuMu_mass : MotherVtxCreated : BphysCollParticle '
ACCEPTANCE_number = 18

class TrigL2BMuMuXFexValidationMonitoring(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexValidation"):
          super(TrigL2BMuMuXFexValidationMonitoring, self).__init__(name)
          self.defineTarget("Validation")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50.,
                                               ybins = 100, ymin = 0., ymax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

class TrigL2BMuMuXFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexOnline"):
          super(TrigL2BMuMuXFexOnlineMonitoring, self).__init__(name)
          self.defineTarget("Online")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

class TrigL2BMuMuXFexValidationMonitoring_noVtx(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexValidation_noVtx"):
          super(TrigL2BMuMuXFexValidationMonitoring_noVtx, self).__init__(name)
          self.defineTarget("Validation")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50.,
                                               ybins = 100, ymin = 0., ymax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

class TrigL2BMuMuXFexOnlineMonitoring_noVtx(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexOnline_noVtx"):
          super(TrigL2BMuMuXFexOnlineMonitoring_noVtx, self).__init__(name)
          self.defineTarget("Online")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

class TrigL2BMuMuXFexValidationMonitoring_noVtx_noOS(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexValidation_noVtx_noOS"):
          super(TrigL2BMuMuXFexValidationMonitoring_noVtx_noOS, self).__init__(name)
          self.defineTarget("Validation")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Total processing time ; t_{tot} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'VertexingTime', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Vertex fit processing time ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'TotalRunTime, VertexingTime', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Total vs. vertex fit processing time ; t_{tot} [ms] ; t_{vtx} [ms] ; # Events',
                                               xbins = 100, xmin = 0., xmax = 50.,
                                               ybins = 100, ymin = 0., ymax = 50. ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

class TrigL2BMuMuXFexOnlineMonitoring_noVtx_noOS(TrigGenericMonitoringToolConfig):
     def __init__ (self, name="L2BMuMuXFexOnline_noVtx_noOS"):
          super(TrigL2BMuMuXFexOnlineMonitoring_noVtx_noOS, self).__init__(name)
          self.defineTarget("Online")
          self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
          self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
          self.Histograms += [ defineHistogram ( 'ROIsEta, ROIsPhi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'EtaTracks, PhiTracks', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of all tracks ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Eta , Muon1Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon+ tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Eta , Muon2Phi', type = 'TH2F',
                                               title = 'L2BMuMuXFex - Eta vs. phi of muon- tracks from Bphys decay (no vertexing); #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
          self.Histograms += [ defineHistogram ( 'Muon1Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Positive muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'Muon2Pt', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Negative muons p_{T} (no vertexing); p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
          self.Histograms += [ defineHistogram ( 'nTracks', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of tracks in RoI; # tracks ; # Events',
                                               xbins = 120, xmin = -0.5, xmax = 119.5 ) ]
          #B+
          self.Histograms += [ defineHistogram ( 'BMuMuK_InvMass_B', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B^{+} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bd
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMassBd_Ks', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of K^{*} in B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_InvMass_Bd', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{d} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bs
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMassBs_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi(1020) in B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_InvMass_Bs', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{s} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Lambda_b
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMassLb_L', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda in #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 0.5, xmax = 2. ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_InvMass_Lb', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #Lambda_{b} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 4., xmax = 7. ) ]
          #Bc
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Phi', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of #phi in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 200, xmin = 0.9, xmax = 1.3 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMassBc_Ds', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of D_{s} in B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 60, xmin = 1.4, xmax = 2.6 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_InvMass_Bc', type = 'TH1F',
                                              title = 'L2BMuMuXFex - InvMass of B_{c} ; InvMass [GeV] ; # Candidates',
                                              xbins = 120, xmin = 1.5, xmax = 7.5 ) ]
          #B particles counters
          self.Histograms += [ defineHistogram ( 'nTriedCombinations', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of track combinations tried ; # combinations ; # Events',
                                               xbins = 500, xmin = 0., xmax = 25000. ) ]
          self.Histograms += [ defineHistogram ( 'nBphys', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of rare B decay candidates ; # rare B ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BMuMuK_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B^{+} decay candidates ; # B^{+} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BdMuMuKs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{d} decay candidates ; # B_{d} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BsMuMuPhi_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{s} decay candidates ; # B_{s} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'LbMuMuL_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of #Lambda_{b} decay candidates ; # #Lambda_{b} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
          self.Histograms += [ defineHistogram ( 'BcMuMuDs_n', type = 'TH1F',
                                               title = 'L2BMuMuXFex - Number of B_{c} decay candidates ; # B_{c} ; # Events',
                                               xbins = 50, xmin = -0.5, xmax = 49.5 ) ]
#          self.Histograms += [ defineHistogram ( 'Chi2toNDoF_Bp', type = 'TH1F',
#                                               title = 'L2BMuMuXFex - Vertex fit #chi^{2}/_{NDoF}-probability ; prob(#chi^{2}/_{NDoF}) ; # Candidates',
#                                               xbins = 100, xmin = 0., xmax = 1. ) ]

