# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# flake8: noqa  (legacy code)

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class L1CorrelationAlgoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L1CorrelationAlgoMonitoring"):
        super(L1CorrelationAlgoMonitoring, self).__init__(name)
        self.defineTarget(["Online","Validation"])
        self.Histograms = [ defineHistogram('EF_L1Corr_beforeafterflag',type='TH1F', title="beforeafterflag", xbins=4, xmin=-1.5, xmax=2.5),
                            defineHistogram('EF_L1Corr_l1a_type, EF_L1Corr_other_type',type='TH2F', title="typeMatrix ; L1A; Other", xbins=8, xmin=-0.5, xmax=7.5, ybins=8, ymin=-0.5, ymax=7.5),
                            # defineHistogram('EF_etaOutOfTimeMuon, EF_phiOutOfTimeMuon',type='TH2F', title="lateMuon_etaphi ; #eta; #phi", xbins=20, xmin=-3.5, xmax=3.5, ybins=20, ymin=-3.15, ymax=3.15),
        ]
    


class DetectorTimingAlgoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="DetectorTimingAlgoMonitoring"):
        super(DetectorTimingAlgoMonitoring, self).__init__(name)
        self.defineTarget(["Online","Validation"])
        self.Histograms = [# defineHistogram('EF_ootfrac_bc',type='TH1F', title="ootfrac_bc", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_ootfrac_bc_10',type='TH1F', title="ootfrac_bc_10", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_ootfrac_bc_25',type='TH1F', title="ootfrac_bc_25", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_ootfrac_bc_50',type='TH1F', title="ootfrac_bc_50", xbins=50, xmin=0., xmax=1),
                            
                           # defineHistogram('EF_ootfrac',type='TH1F', title="ootfrac", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_itfrac',type='TH1F', title="itfrac", xbins=50, xmin=0., xmax=1),
                           # defineHistogram('EF_L1AClusterTime_weighted',type='TH1F', title="L1AClusterTime_weighted", xbins=50, xmin=-35., xmax=35),
                           # defineHistogram('EF_L1AClusterTime_max_2',type='TH1F', title="L1AClusterTime_max_2", xbins=50, xmin=-35., xmax=35),
                           # defineHistogram('EF_L1AClusterTime_max_10',type='TH1F', title="L1AClusterTime_max_10", xbins=50, xmin=-35., xmax=35),
                           # defineHistogram('EF_L1AClusterTime_accept',type='TH1F', title="L1AClusterTime_accept", xbins=2, xmin=-0.5, xmax=1.5),
            # defineHistogram('EF_clustertime_5',type='TH1F', title="clustertime_5", xbins=50, xmin=-35., xmax=35),
            # defineHistogram('EF_clustertime_10',type='TH1F', title="clustertime_10", xbins=50, xmin=-35., xmax=35),            
            # defineHistogram('EF_clustertime_10_sigma',type='TH1F', title="clustertimesigma_10", xbins=50, xmin=-35., xmax=35),

            # defineHistogram('EF_rpctime_eta, EF_rpctime_phi',type='TH2F', title="RPCHIts ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),

            # defineHistogram('EF_rpceta_matched_intimeroi, EF_rpcphi_matched_intimeroi',type='TH2F', title="RPCHIts_INTIMEROI ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),
            # defineHistogram('EF_rpceta_matched_outtimeroi, EF_rpcphi_matched_outtimeroi',type='TH2F', title="RPCHIts_OUTTIMEROI ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),
            # defineHistogram('EF_rpctime_matched_intimeroi',type='TH1F', title="rpctime_matched_intimeroi", xbins=50, xmin=-35., xmax=35),
            # defineHistogram('EF_rpctime_matched_outtimeroi',type='TH1F', title="rpctime_matched_outtimeroi", xbins=50, xmin=-35., xmax=35),
            defineHistogram('EF_rpctime_matched_intimeroi_averaged',type='TH1F', title="rpctime_matched_intimeroi_averaged", xbins=50, xmin=-35., xmax=35),
            defineHistogram('EF_rpctime_matched_outtimeroi_averaged',type='TH1F', title="rpctime_matched_outtimeroi_averaged", xbins=50, xmin=-35., xmax=35),

#            defineHistogram('EF_rpctime_matched_intimeroi_averaged_NOL1BC',type='TH1F', title="rpctime_matched_intimeroi_averaged_NOL1BC", xbins=50, xmin=-35., xmax=35),
#            defineHistogram('EF_rpctime_matched_outtimeroi_averaged_NOL1BC',type='TH1F', title="rpctime_matched_outtimeroi_averaged_NOL1BC", xbins=50, xmin=-35., xmax=35),

            
            # defineHistogram('EF_rpctime_all',type='TH1F', title="rpctime_all", xbins=50, xmin=-35., xmax=35),
            # defineHistogram('EF_rpctime_005',type='TH1F', title="rpctime_005", xbins=50, xmin=-35., xmax=35),
            # defineHistogram('EF_rpctime_intime',type='TH1F', title="rpctime_intime", xbins=50, xmin=-35., xmax=35),
            # defineHistogram('EF_rpctime_other',type='TH1F', title="rpctime_other", xbins=50, xmin=-35., xmax=35),
            
            #  defineHistogram('EF_clustertime_25',type='TH1F', title="clustertime_25", xbins=50, xmin=-35., xmax=35),
                          #  defineHistogram('EF_clustertime_50',type='TH1F', title="clustertime_50", xbins=50, xmin=-35., xmax=35),

                           # defineHistogram('EF_L1ACluster_eta, EF_L1ACluster_phi',type='TH2F', title="L1AClusterEtaPhi ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),
                          #  defineHistogram('EF_L1ACluster_eta_funnypeak, EF_L1ACluster_phi_funnypeak',type='TH2F', title="L1AClusterEtaPhi_funnypeak ; eta; phi",xbins=20, xmin=-4.5, xmax=4.5, ybins=20, ymin=-3.15, ymax=3.15),

                          #  defineHistogram('EF_L1ACluster_eta, EF_L1ACluster_t',type='TH2F', title="L1AClusterEtaTime ; eta; t",xbins=20, xmin=-4.5, xmax=4.5, ybins=30, ymin=-30, ymax=30),
                          #  defineHistogram('EF_L1ACluster_eta_funnypeak, EF_L1ACluster_t_funnypeak',type='TH2F', title="L1AClusterEtaTime_funnypeak ; eta; t",xbins=20, xmin=-4.5, xmax=4.5, ybins=30, ymin=-30, ymax=30),


                           # defineHistogram('EF_ootfrac_bc_10, EF_L1AClusterTime_weighted',type='TH2F', title="ootfrac_bc_10vsL1AClusterTime ; ootfrac; L1AClusterTime", xbins=50, xmin=0., xmax=1, ybins=50, ymin=-35., ymax=35),
                           
			    ################TriggerTowerStuff#####################
			  #  defineHistogram('EF_TT_median', type='TH1F', title="TriggerTowerTiming_median", xbins=102, xmin=-52, xmax=50),
			    defineHistogram('EF_TT_mean', type='TH1F', title="TriggerTowerTiming_mean", xbins=102, xmin=-52, xmax=50),
			    defineHistogram('EF_TT_sigma', type='TH1F', title="TriggerTowerTiming_sigma", xbins=88, xmin=-2, xmax=20),

                            defineHistogram('EF_mu_mean', type='TH1F', title="MuTime_mean", xbins=102, xmin=-52, xmax=50),
			    defineHistogram('EF_mu_sigma', type='TH1F', title="MuTime_sigma", xbins=88, xmin=-2, xmax=20),
			    defineHistogram('EF_mu_timings', type='TH1F', title="MuTiming",xbins=102, xmin=-52, xmax=50),
                            defineHistogram('EF_mu_mean, EF_mu_sigma', type='TH2F', title="MuMean_vs_sigma ; mean; sigma", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=0, ymax=20.),

                            defineHistogram('EF_Obj_mean', type='TH1F', title="CombinedObjectsTime_mean", xbins=102, xmin=-52, xmax=50),
			    defineHistogram('EF_Obj_sigma', type='TH1F', title="CombinedObjectsTime_sigma", xbins=88, xmin=-2, xmax=20),
			    defineHistogram('EF_Obj_timings', type='TH1F', title="combinedObjectsTiming",xbins=102, xmin=-52, xmax=50),
                            defineHistogram('EF_Obj_mean, EF_Obj_sigma', type='TH2F', title="combinedObjectsMean_vs_sigma ; mean; sigma", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=0, ymax=20.),

			    # defineHistogram('EF_TT_median_noFCAL', type='TH1F', title="TriggerTowerTiming_median_noFCAL", xbins=102, xmin=-52, xmax=50),
			    # defineHistogram('EF_TT_mean_noFCAL', type='TH1F', title="TriggerTowerTiming_mean_noFCAL", xbins=102, xmin=-52, xmax=50),
			    # defineHistogram('EF_TT_sigma_noFCAL', type='TH1F', title="TriggerTowerTiming_sigma_noFCAL", xbins=88, xmin=-2, xmax=20),
			    # defineHistogram('EF_TT_median_RoI', type='TH1F', title="TriggerTowerTiming_median_RoI", xbins=102, xmin=-52, xmax=50),
			    # defineHistogram('EF_TT_mean_RoI', type='TH1F', title="TriggerTowerTiming_mean_RoI", xbins=102, xmin=-52, xmax=50),
			    # defineHistogram('EF_TT_sigma_RoI', type='TH1F', title="TriggerTowerTiming_sigma_RoI", xbins=88, xmin=-2, xmax=20),
			    # defineHistogram('EF_TT_total',type='TH1F', title="TT_total", xbins=101, xmin=-0.5, xmax=100.5),
			    # defineHistogram('EF_TTAlg_accept',type='TH1F', title="ObjAlg_accept", xbins=2, xmin=-0.5, xmax=1.5),

                            defineHistogram('EF_ObjAlg_accept_symmetric',type='TH1F', title="ObjAlg_accept_symmetric", xbins=2, xmin=-0.5, xmax=1.5),

			    # defineHistogram('EF_TT_median, EF_TT_total', type='TH2F', title="TTMedian_vs_nTowers ; median; nTowers", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=-0.5, ymax=29.5),
			    # defineHistogram('EF_TT_median, EF_TT_nEM', type='TH2F', title="TTMedian_vs_nTowers_EM ; median; nTowers_EM", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=-0.5, ymax=29.5),
			    # defineHistogram('EF_TT_median, EF_TT_nHad', type='TH2F', title="TTMedian_vs_nTowers_Had ; median; nTowers_Had", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=-0.5, ymax=29.5),

                            defineHistogram('EF_TT_mean, EF_TT_sigma', type='TH2F', title="TTMean_vs_sigma ; mean; sigma", xbins=102, xmin=-52, xmax=50, ybins=30, ymin=0, ymax=20.),

            
			   
			    defineHistogram('EF_TT_timings', type='TH1F', title="TriggerTowerTimings",xbins=102, xmin=-52, xmax=50),
			    #defineHistogram('EF_deltaR', type='TH1F', title="TT_RoI_deltaR",xbins=100, xmin=0, xmax=10),



        ]
    
