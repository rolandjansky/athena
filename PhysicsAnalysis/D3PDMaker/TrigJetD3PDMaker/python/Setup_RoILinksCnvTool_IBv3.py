# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Setup_RoILinksCnvTool_IBv3.py 307763 2010-06-29 14:25:58Z krasznaa $
#
# Code setting up which HLT chains to save navigation information about.
#

from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "RoILinksCnvTool" ):
    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    ToolSvc += RoILinksCnvTool( "RoILinksCnvTool" )

ToolSvc.RoILinksCnvTool.Chains_Jet = [
    "L2_j7", "EF_j10v3", 
    "L2_j15", "EF_j20v2", 
    "L2_j30", "EF_j40", 
    "L2_j60", "EF_j80v2", 
    "L2_j90", "EF_j140", 
    "L2_j130", "EF_j200",
    ]
