# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DijetAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DijetAllTE_Validation"):
        super(TrigL2DijetAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('nInputTEsAllTE', type='TH1F', title="L2DijetAllTE number of input TE", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nTEinTE0', type='TH1F', title="L2DijetAllTE number of TEs in tes_in[0]", xbins=20, xmin=0, xmax=20)]
        self.Histograms += [ defineHistogram('nTEinTE1', type='TH1F', title="L2DijetAllTE number of TEs in tes_in[1]", xbins=20, xmin=0, xmax=20)]
        self.Histograms += [ defineHistogram('ptHighChain_0', type='TH1F', title="L2DijetAllTE pt of leading jet in high pt chain (GeV)", xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('ptLowChain_0', type='TH1F', title="L2DijetAllTE pt of leading jet in low pt chain (GeV)", xbins=200, xmin=0.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('nLowChainHigh', type='TH1F', title="L2DijetAllTE number of events leading jet in low pt chain is not found in high pt chain", xbins=6, xmin=0, xmax=6)]
        
class TrigL2DijetAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DijetAllTE_Online"):
        """ defines histograms for online """
        super(TrigL2DijetAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('nInputTEsAllTE', type='TH1F', title="L2DijetAllTE number of input TE", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nTEinTE0', type='TH1F', title="L2DijetAllTE number of TEs in tes_in[0]", xbins=20, xmin=0, xmax=20)]
        self.Histograms += [ defineHistogram('nTEinTE1', type='TH1F', title="L2DijetAllTE number of TEs in tes_in[1]", xbins=20, xmin=0, xmax=20)]
        self.Histograms += [ defineHistogram('ptHighChain_0', type='TH1F', title="L2DijetAllTE pt of leading jet in high pt chain (GeV)", xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('ptLowChain_0', type='TH1F', title="L2DijetAllTE pt of leading jet in low pt chain (GeV)", xbins=200, xmin=0.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('ptLowChain_1', type='TH1F', title="L2DijetAllTE pt of next leading jet in low pt chain (GeV)", xbins=200, xmin=0.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('nLowChainHigh', type='TH1F', title="L2DijetAllTE number of events leading jet in low pt chain is not found in high pt chain", xbins=6, xmin=0, xmax=6)]


