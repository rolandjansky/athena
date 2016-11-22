# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2JetDEtaFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigL2JetDEtaFexValidationMonitoring"):
        super(TrigL2JetDEtaFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [defineHistogram('GapSize', type='TH1F', title="L2JetDEtaFex gap size", xbins=11, xmin=-1.5, xmax=9.5)]
        self.Histograms +=[defineHistogram('JetEta', type='TH1F', title="L2JetDEtaFex jet eta", xbins=50, xmin=-5., xmax=5.)]
        self.Histograms +=[defineHistogram('NJets', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms +=[defineHistogram('NJetsSelected', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]

        
        
class TrigL2JetDEtaFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigL2JetDEtaFexOnlineMonitoring"):
        super(TrigL2JetDEtaFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [defineHistogram('GapSize', type='TH1F', title="L2JetDEtaFex gap size", xbins=11, xmin=-1.5, xmax=9.5)]
        self.Histograms +=[defineHistogram('JetEta', type='TH1F', title="L2JetDEtaFex jet eta", xbins=50, xmin=-5., xmax=5.)]
        self.Histograms +=[defineHistogram('NJets', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms +=[defineHistogram('NJetsSelected', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        
class TrigEFJetDEtaFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFJetDEtaFexValidationMonitoring"):
        super(TrigEFJetDEtaFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [defineHistogram('GapSize', type='TH1F', title="EFJetDEtaFex gap size", xbins=11, xmin=-1.5, xmax=9.5)]
        self.Histograms +=[defineHistogram('JetEta', type='TH1F', title="EFJetDEtaFex jet eta", xbins=50, xmin=-5., xmax=5.)]
        self.Histograms +=[defineHistogram('NJets', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms +=[defineHistogram('NJetsSelected', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        
class TrigEFJetDEtaFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFJetDEtaFexOnlineMonitoring"):
        super(TrigEFJetDEtaFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [defineHistogram('GapSize', type='TH1F', title="EFJetDEtaFex gap size", xbins=11, xmin=-1.5, xmax=9.5)]
        self.Histograms +=[defineHistogram('JetEta', type='TH1F', title="EFJetDEtaFex jet eta", xbins=50, xmin=-5., xmax=5.)]
        self.Histograms +=[defineHistogram('NJets', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms +=[defineHistogram('NJetsSelected', type='TH1F', title="L2JetDEtaFex n jets", xbins=20, xmin=-0.5, xmax=19.5)]
