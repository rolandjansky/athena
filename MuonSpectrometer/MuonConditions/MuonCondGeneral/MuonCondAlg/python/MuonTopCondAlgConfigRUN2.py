# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## Configuration Access to OFFLINE DB (COMP200)

import copy

from IOVDbSvc.CondDB import conddb

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


def addFolder(parent, db, folder):
    log = logging.getLogger(parent.getName())        
    log.info("Adding folder %s:%s to IOVDbSvc", db, folder)
    conddb.addFolder(db, folder, className='CondAttrListCollection');


class MdtCondDbAlg(CfgMgr.MdtCondDbAlg):
    def __init__(self,name="MdtCondDbAlg",**kwargs):
        if athenaCommonFlags.isOnline:
            kwargs['isOnline'] = True  # COOL folders not available online
        else:    
            kwargs['isOnline'] = False # COOL folders not available online
            if globalflags.DataSource != 'data':
                kwargs['isData'] = False
            if globalflags.DataSource == 'data':
                kwargs['isData'] = True
                kwargs['isRun1'] = False
        super(MdtCondDbAlg,self).__init__(name,**kwargs)
        if globalflags.DataSource == 'data':
            addFolder(self, "DCS_OFL", "/MDT/DCS/HV")
            addFolder(self, "DCS_OFL", "/MDT/DCS/LV")
        else: # if MC or simulation
            addFolder(self, "DCS_OFL", "/MDT/DCS/DROPPEDCH"  )
            addFolder(self, "DCS_OFL", "/MDT/DCS/PSLVCHSTATE")


class RpcCondDbAlg(CfgMgr.RpcCondDbAlg):
    def __init__(self,name="RpcCondDbAlg",**kwargs):
        if athenaCommonFlags.isOnline:
            kwargs['isOnline'] = True  # COOL folders not available online
        else:    
            kwargs['isOnline'] = False # COOL folders not available online
            if globalflags.DataSource != 'data':
                kwargs['isData'] = False
            if globalflags.DataSource == 'data':
                kwargs['isData'] = True
                kwargs['isRun1'] = False
        super(RpcCondDbAlg,self).__init__(name,**kwargs)
        if globalflags.DataSource == 'data':
            addFolder(self, "DCS_OFL", "/RPC/DCS/DeadRopanels"   )
            addFolder(self, "DCS_OFL", "/RPC/DCS/OffRopanels"    )
            addFolder(self, "RPC_OFL", "/RPC/DQMF/ELEMENT_STATUS")
        else: # if MC or simulation
            addFolder(self, "RPC_OFL", "/RPC/DQMF/ELEMENT_STATUS")

class CscCondDbAlg(CfgMgr.CscCondDbAlg):
    def __init__(self,name="CscCondDbAlg",**kwargs):
        if athenaCommonFlags.isOnline:
            kwargs['isOnline'] = True  # COOL folders not available online
        else:    
            kwargs['isOnline'] = False # COOL folders not available online
            if globalflags.DataSource != 'data':
                kwargs['isData'] = False
            if globalflags.DataSource == 'data':
                kwargs['isData'] = True
                kwargs['isRun1'] = False
        super(CscCondDbAlg,self).__init__(name,**kwargs)
        addFolder(self, "CSC_OFL", "/CSC/STAT")

class TgcCondDbAlg(CfgMgr.TgcCondDbAlg):
    def __init__(self,name="TgcCondDbAlg",**kwargs):
        if athenaCommonFlags.isOnline:
            kwargs['isOnline'] = True  # COOL folders not available online
        else:    
            kwargs['isOnline'] = False # COOL folders not available online
            if globalflags.DataSource != 'data':
                kwargs['isData'] = False
            if globalflags.DataSource == 'data':
                kwargs['isData'] = True
                kwargs['isRun1'] = False
        super(TgcCondDbAlg,self).__init__(name,**kwargs)
        addFolder(self, "", "") # which folder?

