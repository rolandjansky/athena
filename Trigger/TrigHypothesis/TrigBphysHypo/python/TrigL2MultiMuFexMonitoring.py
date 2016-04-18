# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

ERROR_labels = ' No_EventInfo : WrongNum_Input_TE :GetMuonFailed : AddTrack_Fails : CalcInvMass_Fails :  BphysColl_Fails '
ERROR_number = 6

ACCEPTANCE_labels = ' Input : Got_Muons : InvMass_Cut  '
ACCEPTANCE_number = 3

class TrigL2MultiMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MultiMuFexValidationMonitoring"):
        super(TrigL2MultiMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2MultiMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2MultiMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt', type = 'TH1F',
                                               title = 'L2MultiMuFex - Muon ID-Track p_{T} - low range ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkEta, MutrkPhi', type = 'TH2F',
                                               title = 'L2MultiMuFex - Muon ID-Track direction ; #eta ; #phi ; # Muons with ID tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb', type = 'TH1F',
                                               title = 'L2MultiMuFex - Invariant mass of the Muon ID-Tracks - low range ; Mass [GeV] ; # Di-muons',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb_wideRange', type = 'TH1F',
                                               title = 'L2MultiMuFex - Invariant mass of the Muon ID-Tracks ; Mass [GeV] ; # Di-muons',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted mass - low range ; Mass [GeV] ; # Candidates',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass_wideRange', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted mass ; Mass [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'Chi2toNDoF', type = 'TH1F',
                                               title = 'L2MultiMuFex - Vertex fit #chi^{2}/_{NDoF} ; #chi^{2}/_{NDoF} ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'FitTotalPt', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted di-muon p_{T} ; p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxR', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted vertex R-position ; R [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxZ', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted vertex Z-position ; Z [mm] ; # Candidates',
                                               xbins = 100, xmin = -500., xmax = 500. ) ]

class TrigL2MultiMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MultiMuFexOnlineMonitoring"):
        super(TrigL2MultiMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2MultiMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2MultiMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'MutrkPt', type = 'TH1F',
                                               title = 'L2MultiMuFex - Muon ID-Track p_{T} - low range ; p_{T} [GeV] ; # Muons with ID tracks',
                                               xbins = 50, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'MutrkEta, MutrkPhi', type = 'TH2F',
                                               title = 'L2MultiMuFex - Muon ID-Track direction ; #eta ; #phi ; # Muons with ID tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb', type = 'TH1F',
                                               title = 'L2MultiMuFex - Invariant mass of the Muon ID-Tracks - low range ; Mass [GeV] ; # Di-muons',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'InvMass_comb_wideRange', type = 'TH1F',
                                               title = 'L2MultiMuFex - Invariant mass of the Muon ID-Tracks ; Mass [GeV] ; # Di-muons',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted mass - low range ; Mass [GeV] ; # Candidates',
                                               xbins = 120, xmin = 0., xmax = 13. ) ]
        self.Histograms += [ defineHistogram ( 'FitMass_wideRange', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted mass ; Mass [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]                                                                                         
        self.Histograms += [ defineHistogram ( 'Chi2toNDoF', type = 'TH1F',
                                               title = 'L2MultiMuFex - Vertex fit #chi^{2}/_{NDoF} ; #chi^{2}/_{NDoF} ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 25. ) ]
        self.Histograms += [ defineHistogram ( 'FitTotalPt', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted di-muon p_{T} ; p_{T} [GeV] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 100. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxR', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted vertex R-position ; R [mm] ; # Candidates',
                                               xbins = 100, xmin = 0., xmax = 10. ) ]
        self.Histograms += [ defineHistogram ( 'FitVtxZ', type = 'TH1F',
                                               title = 'L2MultiMuFex - Fitted vertex Z-position ; Z [mm] ; # Candidates',
                                               xbins = 100, xmin = -500., xmax = 500. ) ]

class TrigL2MultiMuFexOnlineMonitoring_passL2(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MultiMuFexOnlineMonitoring_passL2"):
        super(TrigL2MultiMuFexOnlineMonitoring_passL2, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'L2MultiMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'L2MultiMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
