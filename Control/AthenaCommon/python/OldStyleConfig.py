# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/OldStyleConfig.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

import string
import Logging


### data ---------------------------------------------------------------------
__version__ = '5.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'Algorithm', 'Service', 'AlgTool' ]

## for messaging
log = Logging.logging.getLogger( 'OldStyleConfig' )

### old style configuration is now disallowed; forward if possible, or FATAL
def _compatLookup( name, klass ):
   parts = string.split( name, '/' )
   if (len(parts) > 2) :
      msg = 'invalid argument name = "%s", must be of the form "type[/name]"' % name
      log.error( msg )
      raise RuntimeError( msg )

   import ConfigurableDb
   cls = ConfigurableDb.getConfigurable( parts[0] )
   if not cls or not issubclass( cls, klass ):
      msg = 'unable to locate configurable for type "%s"' % parts[0]
      log.error( msg )
      raise TypeError( msg )

   if len(parts) == 1:
      return cls()
   else:
      return cls(parts[1])

_algConfigurables = {}
def Algorithm( name ):
   import Configurable
   result = _compatLookup( name, Configurable.ConfigurableAlgorithm )
   if result:
      global _algConfigurables
      _algConfigurables[ name ] = result
   return result

Algorithm.configurables = _algConfigurables


_svcConfigurables = {}
def Service( name ):
   import Configurable
   result = _compatLookup( name, Configurable.ConfigurableService )
   if result:
      global _svcConfigurables
      _svcConfigurables[ name ] = result
   return result

Service.configurables = _svcConfigurables


_atlConfigurables = {}
def AlgTool( name ):
   import Configurable
   result = _compatLookup( name, Configurable.ConfigurableAlgTool )
   if result:
      global _atlConfigurables
      _atlConfigurables[ name ] = result
   return result

AlgTool.configurabls = _atlConfigurables
