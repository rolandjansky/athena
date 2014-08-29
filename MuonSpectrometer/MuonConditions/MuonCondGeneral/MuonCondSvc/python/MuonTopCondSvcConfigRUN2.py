# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Configuration Access to OFFLINE DB (COMP200)

import copy

from IOVDbSvc.CondDB import conddb

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# small helper function, which hopefully some day will go into the Configurable class
def getProperty(configurable,property):
    """Helper function to get a property of a configurable: the set value, user set default or C++ default."""
    try:
        # explictly set value
        return getattr(configurable,property)
    except AttributeError:
        # C++ default
        retVal = configurable.getDefaultProperty(property)
        if retVal is None: raise

        return retVal

def MDTCondSummarySvc(name,**kwargs):
     if athenaCommonFlags.isOnline:
         kwargs['ConditionsServices'] = []  # COOL folders not available online
     else:    
         if globalflags.DataSource != 'data':
             kwargs['ConditionsServices'] = ['MDT_DQConditionsSvc']
         if globalflags.DataSource == 'data':
             kwargs['ConditionsServices'] = ['MDT_DCSConditionsSvc']  # COOL folders not available online
     return CfgMgr.MDTCondSummarySvc(name,**kwargs)

def RPCCondSummarySvc(name,**kwargs):
#     if athenaCommonFlags.isOnline:
     if athenaCommonFlags.isOnline:
         kwargs['ConditionsServices'] = []  # COOL folders not available online
     else:    
         if globalflags.DataSource != 'data':
             kwargs['ConditionsServices'] = ['RPC_STATUSConditionsSvc']
         if globalflags.DataSource == 'data':
             kwargs['ConditionsServices'] = ['RPC_STATUSConditionsSvc']  # COOL folders not available online
     return CfgMgr.RPCCondSummarySvc(name,**kwargs)

def CSCCondSummarySvc(name,**kwargs):
     if athenaCommonFlags.isOnline:
         kwargs['ConditionsServices'] = []  # COOL folders not available online
     return CfgMgr.CSCCondSummarySvc(name,**kwargs)

def TGCCondSummarySvc(name,**kwargs):
     if athenaCommonFlags.isOnline:
         kwargs['ConditionsServices'] = []  # COOL folders not available online
     return CfgMgr.TGCCondSummarySvc(name,**kwargs) 
 

class MuonConditionsFolderHelper(object):
    __slots__ = ( 'parent', 'dbName', 'dbFolderProperties' )

    def __init__(self,parent,dbName):
        self.parent = parent
        self.dbName = dbName
        self.dbFolderProperties = []


    def setupDbFolderFromProperty(self,prop,log):
        folder = getProperty(self.parent,prop)
        log.info("Adding folder %s:%s to IOVDbSvc", self.dbName, folder)
        conddb.addFolder(self.dbName, folder)

    def setupDbFolderFromPropertyIf(self,prop):
        if prop not in self.dbFolderProperties: return
        log = logging.getLogger(self.parent.getName())
        self.setupDbFolderFromProperty(prop,log)

    def setupDbFolders(self):
        if not self.dbFolderProperties: return
        log = logging.getLogger(self.parent.getName())        
        for prop in self.dbFolderProperties:
            self.setupDbFolderFromProperty(prop,log)


## The DCS structure will change soon.....

class MDT_DCSConditionsTool(CfgMgr.MDT_DCSConditionsTool):
    __slots__ = ( '_folderHelper', ) # NB: comma is required to make it a tuple of length 1. Otherwise it is just a string.

    def __init__(self,name="MDT_DCSConditionsTool",**kwargs):
        # data vs. MC/simulation
        kwargs.setdefault("Simulation_Setup", globalflags.DataSource != 'data')
        super(MDT_DCSConditionsTool,self).__init__(name,**kwargs)
        self._folderHelper = MuonConditionsFolderHelper(self,"DCS_OFL")
        #self._folderHelper = MuonConditionsFolderHelper("DropChamberFolder","sqlite://;schema=mydcs_droppedCh.db")
        #self._folderHelper = MuonConditionsFolderHelper("LVFolder","sqlite://;schema=mydcs.db")
        ### transfer some tool folders to conddb
        # if data COMP200
        if globalflags.DataSource == 'data':
            self._folderHelper.dbFolderProperties = [ "DropChamberFolder", "LVFolder", "HVFolder", "JTAGFolder", "SetPointsV0Folder", "SetPointsV1Folder" ]
        else: # if MC or simulation
            self._folderHelper.dbFolderProperties = [ "DropChamberFolder", "LVFolder" ]

        self._folderHelper.setupDbFolders()


    # automatic forwarding to conddb if folder property is set later on
    def __setattr__(self,name,value):
        super(MDT_DCSConditionsTool,self).__setattr__(name,value)
        if hasattr(self,'_folderHelper'):
            self._folderHelper.setupDbFolderFromPropertyIf(name)

## comment because undert test these lines could cause crash in simulation if "on"



