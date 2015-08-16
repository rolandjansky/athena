# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2JetDEtaHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigL2JetDEtaHypoValidationMonitoring"):
        super(TrigL2JetDEtaHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms= [defineHistogram('GapCounter', type='TH1F', title="L2JetDEtaHypo gap counter", xbins=6, xmin=-1.5, xmax=4.5)]
        self.Histograms+=[defineHistogram('GapSize',    type='TH1F', title="L2JetDEtaHypo gap size",    xbins=50, xmin=-0, xmax=10.)]
        
class TrigL2JetDEtaHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigL2JetDEtaHypoOnlineMonitoring"):
        super(TrigL2JetDEtaHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms= [defineHistogram('GapCounter', type='TH1F', title="L2JetDEtaHypo gap counter", xbins=6, xmin=-1.5, xmax=4.5)]
        self.Histograms+=[defineHistogram('GapSize',    type='TH1F', title="L2JetDEtaHypo gap size",    xbins=50, xmin=-0, xmax=10.)]


class TrigEFJetDEtaHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFJetDEtaHypoValidationMonitoring"):
        super(TrigEFJetDEtaHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms= [defineHistogram('GapCounter', type='TH1F', title="EFJetDEtaHypo gap counter", xbins=6, xmin=-1.5, xmax=4.5)]
        self.Histograms+=[defineHistogram('GapSize',    type='TH1F', title="EFJetDEtaHypo gap size",    xbins=50, xmin=-0, xmax=10.)]
        
class TrigEFJetDEtaHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFJetDEtaHypoOnlineMonitoring"):
        super(TrigEFJetDEtaHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms= [defineHistogram('GapCounter', type='TH1F', title="EFJetDEtaHypo gap counter", xbins=6, xmin=-1.5, xmax=4.5)]
        self.Histograms+=[defineHistogram('GapSize',    type='TH1F', title="EFJetDEtaHypo gap size",    xbins=50, xmin=-0, xmax=10.)]
