# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DijetComboValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DijetCombo_Validation"):
        super(TrigL2DijetComboValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('nTe2jCounter', type='TH1F', title="L2DijetCombo number of input TE", xbins=6, xmin=0, xmax=6)]


class TrigL2DijetComboOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DijetCombo_Online"):
        """ defines histograms for online """
        super(TrigL2DijetComboOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('nTe2jCounter', type='TH1F', title="L2DijetCombo number of input TE", xbins=6, xmin=0, xmax=6)]


class TrigL2DijetComboCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DijetCombo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2DijetComboCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('nTe2jCounter', type='TH1F', title="L2DijetCombo number of input TE", xbins=6, xmin=0, xmax=6)]

