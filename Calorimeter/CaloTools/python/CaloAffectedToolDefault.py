# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

def CaloAffectedToolDefault(name='CaloAffectedToolDefault'):

    # check if tool already exists
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured (public) tool
        return getattr(svcMgr.ToolSvc, name)

    mlog = logging.getLogger( 'CaloAffectedToolDefault' )

    from CaloTools.CaloToolsConf import CaloAffectedTool


    theTool = CaloAffectedTool(name)

    from RecExConfig.RecFlags import rec
    if rec.readRDO():
       theTool.readRaw = True
    else:
       theTool.readRaw = False
       from IOVDbSvc.CondDB import conddb
       conddb.addFolder ('', '/LAR/LArAffectedRegionInfo<metaOnly/>')

    return theTool
