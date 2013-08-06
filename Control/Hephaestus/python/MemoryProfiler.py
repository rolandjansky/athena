# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: Hephaestus/MemoryProfiler.py
# Author Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '1.0.0'
__author__ = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

from MemoryTracker import configure, start, stop
from MemoryTracker import PROFILE, LEAK_CHECK, FILTER_STL

def output( name ):
   import MemoryTracker
   MemoryTracker.configure( MemoryTracker.PROFILE | MemoryTracker.configure() )
   MemoryTracker._symbname( name )

   import Auditor
   Auditor.GlobalSettings[ 'label' ] = name
