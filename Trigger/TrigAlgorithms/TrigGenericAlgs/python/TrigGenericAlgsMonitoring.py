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
    


class DetectorTimingAlgoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="DetectorTimingAlgoMonitoring"):
        super(DetectorTimingAlgoMonitoring, self).__init__(name)
        self.defineTarget(["Online","Validation"])
        self.Histograms = [# defineHistogram('EF_ootfrac_bc',type='TH1F', title="ootfrac_bc", xbins=50, xmin=0., xmax=1),
                            defineHistogram('EF_ootfrac_bc_10',type='TH1F', title="ootfrac_bc_10", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_ootfrac_bc_25',type='TH1F', title="ootfrac_bc_25", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_ootfrac_bc_50',type='TH1F', title="ootfrac_bc_50", xbins=50, xmin=0., xmax=1),
                            
                           # defineHistogram('EF_ootfrac',type='TH1F', title="ootfrac", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_itfrac',type='TH1F', title="itfrac", xbins=50, xmin=0., xmax=1),
                            defineHistogram('EF_L1AClusterTime_weighted',type='TH1F', title="L1AClusterTime_weighted", xbins=50, xmin=-35., xmax=35),
                            defineHistogram('EF_L1AClusterTime_max_2',type='TH1F', title="L1AClusterTime_max_2", xbins=50, xmin=-35., xmax=35),
                            defineHistogram('EF_L1AClusterTime_max_10',type='TH1F', title="L1AClusterTime_max_10", xbins=50, xmin=-35., xmax=35),
                            defineHistogram('EF_L1AClusterTime_accept',type='TH1F', title="L1AClusterTime_accept", xbins=2, xmin=-0.5, xmax=1.5),

                          #  defineHistogram('EF_clustertime_5',type='TH1F', title="clustertime_5", xbins=50, xmin=-35., xmax=35),
                            defineHistogram('EF_clustertime_10',type='TH1F', title="clustertime_10", xbins=50, xmin=-35., xmax=35),
                          #  defineHistogram('EF_clustertime_25',type='TH1F', title="clustertime_25", xbins=50, xmin=-35., xmax=35),
                          #  defineHistogram('EF_clustertime_50',type='TH1F', title="clustertime_50", xbins=50, xmin=-35., xmax=35),

                            defineHistogram('EF_L1ACluster_eta, EF_L1ACluster_phi',type='TH2F', title="L1AClusterEtaPhi ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),
                          #  defineHistogram('EF_L1ACluster_eta_funnypeak, EF_L1ACluster_phi_funnypeak',type='TH2F', title="L1AClusterEtaPhi_funnypeak ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),

                            defineHistogram('EF_L1ACluster_eta, EF_L1ACluster_t',type='TH2F', title="L1AClusterEtaTime ; eta; t",xbins=20, xmin=-4.5, xmax=4.5, ybins=30, ymin=-30, ymax=30),
                          #  defineHistogram('EF_L1ACluster_eta_funnypeak, EF_L1ACluster_t_funnypeak',type='TH2F', title="L1AClusterEtaTime_funnypeak ; eta; t",xbins=20, xmin=-4.5, xmax=4.5, ybins=30, ymin=-30, ymax=30),


                            defineHistogram('EF_ootfrac_bc_10, EF_L1AClusterTime_weighted',type='TH2F', title="ootfrac_bc_10vsL1AClusterTime ; ootfrac; L1AClusterTime", xbins=50, xmin=0., xmax=1, ybins=50, ymin=-35., ymax=35),
                            

                          



        ]
    
