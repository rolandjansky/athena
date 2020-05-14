# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/LBMetadataConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Return a configured LBMetadataTool.
#


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags


def LBMetadataConfig ():
    # Configure LumiBlockMetaDataTool, if it hasn't been done already.
    from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
    mdtools = [t.getType() for t in ServiceMgr.MetaDataSvc.MetaDataTools]
    if not 'LumiBlockMetaDataTool' in mdtools:
        # add LumiBlockMetaDataTool to ToolSvc and configure
        from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
        ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )

        # add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
        ServiceMgr.MetaDataSvc.MetaDataTools += [ToolSvc.LumiBlockMetaDataTool]
        
    return EventCommonD3PDMaker.LBMetadataTool (Metakey = 'Lumi/')
