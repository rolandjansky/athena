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

    MemoryTracker.ignore ('THashTable::THashTable')
    MemoryTracker.ignore ('InitCallFunc_')
    MemoryTracker.ignore ('_PyObject_GenericSetAttrWithDict')
    MemoryTracker.ignore ('_PyObject_GC_NewVar')
    MemoryTracker.ignore ('PyType_GenericAlloc')
    MemoryTracker.ignore ('PyDict_MergeFromSeq2')
    MemoryTracker.ignore ('PyEval_EvalFrameEx')
    MemoryTracker.ignore ('Cintex::CINTClassBuilder')
    MemoryTracker.ignore ('Cintex::CintTag')
    MemoryTracker.ignore ('PyROOT::')
    MemoryTracker.ignore ('ROOT::TSchemaRule::ProcessVersion')
    MemoryTracker.ignore ('CLHEP::HepMatrix::invert')
    MemoryTracker.ignore ('IncidentSvc::addListener')
    MemoryTracker.ignore ('TClass::Init')
    MemoryTracker.ignore ('_PyObject_GC_New')
    MemoryTracker.ignore ('_PyObject_GC_Malloc')
    MemoryTracker.ignore ('_PyObject_GC_Resize')
    MemoryTracker.ignore ('TString::Clobber')
    MemoryTracker.ignore ('PyString_FromStringAndSize')
    MemoryTracker.ignore ('clang::')
    MemoryTracker.ignore ('cling::')
    MemoryTracker.ignore ('llvm::')
    MemoryTracker.ignore ('TExMap::Expand')
    MemoryTracker.ignore ('TExMap::TExMap')
    MemoryTracker.ignore ('TCling::')
    MemoryTracker.ignore ('TClingDataMemberInfo::TClingDataMemberInfo')
    MemoryTracker.ignore ('TClingBaseClassInfo::TClingBaseClassInfo')
    MemoryTracker.ignore ('TClingCallFunc::SetFuncProto')
    MemoryTracker.ignore ('TClassTable::AddAlternate')
    MemoryTracker.ignore ('ROOT::TMetaUtils::GetFileName')

    atexit.register( MemoryTracker.atexit )

setup()
del setup

