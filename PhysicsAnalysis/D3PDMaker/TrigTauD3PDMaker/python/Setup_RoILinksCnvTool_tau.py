# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#import logging
#log = logging.getLogger('Setup_RoILinksCnvTool_tau.py')

from AthenaCommon.AppMgr             import ToolSvc
if not hasattr (ToolSvc, 'RoILinksCnvTool'):
    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')
    
    
ToolSvc.RoILinksCnvTool.Chains_Tau = [
    'L2_tau.*',
    'L2_2tau.*',
    'EF_tau.*',
    'EF_2tau.*'    
    ]