class MDT_DQConditionsTool(CfgMgr.MDT_DQConditionsTool):
    __slots__ = ( '_folderHelper', ) # NB: comma is required to make it a tuple of length 1. Otherwise it is just a string.

    def __init__(self,name="MDT_DQConditionsTool",**kwargs):
        # data vs. MC/simulation
        kwargs.setdefault("Simulation_Setup", globalflags.DataSource != 'data')
        super(MDT_DQConditionsTool,self).__init__(name,**kwargs)
        self._folderHelper = MuonConditionsFolderHelper(self,"MDT_OFL")
        #self._folderHelper = MuonConditionsFolderHelper("DropChamberFolder","sqlite://;schema=mydcs_droppedCh.db")
        #self._folderHelper = MuonConditionsFolderHelper("LVFolder","sqlite://;schema=mydcs.db")
        ### transfer some tool folders to conddb
        # if data COMP200
        if globalflags.DataSource == 'data':
            self._folderHelper.dbFolderProperties = [ "DeadChamberFolder" ]
        else: # if MC or simulation
            self._folderHelper.dbFolderProperties = [ "DropChamberFolder" ]

        self._folderHelper.setupDbFolders()


    # automatic forwarding to conddb if folder property is set later on
    def __setattr__(self,name,value):
        super(MDT_DQConditionsTool,self).__setattr__(name,value)
        if hasattr(self,'_folderHelper'):
            self._folderHelper.setupDbFolderFromPropertyIf(name)




class RPC_DCSConditionsTool(CfgMgr.RPC_DCSConditionsTool):
    __slots__ = ( '_folderHelper', ) # NB: comma is required to make it a tuple of length 1. Otherwise it is just a string.
    
    def __init__(self,name="RPC_DCSConditionsTool",**kwargs):
        # data vs. MC/simulation
        #kwargs.setdefault("Simulation_Setup", globalflags.DataSource != 'data')
        super(RPC_DCSConditionsTool,self).__init__(name,**kwargs)
        self._folderHelper = MuonConditionsFolderHelper(self,"DCS_OFL")
        
        ### transfer some tool folders to conddb
        # if data COMP200
        if globalflags.DataSource == 'data':
            self._folderHelper.dbFolderProperties = [  "OffPanelFolder", "DeadPanel" ]
        else: # if MC or simulation
            self._folderHelper.dbFolderProperties = []
            
        self._folderHelper.setupDbFolders()


    # automatic forwarding to conddb if folder property is set later on
    def __setattr__(self,name,value):
        super(RPC_DCSConditionsTool,self).__setattr__(name,value)
        if hasattr(self,'_folderHelper'):
            self._folderHelper.setupDbFolderFromPropertyIf(name)




class RpcDetectorStatusDbTool(CfgMgr.RpcDetectorStatusDbTool):
    __slots__ = ( '_folderHelper', ) # NB: comma is required to make it a tuple of length 1. Otherwise it is just a string.

    def __init__(self,name="RpcDetectorStatusDbTool",**kwargs):
        # data vs. MC/simulation
        # kwargs.setdefault("Simulation_Setup", globalflags.DataSource != 'data')
        # make the tool instance
        super(RpcDetectorStatusDbTool,self).__init__(name,**kwargs)
        self._folderHelper = MuonConditionsFolderHelper(self,"RPC_OFL")

        ### transfer some tool folders to conddb
        # if data COMP200
        if globalflags.DataSource == 'data':
            self._folderHelper.dbFolderProperties = [ "RpcDqFolder" ]
        else: # if MC or simulation
            self._folderHelper.dbFolderProperties = [ "RpcDqFolder" ]

        self._folderHelper.setupDbFolders()


    # automatic forwarding to conddb if folder property is set later on
    def __setattr__(self,name,value):
        super(RpcDetectorStatusDbTool,self).__setattr__(name,value)
        if hasattr(self,'_folderHelper'):
            self._folderHelper.setupDbFolderFromPropertyIf(name)



class CSC_DCSConditionsTool(CfgMgr.CSC_DCSConditionsTool):
    __slots__ = ( '_folderHelper', ) # NB: comma is required to make it a tuple of length 1. Otherwise it is just a string.

    def __init__(self,name="CSC_DCSConditionsTool",**kwargs):
        # data vs. MC/simulation
        #kwargs.setdefault("Simulation_Setup", globalflags.DataSource != 'data')
        # make the tool instance
        super(CSC_DCSConditionsTool,self).__init__(name,**kwargs)
        self._folderHelper = MuonConditionsFolderHelper(self,"DCS_OFL")

        ### transfer some tool folders to conddb
        # if data COMP200
        if globalflags.DataSource == 'data':
            self._folderHelper.dbFolderProperties = [ "HVFolder", "ChamberFolder" ]
        else: # if MC or simulation
            self._folderHelper.dbFolderProperties = []

        self._folderHelper.setupDbFolders()


    # automatic forwarding to conddb if folder property is set later on
    def __setattr__(self,name,value):
        super(CSC_DCSConditionsTool,self).__setattr__(name,value)
        if hasattr(self,'_folderHelper'):
            self._folderHelper.setupDbFolderFromPropertyIf(name)
