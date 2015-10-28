# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

ERROR_labels = ' No_EventInfo : Not_2_Input_TE : No_RoI_1 : No_RoI_2 : Mu1_not_Combined : Mu1_not_Standalone : Mu1_Standalone_Invalid : Mu2_not_Combined : Mu2_not_Standalone : Mu2_Standalone_Invalid : MuMu_not_Both_Combined : MuMu_not_Comb_Standalone : AddTrack_Fails : CalcInvMass_Fails : CalcMother_Fails : CalcMassPull_Fails : BphysColl_Fails '
ERROR_number = 17

ACCEPTANCE_labels = ' Input : Got_RoIs : Mu1_Combined : Mu1_Standalone : Mu2_Combined : Mu2_Standalone : MuMu_Both_Combined : MuMu_Comb_Standalone : Mu1_IDTrack : Mu2_IDTrack : MuMu_Both_IDTracks : CombMu_IDTrack : DiMuon_Pair : vtxpass_is_TRUE : mumuIDpass_is_TRUE : Opp_Charge : InvMass_Cut : AddTrack_Test : Vertexing : CalcInvMass : CalcMother : Vertex_not_NULL : SEPARATOR : BphysColl_not_Empty '
ACCEPTANCE_number = 24

class TrigL2BMuMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuFexValidationMonitoring"):
        super(TrigL2BMuMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'ROIEta, ROIPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta, Roi1Roi2dPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta#Delta#phi difference ; #Delta#eta ; #Delta#phi ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0,
                                               ybins = 32, ymin = 0., ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta difference ; #Delta#eta ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dPhi', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#phi difference ; #Delta#phi ; # ROIs',
                                               xbins = 32, xmin = 0., xmax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dR', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #DeltaR difference ; #DeltaR ; # ROIs',
                                               xbins = 50, xmin = 0., xmax = 5. ) ]
        self.Histograms += [ defineHistogram ( 'MucombROIdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-ROI #DeltaR difference ; #DeltaR ; # Combined muons',
                                               xbins = 50, xmin = 0., xmax = 1. ) ]
        self.Histograms += [ defineHistogram ( 'MucombTrkdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-IDTrack #DeltaR difference ; #DeltaR ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 0.1 ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} - low range ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 200. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkEta, MutrkPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - Muon ID-Track direction ; #eta ; #phi ; # Muons with ID tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dEta', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #Delta#eta difference ; #Delta#eta ; # Di-muons',
                                               xbins = 30, xmin = 0., xmax = 3.0 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dPhi', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #Delta#phi difference ; #Delta#phi ; # Di-muons',
                                               xbins = 32, xmin = 0., xmax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dR', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #DeltaR difference ; #DeltaR ; # Di-muons',
                                               xbins = 50, xmin = 0., xmax = 5. ) ]
        self.Histograms += [ defineHistogram ( 'SumPtMutrk12', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks sum of p_{T} ; p_{Ttrk1}+p_{Ttrk2} [GeV] ; # Di-muons',
                                               xbins = 50, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb', type = 'TH1F',
                                               title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks - low range ; Mass [GeV] ; # Di-muons',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks ; Mass [GeV] ; # Di-muons',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted mass - low range ; Mass [GeV] ; # Candidates',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted mass ; Mass [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'Chi2toNDoF', type = 'TH1F',
                                               title = 'L2BMuMuFex - Vertex fit #chi^{2}/_{NDoF} ; #chi^{2}/_{NDoF} ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'FitTotalPt', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted di-muon p_{T} ; p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxR', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted vertex R-position ; R [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxZ', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted vertex Z-position ; Z [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 500. ) ]

class TrigL2BMuMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuFexOnlineMonitoring"):
        super(TrigL2BMuMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'ROIEta, ROIPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta, Roi1Roi2dPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta#Delta#phi difference ; #Delta#eta ; #Delta#phi ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0,
                                               ybins = 32, ymin = 0., ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta difference ; #Delta#eta ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dPhi', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#phi difference ; #Delta#phi ; # ROIs',
                                               xbins = 32, xmin = 0., xmax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dR', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #DeltaR difference ; #DeltaR ; # ROIs',
                                               xbins = 50, xmin = 0., xmax = 5. ) ]
        self.Histograms += [ defineHistogram ( 'MucombROIdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-ROI #DeltaR difference ; #DeltaR ; # Combined muons',
                                               xbins = 50, xmin = 0., xmax = 1. ) ]
        self.Histograms += [ defineHistogram ( 'MucombTrkdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-IDTrack #DeltaR difference ; #DeltaR ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 0.1 ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} - low range ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 200. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkEta, MutrkPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - Muon ID-Track direction ; #eta ; #phi ; # Muons with ID tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dEta', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #Delta#eta difference ; #Delta#eta ; # Di-muons',
                                               xbins = 30, xmin = 0., xmax = 3.0 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dPhi', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #Delta#phi difference ; #Delta#phi ; # Di-muons',
                                               xbins = 32, xmin = 0., xmax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dR', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks #DeltaR difference ; #DeltaR ; # Di-muons',
                                               xbins = 50, xmin = 0., xmax = 5. ) ]
        self.Histograms += [ defineHistogram ( 'SumPtMutrk12', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Tracks sum of p_{T} ; p_{Ttrk1}+p_{Ttrk2} [GeV] ; # Di-muons',
                                               xbins = 50, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb', type = 'TH1F',
                                               title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks - low range ; Mass [GeV] ; # Di-muons',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks ; Mass [GeV] ; # Di-muons',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted mass - low range ; Mass [GeV] ; # Candidates',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted mass ; Mass [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]                                                                                         
        self.Histograms += [ defineHistogram ( 'Chi2toNDoF', type = 'TH1F',
                                               title = 'L2BMuMuFex - Vertex fit #chi^{2}/_{NDoF} ; #chi^{2}/_{NDoF} ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'FitTotalPt', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted di-muon p_{T} ; p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxR', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted vertex R-position ; R [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxZ', type = 'TH1F',
                                               title = 'L2BMuMuFex - Fitted vertex Z-position ; Z [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 500. ) ]


class TrigL2BMuMuFexOnlineMonitoring_noid(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuFexOnlineMonitoring_noid"):
        super(TrigL2BMuMuFexOnlineMonitoring_noid, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'ROIEta, ROIPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROIs direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta, Roi1Roi2dPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta#Delta#phi difference ; #Delta#eta ; #Delta#phi ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0,
                                               ybins = 32, ymin = 0., ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dEta', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#eta difference ; #Delta#eta ; # ROIs',
                                               xbins = 30, xmin = 0., xmax = 3.0 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dPhi', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #Delta#phi difference ; #Delta#phi ; # ROIs',
                                               xbins = 32, xmin = 0., xmax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'Roi1Roi2dR', type = 'TH1F',
                                               title = 'L2BMuMuFex - ROI_{1}-ROI_{2} #DeltaR difference ; #DeltaR ; # ROIs',
                                               xbins = 50, xmin = 0., xmax = 5. ) ]
        self.Histograms += [ defineHistogram ( 'MucombROIdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-ROI #DeltaR difference ; #DeltaR ; # Combined muons',
                                               xbins = 50, xmin = 0., xmax = 1. ) ]
        self.Histograms += [ defineHistogram ( 'MucombTrkdR', type = 'TH1F',
                                               title = 'L2BMuMuFex - CombinedMuon-IDTrack #DeltaR difference ; #DeltaR ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 0.1 ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} - low range ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt_wideRange', type = 'TH1F',
                                               title = 'L2BMuMuFex - Muon ID-Track p_{T} ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 200. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkEta, MutrkPhi', type = 'TH2F',
                                               title = 'L2BMuMuFex - Muon ID-Track direction ; #eta ; #phi ; # Muons with ID tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dEta', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Muon ID-Tracks #Delta#eta difference ; #Delta#eta ; # Di-muons',
        #                                       xbins = 30, xmin = 0., xmax = 3.0 ) ]
        #self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dPhi', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Muon ID-Tracks #Delta#phi difference ; #Delta#phi ; # Di-muons',
        #                                       xbins = 32, xmin = 0., xmax = 3.2 ) ]
        #self.Histograms += [ defineHistogram ( 'Mutrk1Mutrk2dR', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Muon ID-Tracks #DeltaR difference ; #DeltaR ; # Di-muons',
        #                                       xbins = 50, xmin = 0., xmax = 5. ) ]
        #self.Histograms += [ defineHistogram ( 'SumPtMutrk12', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Muon ID-Tracks sum of p_{T} ; p_{Ttrk1}+p_{Ttrk2} [GeV] ; # Di-muons',
        #                                       xbins = 50, xmin = 0., xmax = 100. ) ]
        #self.Histograms += [ defineHistogram ( 'InvMass_comb', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks - low range ; Mass [GeV] ; # Di-muons',
        #                                       xbins = 120, xmin = 0., xmax = 13. ) ]
        #self.Histograms += [ defineHistogram ( 'InvMass_comb_wideRange', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Invariant mass of the Muon ID-Tracks ; Mass [GeV] ; # Di-muons',
        #                                       xbins = 100, xmin = 0., xmax = 100. ) ]
        #self.Histograms += [ defineHistogram ( 'FitMass', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Fitted mass - low range ; Mass [GeV] ; # Candidates',
        #                                       xbins = 120, xmin = 0., xmax = 13. ) ]
        #self.Histograms += [ defineHistogram ( 'FitMass_wideRange', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Fitted mass ; Mass [GeV] ; # Candidates',
        #                                       xbins = 100, xmin = 0., xmax = 100. ) ]                                                                                         
        #self.Histograms += [ defineHistogram ( 'Chi2toNDoF', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Vertex fit #chi^{2}/_{NDoF} ; #chi^{2}/_{NDoF} ; # Candidates',
        #                                       xbins = 100, xmin = 0., xmax = 25. ) ]
        #self.Histograms += [ defineHistogram ( 'FitTotalPt', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Fitted di-muon p_{T} ; p_{T} [GeV] ; # Candidates',
        #                                       xbins = 100, xmin = 0., xmax = 100. ) ]
        #self.Histograms += [ defineHistogram ( 'FitVtxR', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Fitted vertex R-position ; R [mm] ; # Candidates',
        #                                       xbins = 100, xmin = 0., xmax = 10. ) ]
        #self.Histograms += [ defineHistogram ( 'FitVtxZ', type = 'TH1F',
        #                                       title = 'L2BMuMuFex - Fitted vertex Z-position ; Z [mm] ; # Candidates',
        #                                       xbins = 100, xmin = 0., xmax = 500. ) ]


class TrigL2BMuMuFexOnlineMonitoring_passL2(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuFexOnlineMonitoring_passL2"):
        super(TrigL2BMuMuFexOnlineMonitoring_passL2, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2BMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2BMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]