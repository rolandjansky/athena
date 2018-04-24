# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Fri May  8 15:13:01 2015"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *

class SCT_SLHC_DetectorTool( ConfigurableAlgTool ) :
  __slots__ = { 
    'MonitorService' : 'MonitorSvc', # str
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
    'EvtStore' : ServiceHandle('StoreGateSvc'), # GaudiHandle
    'DetStore' : ServiceHandle('StoreGateSvc/DetectorStore'), # GaudiHandle
    'UserStore' : ServiceHandle('UserDataSvc/UserDataSvc'), # GaudiHandle
    'DetectorName' : 'SCT', # str
    'Alignable' : False, # bool
    'GmxFilename' : '', # str
    'ServiceBuilderTool' : PublicToolHandle(''), # GaudiHandle
    'RDBAccessSvc' : ServiceHandle('RDBAccessSvc'), # GaudiHandle
    'GeometryDBSvc' : ServiceHandle('InDetGeometryDBSvc'), # GaudiHandle
    'GeoModelSvc' : ServiceHandle('GeoModelSvc'), # GaudiHandle
    'LorentzAngleSvc' : ServiceHandle('SCTLorentzAngleSvc'), # GaudiHandle
  }
  _propertyDocDct = { 
    'DetStore' : """ Handle to a StoreGateSvc/DetectorStore instance: it will be used to retrieve data during the course of the job """,
    'UserStore' : """ Handle to a UserDataSvc/UserDataSvc instance: it will be used to retrieve user data during the course of the job """,
    'EvtStore' : """ Handle to a StoreGateSvc instance: it will be used to retrieve data during the course of the job """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(SCT_SLHC_DetectorTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'SCT_GeoModelXml'
  def getType( self ):
      return 'SCT_SLHC_DetectorTool'
  pass # class SCT_SLHC_DetectorTool
