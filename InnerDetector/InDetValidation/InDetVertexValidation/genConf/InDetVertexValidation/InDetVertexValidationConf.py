# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Mon Aug 14 17:17:53 2017"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *

class InDetVertexValidationTool( ConfigurableAlgTool ) :
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
    'ProcessNEvents' : 0, # int
    'histoPathBase' : '', # str
    'PreScale' : 0, # int
    'TriggerChain' : '', # str
    'TriggerGroup' : '', # str
    'ManagerName' : 'AthenaMonManager', # str
    'TrigDecisionTool' : PublicToolHandle(''), # GaudiHandle
    'TriggerTranslatorTool' : PublicToolHandle(''), # GaudiHandle
    'FilterTools' : PublicToolHandleArray([]), # GaudiHandleArray
    'EnableLumi' : False, # bool
    'DetailLevel' : 1, # int
    'FileKey' : '', # str
    'DataType' : 'userDefined', # str
    'Environment' : 'noOutput', # str
    'EventInfoContainerName' : 'EventInfo', # str
    'VertexContainerName' : 'PrimaryVertices', # str
    'TruthVertexContainerName' : 'TruthVertices', # str
    'TrackContainerName' : 'InDetTrackParticles', # str
    'truthEventContainerName' : 'TruthEvents', # str
    'truthPileupEventContainerName' : 'TruthPileupEvents', # str
    'DirName' : 'IDPerformanceMon', # str
    'MaxLayoutEta' : 3.00000, # float
    'MuCorrected' : -1.0000000, # float
    'MuWeight' : 1.0000000, # float
    'TightTrackVertexAssoTool' : PublicToolHandle('CP::ITrackVertexAssociationTool'), # GaudiHandle
    'LooseTrackVertexAssoTool' : PublicToolHandle('CP::ITrackVertexAssociationTool'), # GaudiHandle
  }
  _propertyDocDct = { 
    'DetStore' : """ Handle to a StoreGateSvc/DetectorStore instance: it will be used to retrieve data during the course of the job """,
    'UserStore' : """ Handle to a UserDataSvc/UserDataSvc instance: it will be used to retrieve user data during the course of the job """,
    'EvtStore' : """ Handle to a StoreGateSvc instance: it will be used to retrieve data during the course of the job """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(InDetVertexValidationTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'InDetVertexValidation'
  def getType( self ):
      return 'InDetVertexValidationTool'
  pass # class InDetVertexValidationTool
