# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigVxSecondaryMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigVxSecondaryMonitoring"):
        super (InDetTrigVxSecondaryMonitoring, self).__init__(name)

        self.Histograms += [ defineHistogram('SecVtx_NumTrkTot', type='TH1F', title="Total number of tracks from container",
                                             xbins=50, xmin=0.0, xmax=50.0) ]
        # Consider putting this back in at some point ???
        # (If so, needs to be added to InDetTrigVxSecondary)
        #self.Histograms += [ defineHistogram('SecVtx_BadTrkTot', type='TH1F', title="Number of bad tracks from VKalVertex",
        #                                     xbins=10, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('SecVtx_TwoTrkTot', type='TH1F', title="Number of 2-track vertices",
                                             xbins=10, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('SecVtx_NumTrkSV', type='TH1F', title="Number of tracks associated to the reconstructed secondary vertex",
                                             xbins=15, xmin=0.0, xmax=15.0) ]
        self.Histograms += [ defineHistogram('SecVtx_NumPV', type='TH1F', title="Number of reconstructed primary vertices",
                                             xbins=10, xmin=0.0, xmax=10.0) ]                
        self.Histograms += [ defineHistogram('SecVtx_NumSV', type='TH1F', title="Number of reconstructed secondary vertices",
                                             xbins=5, xmin=0.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('SecVtx_Mass', type='TH1F', title="Mass of the reconstructed secondary vertex [MeV]",
                                             xbins=40, xmin=0.0, xmax=6000.0) ]
        self.Histograms += [ defineHistogram('SecVtx_Energy', type='TH1F', title="Energy fraction of the reconstructed secondary vertex E(SecVtx)/E(Jet)",
                                             xbins=25, xmin=0.0, xmax=1.0) ]



class InDetTrigVxSecondaryOnlineMonitoring(InDetTrigVxSecondaryMonitoring):
    def __init__(self, name="InDetTrigVxSecondaryOnlineMonitoring"):
        super (InDetTrigVxSecondaryOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")



class InDetTrigVxSecondaryValidationMonitoring(InDetTrigVxSecondaryMonitoring):
    def __init__(self, name="InDetTrigVxSecondaryValidationMonitoring"):
        super (InDetTrigVxSecondaryValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")


