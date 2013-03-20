#import logging
#log = logging.getLogger('Setup_RoILinksCnvTool_tau.py')

from AthenaCommon.AppMgr             import ToolSvc
if not hasattr (ToolSvc, 'RoILinksCnvTool'):
    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

ToolSvc.RoILinksCnvTool.Chains_Tau = [
    'L2_trk29_Central_Tau_IDCalib', 'L2_tau12_loose', 'L2_trk16_loose', 'L2_tau16i_medium', 'L2_tau125_loose', 'L2_tauNoCut_SiTrk', 'L2_tau50_medium', 'L2_trk16_Fwd_Tau_IDCalib', 'L2_2tau29_loose', 'L2_tau29i_loose', 'L2_tau16i_loose', 'L2_trk9_Fwd_Tau_IDCalib', 'L2_tau84_loose', 'L2_tau50_loose_SiTrk', 'L2_trk9_Fwd_Tau_SiTrack_IDCalib', 'L2_tau38_loose', 'L2_2tau29i_loose', 'L2_tau20i_loose', 'L2_2tau29i_medium', 'L2_tau12_loose_PT', 'L2_tauNoCut', 'L2_tauNoCut_hasTrk_MV', 'L2_trk9_loose', 'L2_tau29_loose', 'L2_tau16_loose_SiTrk', 'L2_tauNoCut_TRT', 'L2_tau16_loose_PT', 'L2_trk16_Central_Tau_IDCalib', 'L2_2tau20_loose_PT L2_trk9_Central_Tau_IDCalib', 'L2_tau20_loose', 'L2_tau50_loose', 'L2_2tau20_loose', 'L2_tauNoCut_NoIDTrkCut', 'L2_trk9_Central_Tau_SiTrack_IDCalib', 'L2_trk29_Fwd_Tau_IDCalib', 'L2_tau50_tight', 'L2_tau16_loose', 'L2_tau16i_NoIDTrkCut', 'L2_tau50_loose_PT',
    'EF_tau12_loose', 'EF_trk16_loose', 'EF_tau16i_medium', 'EF_tau125_loose', 'EF_tauNoCut_SiTrk', 'EF_tau50_medium', 'EF_2tau29_loose', 'EF_tau29i_loose', 'EF_tau16i_loose', 'EF_tau84_loose', 'EF_tau50_loose_SiTrk', 'EF_tau38_loose', 'EF_2tau29i_loose', 'EF_tau20i_loose', 'EF_2tau29i_medium', 'EF_tau12_loose_PT', 'EF_tauNoCut', 'EF_tauNoCut_hasTrk_MV', 'EF_trk9_loose', 'EF_tau29_loose', 'EF_tau16_loose_SiTrk', 'EF_tauNoCut_TRT', 'EF_tau16_loose_PT', 'EF_2tau20_loose_PT', 'EF_tau20_loose', 'EF_tau50_loose', 'EF_2tau20_loose', 'EF_tauNoCut_NoIDTrkCut', 'EF_tau50_tight', 'EF_tau16_loose', 'EF_tau16i_NoIDTrkCut', 'EF_tau50_loose_PT', 
    ]
