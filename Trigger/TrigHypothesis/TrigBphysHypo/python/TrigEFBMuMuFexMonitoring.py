# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' No_EventInfo : Not_2_Input_TE : No_RoI_1 : No_RoI_2 : MuonFeature_Fails : BphysColl_Fails '
ERROR_number = 6

ACCEPTANCE_labels = ' Input : Got_RoIs : Mu1 : Mu2 : MuMu : MuMu_Unique : Opp_Charge : Dimuon_Built : InvMass_Cut : SEPARATOR : BphysColl_not_Empty '
ACCEPTANCE_number = 11

class TrigEFBMuMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuFexValidationMonitoring"):
        super(TrigEFBMuMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'EFBMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'EFBMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('DeltaEtaRoIs', type='TH1F', title="DeltaEta_RoIs ",
                                           xbins=50, xmin=0.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('DeltaPhiRoIs', type='TH1F', title="DeltaPhi_RoIs ",
                                           xbins=50, xmin=0.0, xmax=3.2) ]
        self.Histograms += [ defineHistogram('DeltaEtaMuMu', type='TH1F', title="DeltaEta_MuMu ",
                                           xbins=50, xmin=0.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('DeltaPhiMuMu', type='TH1F', title="DeltaPhi_MuMu ",
                                           xbins=50, xmin=0.0, xmax=3.2) ]
        self.Histograms += [ defineHistogram('pTsumMuMu', type='TH1F', title="sumPtMu1Mu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu1', type='TH1F', title="PtMu1 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu2', type='TH1F', title="PtMu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('etaMu1, phiMu1', type = 'TH2F', title = 'Mu1 eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('etaMu2, phiMu2', type = 'TH2F', title = 'Mu2 eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('BmassFit', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('VtxChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]

class TrigEFBMuMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuFexOnlineMonitoring"):
        super(TrigEFBMuMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'EFBMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'EFMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('DeltaEtaRoIs', type='TH1F', title="DeltaEta_RoIs ",
                                           xbins=50, xmin=0.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('DeltaPhiRoIs', type='TH1F', title="DeltaPhi_RoIs ",
                                           xbins=50, xmin=0.0, xmax=3.2) ]
        self.Histograms += [ defineHistogram('DeltaEtaMuMu', type='TH1F', title="DeltaEta_MuMu ",
                                           xbins=50, xmin=0.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('DeltaPhiMuMu', type='TH1F', title="DeltaPhi_MuMu ",
                                           xbins=50, xmin=0.0, xmax=3.2) ]
        self.Histograms += [ defineHistogram('pTsumMuMu', type='TH1F', title="sumPtMu1Mu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu1', type='TH1F', title="PtMu1 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu2', type='TH1F', title="PtMu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('etaMu1, phiMu1', type = 'TH2F', title = 'Mu1 eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('etaMu2, phiMu2', type = 'TH2F', title = 'Mu2 eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('BmassFit', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('VtxChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]


class TrigEFBMuMuFexOnlineMonitoring_passEF(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuFexOnlineMonitoring_passEF"):
        super(TrigEFBMuMuFexOnlineMonitoring_passEF, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'EFBMuMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'EFMuMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
