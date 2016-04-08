# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigSecVtxFinderMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigSecVtxFinderMonitoring"):
        super(TrigSecVtxFinderMonitoring, self).__init__(name)

        self.Histograms += [ defineHistogram('SecVtx_Num', type='TH1F', title="Number of reconstructed secondary vertices per RoI",
                                             xbins=2, xmin=0.0, xmax=2.0) ]
        self.Histograms += [ defineHistogram('SecVtx_Mass', type='TH1F', title="Mass of the reconstructed secondary vertex [MeV]",
                                             xbins=40, xmin=0.0, xmax=6000.0) ]
        self.Histograms += [ defineHistogram('SecVtx_TwoTrk', type='TH1F', title="Number of 2-track vertices",
                                             xbins=15, xmin=0.0, xmax=15.0) ]
        self.Histograms += [ defineHistogram('SecVtx_NumTrk', type='TH1F', title="Number of tracks associated to the reconstructed secondary vertex",
                                             xbins=15, xmin=0.0, xmax=15.0) ]
        self.Histograms += [ defineHistogram('SecVtx_Energy', type='TH1F', title="Energy fraction of the reconstructed secondary vertex E(SecVtx)/E(Jet)",
                                             xbins=25, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('SecVtx_DecayLength', type='TH1F', title="Decay length of the reconstructed secondary vertex [mm]",
                                             xbins=25, xmin=0.0, xmax=25.0) ]
        self.Histograms += [ defineHistogram('SecVtx_DecayLengthSig', type='TH1F', title="Decay length significance of the reconstructed secondary vertex",
                                             xbins=25, xmin=0.0, xmax=25.0) ]        


class TrigSecVtxFinderValidationMonitoring(TrigSecVtxFinderMonitoring):
    def __init__ (self, name="TrigSecVtxFinderValidationMonitoring"):
        super(TrigSecVtxFinderValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")


class TrigSecVtxFinderOnlineMonitoring(TrigSecVtxFinderMonitoring):
    def __init__ (self, name="TrigSecVtxFinderOnlineMonitoring"):
        super(TrigSecVtxFinderOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
