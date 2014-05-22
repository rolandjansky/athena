# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: HeapMon/__init__.py
# Author: Mous Tatarkhanov (MMTatarkhanov@lbl.gov) 

__version__ = '0.0.1'
__author__ = 'Mous Tatarkhanov (tmmous@berkeley.edu)'

def setup():
   import atexit, os
   __path__.append( os.path.join( __path__[0], os.environ[ 'CMTCONFIG' ] ) )

   from HeapMon import MemoryMarker
   MemoryMarker.configure( MemoryMarker.IS_RUNNING )

   atexit.register( MemoryMarker.atexit )

setup()
del setup
