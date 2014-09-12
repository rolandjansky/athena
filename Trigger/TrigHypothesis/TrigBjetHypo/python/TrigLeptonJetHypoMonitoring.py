# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigLeptonJetHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLeptonJetHypoMonitoring"):
        super(TrigLeptonJetHypoMonitoring, self).__init__(name)
         
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="Bjet Hypo cut counter",
                                            xbins=3, xmin=-1.5, xmax=1.5,
                                            labels='No Bjet obj:Rejected:Accepted') ]



class TrigL2LeptonJetHypoValidationMonitoring(TrigLeptonJetHypoMonitoring):
    def __init__ (self, name="TrigL2LeptonJetHypoValidationMonitoring"):
        """ defines hisograms for online """
        super(TrigL2LeptonJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")



class TrigL2LeptonJetHypoOnlineMonitoring(TrigLeptonJetHypoMonitoring):
    def __init__ (self, name="TrigL2LeptonJetHypoOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2LeptonJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")



class TrigEFLeptonJetHypoValidationMonitoring(TrigLeptonJetHypoMonitoring):
    def __init__ (self, name="TrigEFLeptonJetHypoValidationMonitoring"):
        super(TrigEFLeptonJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFLeptonJetHypoOnlineMonitoring(TrigLeptonJetHypoMonitoring):
    def __init__ (self, name="TrigEFLeptonJetHypoOnlineMonitoring"):
        super(TrigEFLeptonJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
