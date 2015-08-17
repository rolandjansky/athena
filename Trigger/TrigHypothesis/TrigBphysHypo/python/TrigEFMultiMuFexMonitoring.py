# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' No_EventInfo : WrongNum_Input_TE : GetMuonFailed : BphysColl_Fails '
ERROR_number = 4

ACCEPTANCE_labels = ' Input : Got_Muons : InvMass_Cut  '
ACCEPTANCE_number = 3

class TrigEFMultiMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuFexValidationMonitoring"):
        super(TrigEFMultiMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'EFMultiMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'EFMultiMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('pTMu1', type='TH1F', title="PtMu1 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu2', type='TH1F', title="PtMu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('VtxChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]



class TrigEFMultiMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuFexOnlineMonitoring"):
        super(TrigEFMultiMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'EFMultiMuFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'EFMultiMuFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('pTMu1', type='TH1F', title="PtMu1 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('pTMu2', type='TH1F', title="PtMu2 [GeV]",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('VtxChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]
