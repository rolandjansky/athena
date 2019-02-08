# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/DumpProperties.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Dump all properties from Gaudi objects in a given namespace."""

import os
import sys
import GaudiPython


### data ---------------------------------------------------------------------
__version__ = '1.2.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'pprint', 'dump', 'dumpSet', 'dumpAlgorithms', 'dumpServices' ]

indent = '   '


### logging and messages -----------------------------------------------------
import logging
log = logging.getLogger( 'DumpProperties' )


### helper -------------------------------------------------------------------
def _printFromLookup( ns, listing, lookup, extra, klass ):
   """<internal>"""
 
   if 'theApp' not in ns:
      log.error( 'given namespace does not contain "theApp"' )
      return

   if lookup not in ns:
      log.error( 'given namespace does not contain "%s" lookup', lookup )
      return

   app = ns[ 'theApp' ]
   if app.state() != app.State.CONFIGURED:
      listing = []
      for name, obj in ns.items():
         if isinstance( obj, klass ):
            listing.append( name )
   else:
      listing = getattr( app, listing )()

   if extra and type(extra) != list:
      extra = getattr( app, extra )

   extra = [ e for e in extra if e not in listing ]

   lookup = ns[ lookup ]
   for name in listing:
      pprint( lookup( name ) )

   for name in extra:
      pprint( klass( name ) )


### function pprint ---------------------------------------------------------
def pprint( obj, stream = sys.stdout ):
   """Pretty print the properties of the given Gaudi object."""

   from Configurable import Configurable
   if isinstance( obj, Configurable ):
      stream.write( str(obj) )
      stream.write( '\n' )
      return

   try:
      stream.write( obj.name() + os.linesep )
   except TypeError:
      print obj
   for prop, value in obj.properties().items():
      if not obj._ip:
         try:
            value = eval( value )
         except Exception:
            pass

      if value and type(value) == list:
         stream.write( indent + '%s = [\n' % prop )
         nv = len(value)
         for i in range(nv):
            v = value[i]
            if type(v) == str:
               v = '"%s"' % v
            stream.write( 2*indent + str(v) + ( i == nv-1 and "\n" or ",\n" ) )
         stream.write( indent + ']\n' )
         continue

      if hasattr( value, 'value' ) and callable( value.value ):
         value = value.value()

      if type(value) == str:
         value = '"%s"' % value

      stream.write( indent + '%-20s = %s\n' % (prop,value) )
   stream.write( os.linesep )


### function dump -----------------------------------------------------------
def dump( opt = [ 'set' ], ns = None ):
   """Dump properties of Gaudi objects; opt can be 'set', 'algorithms', 'services',
'all', or a list of a combination of these. The given namespace must contain the
application manager."""

   if ns is None:
      import __main__
      ns = __main__.__dict__ 

   if hasattr( opt, 'properties' ) and hasattr( opt, 'name' ):
      pprint( opt )
      return

   if type(opt) == str:
      opt = [ opt ]

   if 'set' in opt:
      log.info( '  ***** dumping properties of configured objects *****  ' )
      dumpSet( ns )

   all = 'all' in opt
   if all or 'algorithms' in opt:
      log.info( '  ***** dumping algorithm properties *****  ' )
      dumpAlgorithms( ns )

   if all or 'services' in opt:
      log.info( '  ***** dumping service properties   *****  ' )
      dumpServices( ns )
 

### selected dump functions -------------------------------------------------
def dumpSet( ns = None ):
   """Dump all Gaudi objects that have had their properties set. The namespace must
contain the application manager."""

   if ns is None:
      import __main__
      ns = __main__.__dict__

   for name, obj in ns.items():
      if isinstance( obj, GaudiPython.Bindings.iProperty ) or name == "StoreGate":
         pprint( obj )


def dumpAlgorithms( ns = None ):
   """Dump all algorithm properties. The namespace must contain the application mgr."""

   if ns is None:
      import __main__
      ns = __main__.__dict__

   _printFromLookup( ns, 'algorithms', 'Algorithm', 'TopAlg', GaudiPython.Bindings.iAlgorithm )


def dumpServices( ns = None ):
   """Dump all service properties. The namespace must contain the application mgr."""

   if ns is None:
      import __main__
      ns = __main__.__dict__

   _printFromLookup( ns, 'services', 'Service', [ 'StoreGateSvc' ], GaudiPython.Bindings.iService )
