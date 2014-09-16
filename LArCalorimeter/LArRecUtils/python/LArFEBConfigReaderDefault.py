# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr

def LArFEBConfigReaderDefault (name="LArFEBConfigReaderDefault", **kw): 
    mlog = logging.getLogger( 'LArFEBConfigReaderDefault::__init__ ' )

    if hasattr(ServiceMgr.ToolSvc,name):
        mlog.info("LArFEBConfigReader with name" + name + "already known to ToolSvc")
        return getattr(ServiceMgr.ToolSvc,name)


    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArFEBConfigReader (**kw)

    LArFebConfigFolders=[
    "/LAR/Configuration/FEBConfig/Physics/EMBA1",  
    "/LAR/Configuration/FEBConfig/Physics/EMBA2",
    "/LAR/Configuration/FEBConfig/Physics/EMBA3",
    "/LAR/Configuration/FEBConfig/Physics/EMBA4",
    "/LAR/Configuration/FEBConfig/Physics/EMBC1",
    "/LAR/Configuration/FEBConfig/Physics/EMBC2",
    "/LAR/Configuration/FEBConfig/Physics/EMBC3",
    "/LAR/Configuration/FEBConfig/Physics/EMBC4", 
    "/LAR/Configuration/FEBConfig/Physics/EMECA1",
    "/LAR/Configuration/FEBConfig/Physics/EMECA2",
    "/LAR/Configuration/FEBConfig/Physics/EMECA3",
    "/LAR/Configuration/FEBConfig/Physics/EMECC1",
    "/LAR/Configuration/FEBConfig/Physics/EMECC2",
    "/LAR/Configuration/FEBConfig/Physics/EMECC3",
    "/LAR/Configuration/FEBConfig/Physics/FCALA", 
    "/LAR/Configuration/FEBConfig/Physics/FCALC", 
    "/LAR/Configuration/FEBConfig/Physics/HECA", 
    "/LAR/Configuration/FEBConfig/Physics/HECC", 
    ]


    for f in LArFebConfigFolders:
        conddb.addFolder("LAR_ONL",f)

    tool.ListOfFolders=LArFebConfigFolders
    return tool
    
