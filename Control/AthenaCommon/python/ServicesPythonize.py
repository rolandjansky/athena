# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/share/ServicesPythonize.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Pythonizations for common Gaudi/Athena services."""

import sys
try:
   from GaudiPython.Bindings import Helper, gbl
   from GaudiPython.Bindings import InterfaceCast as Interface
except ImportError:
   from gaudimodule import Helper, Interface, gbl


### data ---------------------------------------------------------------------
__version__ = '2.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'JobOptionsSvc' ]


### Job Options Service ease of use ------------------------------------------
JobOptionsSvc = Helper.service( gbl.Gaudi.svcLocator(), 'JobOptionsSvc' )
JobOptionsSvc = Interface( gbl.IJobOptionsSvc ).cast( JobOptionsSvc )
JobOptionsSvc.__class__.__doc__ = """Gaudi Job Options Service

In normal use, the JobOptionsSvc is one of the earliest services available.
Hence, you can rely on this proxy to have bound to the actual service on the
C++ side.

In addition to the normal JobOptions interface, the python proxy defines the
dump(), clean(), and verify() methods. See below for details."""

def _josvc_dump( self, what = None, out = sys.stdout ):
   """Print a dump of the current catalogue entries of <what> or simply all
entries if no argument is given."""

   if what and not ( type(what) == list or type(what) == tuple ):
      what = [ what ]

   allClients = list( self.getClients() )
   allClients.sort()

   out.write( 'Dump of current catalogue entries:\n' )
   for client in allClients:
      if what and not client in what:
         continue

      out.write( '  %s\n' % client )
      allProps = map( lambda p: (p.name(),p.value()), self.getProperties( client ) )
      allProps.sort()
      for prop in allProps:
         out.write( '     %-17s : %s\n' % (prop[0],prop[1]) )

JobOptionsSvc.__class__.dump = _josvc_dump
del _josvc_dump


def _josvc_clean( self, what = None ):
   """Remove all properties of <what> from the current catalogue, or all
entries if no argument is given."""

   for client in self.getClients():
      if what and client != what:
         continue

      props = list(self.getProperties( client ))
      for prop in props:
          self.removePropertyFromCatalogue( client, prop.name() )

JobOptionsSvc.__class__.clean = _josvc_clean
del _josvc_clean


def _josvc_verify( self, client, property, value ):
   """Return True if 'client.property == value' and False otherwise."""

   for prop in self.getProperties( client ) or []:
      if prop.name() == property:
         if prop.value() == value:
            return True
         break

   return False

JobOptionsSvc.__class__.verify = _josvc_verify
del _josvc_verify
