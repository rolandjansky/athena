# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: Hephaestus/__init__.py
# Author Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '2.0.0'
__author__ = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

def setup():
    import atexit, os
    __path__.append( os.path.join( __path__[0], os.environ[ 'CMTCONFIG' ] ) )

  # make sure that 3rd party libraries can see Hephaestus symbols by letting
  # python load the memory tracker in "broadcast" mode
    import DLFCN, sys
    dlflags = sys.getdlopenflags() 
    sys.setdlopenflags( DLFCN.RTLD_GLOBAL | DLFCN.RTLD_NOW )
    import MemoryTracker
    sys.setdlopenflags( dlflags )

  # switch off profiling; do filter STL internal allocations
    MemoryTracker.configure( MemoryTracker.LEAK_CHECK |
                             MemoryTracker.QUICK |
                             MemoryTracker.FILTER_STL )

  # ignore muon common blocks in report
    MemoryTracker.ignore( '__m_mb_' )

  # ignore CINT dictionary builtup in report
    MemoryTracker.ignore( 'G__' )
    MemoryTracker.ignore( 'TCint::' )

  # ignore Reflex dictionary builtup in report
    MemoryTracker.ignore( 'Reflex::' )

  # ignore streamers and collections from ROOT I/O in report
    MemoryTracker.ignore( 'TStreamerInfo' )
    MemoryTracker.ignore( 'TGenCollectionProxy' )
    MemoryTracker.ignore( 'TCollectionProxy' )
    MemoryTracker.ignore( 'TStorage' )

  # like STL, the following only leaks if the full object is leaked, so it's a dupe
    MemoryTracker.ignore( 'TStringRef::GetRep' )
    MemoryTracker.ignore( 'TString::Init' )
    MemoryTracker.ignore( 'TString::Replace' )
    MemoryTracker.ignore( 'TList::NewLink' )

  # this was a known issue, no longer relevant, but leave ignore in
    MemoryTracker.ignore( 'StoreGateSvc::setupProxy' )

  # ignore unknowns, as they've never proven useful; they typically arise from file
  # static functions (that for that reason have no linker symbol associated with
  # them), which are usually in system libraries rather than in ATLAS code ...
    MemoryTracker.ignore( '<unknown>' )

    atexit.register( MemoryTracker.atexit )

setup()
del setup
