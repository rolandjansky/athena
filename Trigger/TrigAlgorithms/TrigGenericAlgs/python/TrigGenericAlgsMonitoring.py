# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class L1CorrelationAlgoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L1CorrelationAlgoMonitoring"):
        super(L1CorrelationAlgoMonitoring, self).__init__(name)
        self.defineTarget(["Online","Validation"])
        self.Histograms = [ defineHistogram('EF_L1Corr_beforeafterflag',type='TH1F', title="beforeafterflag", xbins=4, xmin=-1.5, xmax=2.5),
                            defineHistogram('EF_L1Corr_l1a_type, EF_L1Corr_other_type',type='TH2F', title="typeMatrix ; L1A; Other", xbins=8, xmin=-0.5, xmax=7.5, ybins=8, ymin=-0.5, ymax=7.5),
                            defineHistogram('EF_etaOutOfTimeMuon, EF_phiOutOfTimeMuon',type='TH2F', title="lateMuon_etaphi ; #eta; #phi", xbins=20, xmin=-3.5, xmax=3.5, ybins=20, ymin=-3.15, ymax=3.15),
        ]
    
