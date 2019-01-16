# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ERROR_labels = ' AlgorithmProblem : BphysColl_Fails '
ERROR_number = 2

ACCEPTANCE_labels = ' Input : HltExecute : PassNL2SAMuons : PassNL2CombMuons : PassNEFMuons : PassNTracks : HighPtTrack : NTrkCharge : NTrkMass : PairCharge : PairMass : RecordedCollection '
ACCEPTANCE_number = 13

class TrigMultiTrkFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMultiTrkFexValidationMonitoring"):
        super(TrigMultiTrkFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'MultiTrkFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'MultiTrkFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('NTrkMass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrkFitMass', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrkChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('PairMass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrk_all', type='TH1F', title="Number of tracks",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NTrk_highpt', type='TH1F', title="Number of high pt tracks",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NTrkHighPt_accepted', type='TH1F', title="Number of high pt tracks in accepted events",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NPair_all', type='TH1F', title="Number of hight pt pairs",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NPair_accepted', type='TH1F', title="Number of hight pt pairs (accepted)",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('logNCombinations_processed', type='TH1F', title="Log10 (Number of combinations processed)",
                                           xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('logNCombinations_accepted', type='TH1F', title=" Log10 (Number of combinations accepted)",
                                           xbins=50, xmin=0., xmax=5.) ]



class TrigMultiTrkFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMultiTrkFexOnlineMonitoring"):
        super(TrigMultiTrkFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram ( 'Errors', type = 'TH1F',
                                               title = 'MultiTrkFex - Algorithm errors ; ; # Events / Candidates',
                                               xbins = ERROR_number , xmin = -0.5, xmax = ERROR_number - 0.5,
                                               labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'MultiTrkFex - Reached steps - acceptance ; ; # Events / Candidates',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('NTrkMass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrkFitMass', type='TH1F', title=" FitBMass [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrkChi2', type='TH1F', title="Chi2 of vertex fit",
                                           xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('PairMass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NTrk_all', type='TH1F', title="Number of tracks",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NTrk_highpt', type='TH1F', title="Number of high pt tracks",
                                           xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('NTrkHighPt_accepted', type='TH1F', title="Number of high pt tracks (accepted)",
                                           xbins=100, xmin=0., xmax=100.) ]
        #self.Histograms += [ defineHistogram('NPair_all', type='TH1F', title="Number of hight pt pairs",
        #                                   xbins=100, xmin=0., xmax=100.) ]
        #self.Histograms += [ defineHistogram('NPair_accepted', type='TH1F', title="Number of hight pt pairs (accepted)",
        #                                   xbins=100, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('logNCombinations_processed', type='TH1F', title="Log10 (Number of combinations processed)",
                                           xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('logNCombinations_accepted', type='TH1F', title=" Log10 (Number of combinations accepted)",
                                           xbins=50, xmin=0., xmax=5.) ]
