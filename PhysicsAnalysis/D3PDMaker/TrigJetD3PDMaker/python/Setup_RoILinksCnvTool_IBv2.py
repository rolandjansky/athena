# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
import logging
log = logging.getLogger('Setup_RoILinksCnvTool_IBv2.py')

from AthenaCommon.AppMgr             import ToolSvc
ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

ToolSvc.RoILinksCnvTool.Chains_Electron = [
    'L2_e3_NoCut', 'EF_e3_NoCut',
    'L2_e3_NoCut_IdScan', 'EF_e3_NoCut_IdScan',
    'L2_e3_NoCut_TRT', 'EF_e3_NoCut_TRT',
    'L2_e5_NoCut_cells', 'EF_e5_NoCut_cells',
    'L2_e5_NoCut','EF_e5_NoCut',
    'L2_e5_NoCut_SiTrk', 'EF_e5_NoCut_SiTrk',
    'L2_e5_NoCut_IdScan', 'EF_e5_NoCut_IdScan',
    'L2_e5_NoCut_TRT', 'EF_e5_NoCut_TRT',
    'L2_e5_NoCut_FwdBackTrk', 'EF_e5_NoCut_FwdBackTrk',
    'L2_e5_loose', 'EF_e5_loose',
    'L2_e5_loose_IdScan', 'EF_e5_loose_IdScan',
    'L2_e5_loose_TRT', 'EF_e5_loose_TRT',
    'L2_e10_loose', 'EF_e10_loose',
    'L2_e10_loose_passL2', 'EF_e10_loose_passL2',
    'L2_e10_loose_passEF', 'EF_e10_loose_passEF',
    'L2_e10_medium', 'EF_e10_medium',
    'L2_g3_nocut', 'EF_g3_nocut',
    'L2_g5_nocut', 'EF_g5_nocut',
    'L2_g5_loose', 'EF_g5_loose', 
    'L2_g10_loose', 'EF_g10_loose',
    #ID monitoring
    'L2_e5_loose_NoIDTrkCut', 'EF_e5_loose_NoIDTrkCut',
    'L2_e20_loose_NoIDTrkCut', 'EF_e20_loose_NoIDTrkCut',
]
