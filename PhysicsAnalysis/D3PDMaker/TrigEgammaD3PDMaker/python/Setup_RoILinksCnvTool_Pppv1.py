# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
import logging
log = logging.getLogger('Setup_RoILinksCnvTool_Pppv1.py')

from AthenaCommon.AppMgr             import ToolSvc
if not hasattr(ToolSvc, 'RoILinksCnvTool'):
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

ToolSvc.RoILinksCnvTool.Chains_Electron = [
    'L2_e.*', 'EF_e.*', 'L2_2e.*', 'EF_2e.*', 'L2_3e.*', 'EF_3e.*', 
    'L2_g.*', 'EF_g.*', 'L2_2g.*', 'EF_2g.*', 'L2_3g.*', 'EF_3g.*', 
    ]
