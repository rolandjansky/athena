# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from GaudiKernel.GaudiHandles import  ServiceHandle, PrivateToolHandleArray,PrivateToolHandle, PublicToolHandleArray, PublicToolHandle
from AthenaCommon.Configurable import Configurable,ConfigurableService, ConfigurableAlgTool


class dummyService(ConfigurableService):
  __slots__ = { 
    'OutputLevel' : 0, # int
    'AuditServices' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'SomeTools' : PrivateToolHandleArray([]), # GaudiHandleArray
    'OneTool' : PrivateToolHandle(''), # GaudiHandle
    'SomePublicTools' : PublicToolHandleArray([]), # GaudiHandleArray
    'OnePublicTool' :  PublicToolHandle(''), 
    'AnotherService' : ServiceHandle(''),
    'AString' : '', #str  
    'AList' : [ ]
  }
  _propertyDocDct = { 
    'OutputLevel' : """ output level [Service] """,
    'AuditServices' : """ [[deprecated]] unused [Service] """,
    'AuditInitialize' : """ trigger auditor on initialize() [Service] """,
    'AuditStart' : """ trigger auditor on start() [Service] """,
    'AuditStop' : """ trigger auditor on stop() [Service] """,
    'AuditFinalize' : """ trigger auditor on finalize() [Service] """,
    'AuditReinitialize' : """ trigger auditor on reinitialize() [Service] """,
    'AuditRestart' : """ trigger auditor on restart() [Service] """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(dummyService, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'AthenaServices'
  def getType( self ):
      return 'dummyService'
  pass # class MetaDataSvc



class dummyTool( ConfigurableAlgTool ) :
  __slots__ = { 
    'ExtraInputs' : [], # list
    'ExtraOutputs' : [], # list
    'OutputLevel' : 0, # int
    'MonitorService' : 'MonitorSvc', # str
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'EvtStore' : ServiceHandle('StoreGateSvc'), # GaudiHandle
    'DetStore' : ServiceHandle('StoreGateSvc/DetectorStore'), # GaudiHandle
    'BString' : '', #str  
    'BList' : [ ]
  }
  _propertyDocDct = { 
    'ExtraInputs' : """  [DataHandleHolderBase<PropertyHolder<CommonMessaging<implements<IAlgTool,IDataHandleHolder,IProperty,IStateful> > > >] """,
    'ExtraOutputs' : """  [DataHandleHolderBase<PropertyHolder<CommonMessaging<implements<IAlgTool,IDataHandleHolder,IProperty,IStateful> > > >] """,
    'OutputLevel' : """ output level [AlgTool] """,
    'MonitorService' : """ name to use for Monitor Service [AlgTool] """,
    'AuditTools' : """ [[deprecated]] unused [AlgTool] """,
    'AuditInitialize' : """ trigger auditor on initialize() [AlgTool] """,
    'AuditStart' : """ trigger auditor on start() [AlgTool] """,
    'AuditStop' : """ trigger auditor on stop() [AlgTool] """,
    'AuditFinalize' : """ trigger auditor on finalize() [AlgTool] """,
    'AuditReinitialize' : """ trigger auditor on reinitialize() [AlgTool] """,
    'AuditRestart' : """ trigger auditor on restart() [AlgTool] """,
    'EvtStore' : """ Handle to a StoreGateSvc instance: it will be used to retrieve data during the course of the job [unknown owner type] """,
    'DetStore' : """ Handle to a StoreGateSvc/DetectorStore instance: it will be used to retrieve data during the course of the job [unknown owner type] """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(dummyTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'IOVSvc'
  def getType( self ):
      return 'dummyTool'
  pass # class IOVSvcTool
