# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaCommon/python/CfgMgr.py
# @purpose: facade to Configurables
# @author: Sebastien Binet <binet@cern.ch>

__author__  = "Sebastien Binet <binet@cern.ch>"
__version__ = "$Revision: 1.3 $"
__doc__     = """
A module facade to the repository of automatically generated configurables.
Once imported it allows queries like so:
 >>> from AthenaCommon import CfgMgr
 >>> topSequence += CfgMgr.MyAlgorithm( 'MyAlg', OutputLevel = INFO )
 >>> topSequence += CfgMgr.MyCppNs__MyAlgorithm( 'UrAlg' )
"""

import sys
import types

class ModuleFacade(types.ModuleType):
   """a helper class to allow easy retrieval of automatically generated
   configurables (stolen from PyRoot)
   """
   def __init__( self, module ):
      types.ModuleType.__init__( self, __name__ )
        
      self.__dict__[ '__doc__'  ] = module.__doc__
      self.__dict__[ '__name__' ] = module.__name__

      from ConfigurableDb import getConfigurable
      self.__dict__[ 'getConfigurable' ] = getConfigurable

   def __getattr__(self, k):
      if not k.startswith( '__' ):
         return self.getConfigurable( k, assumeCxxClass = False )

      return super( types.ModuleType, self ).__getattr__( self, k )

sys.modules[ __name__ ] = ModuleFacade( sys.modules[ __name__ ] )
del ModuleFacade
