# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: JobOptCfg.py
# @purpose: a customized Configurable class for the ValgrindSvc
# @author: Frank Winklmeier

# import the automatically generated Configurable
from Valkyrie.ValkyrieConf import ValgrindSvc as _ValgrindSvc

class ValgrindSvc( _ValgrindSvc):

    __slots__ = ( )      # enforce no new properties

    def __init__(self, name = "ValgrindSvc", **kwargs ):
       # have to call base init
       kwargs['name'] = name
       super( ValgrindSvc, self ).__init__( **kwargs )
#       return

#    def setDefaults( cls, handle ):

       # Enable the auditor hooks
       from AthenaCommon.AppMgr import theApp
       theApp.AuditAlgorithms = True
       theApp.AuditTools      = True
       theApp.AuditServices   = True

       # make sure the application manager explicitly creates the service
       #if hasattr(handle, "getFullJobOptName") :
       handleName = self.getFullJobOptName()
       if handleName not in theApp.CreateSvc:
          theApp.CreateSvc += [ handleName ]

       return

    pass  # class ValgrindSvc


