# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Setup_RoILinksCnvTool_AllMuonChains.py 463173 2011-10-14 09:32:25Z kanno $
# Use regular expression of muon chains
# 

from AthenaCommon.AppMgr import ToolSvc
if not hasattr (ToolSvc, 'RoILinksCnvTool'):
    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

#ToolSvc.RoILinksCnvTool.Chains_Muon   = ['L2_mu.*','L2_2mu.*','L2_3mu.*','EF_mu.*','EF_2mu.*','EF_3mu.*']
ToolSvc.RoILinksCnvTool.Chains_Muon   = ['L2_.*mu[0-9].*','EF_.*mu[0-9].*']
ToolSvc.RoILinksCnvTool.Chains_TileMu = ['L2_mu.*tile.*','L2_mu.*trod.*','EF_mu.*tile.*','EF_mu.*trod.*']
