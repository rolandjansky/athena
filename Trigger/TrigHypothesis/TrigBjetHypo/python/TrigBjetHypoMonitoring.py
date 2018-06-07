# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigBjetHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBjetHypoMonitoring"):
        super(TrigBjetHypoMonitoring, self).__init__(name)
         
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="Bjet Hypo cut counter",
                                            xbins=4, xmin=-1.5, xmax=2.5,
                                            labels='No Bjet obj:No usable beam spot:Rejected:Accepted') ]
        self.Histograms += [ defineHistogram('Method', type='TH1F', title="Method used to perform the cut",
                                             xbins=4, xmin=0.0, xmax=4.0,
                                             labels='CHI2:IP2D:IP3D:COMB') ]



class TrigEFBjetHypoValidationMonitoring(TrigBjetHypoMonitoring):
    def __init__ (self, name="TrigEFBjetHypoValidationMonitoring"):
        super(TrigEFBjetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFBjetHypoOnlineMonitoring(TrigBjetHypoMonitoring):
    def __init__ (self, name="TrigEFBjetHypoOnlineMonitoring"):
        super(TrigEFBjetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")


        


