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
    import sys
    dlflags = sys.getdlopenflags() 
    sys.setdlopenflags( os.RTLD_GLOBAL | os.RTLD_NOW )
    import MemoryTracker
    sys.setdlopenflags( dlflags )

  # switch off profiling; do filter STL internal allocations
    MemoryTracker.configure( MemoryTracker.LEAK_CHECK |
                             MemoryTracker.QUICK |
                             MemoryTracker.FILTER_STL )

  # ignore muon common blocks in report
    MemoryTracker.ignore( '__m_mb_' )

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

    MemoryTracker.ignore ('SvcFactory<SegMemSvc>::create')
    MemoryTracker.ignore ('SvcFactory<JobIDSvc>::create')
    MemoryTracker.ignore ('SvcFactory<MuonTGC_CablingSvc>::create')
    MemoryTracker.ignore ('Service::Service')
    MemoryTracker.ignore ('AthService::AthService')
    MemoryTracker.ignore ('emplace_back_aux<Property')
    MemoryTracker.ignore ('PluginService::Factory2')
    MemoryTracker.ignore ('PropertyMgr::declareProperty')
    MemoryTracker.ignore ('SCT_ByteStreamErrorsSvc')
    MemoryTracker.ignore ('TClassTable::SortTable')
    MemoryTracker.ignore ('TWebPalette::TWebPalette')
    MemoryTracker.ignore ('Gaudi::PluginService::Factory')

    # ???
    MemoryTracker.ignoreCall ('TClass::GetClass')
    MemoryTracker.ignoreCall ('TClass::TClass')
    MemoryTracker.ignoreCall ('TClass::GetStreamerInfo')
    MemoryTracker.ignoreCall ('uuid_generate')
    MemoryTracker.ignoreCall ('TPluginManager::FindHandler')
    MemoryTracker.ignoreCall ('TROOT::RegisterModule')
    MemoryTracker.ignoreCall ('TStreamerInfo::TStreamerInfo')
    MemoryTracker.ignoreCall ('TStreamerInfo::Build')
    MemoryTracker.ignoreCall ('TClass::Property')
    MemoryTracker.ignoreCall ('register_xAOD_')
    MemoryTracker.ignoreCall ('xAOD::TDVCollectionProxy::TDVCollectionProxy')
    MemoryTracker.ignoreCall ('Property* PropertyMgr::declareProperty')

    # Come back to
    MemoryTracker.ignore ('ServiceManager::service')
    MemoryTracker.ignore ('TDVCollectionProxy')
    MemoryTracker.ignore ('allocator<IOVRange>')
    MemoryTracker.ignore ('ArenaCachingHandle')
    MemoryTracker.ignore ('ArenaHandle')
    MemoryTracker.ignore ('vector<SG::ArenaBase')
    MemoryTracker.ignore ('TConverterRegistry::AddConverter')
    MemoryTracker.ignore ('TEmulatedCollectionProxy::InitializeEx')
    MemoryTracker.ignore ('FileMgr::open')
    MemoryTracker.ignore ('FileMgr::close')
    MemoryTracker.ignore ('TMVA::DataSetFactory::Build')
    MemoryTracker.ignore ('SimpleProperty')
    MemoryTracker.ignore ('TDVCollectionFuncs::create_env') ###
    MemoryTracker.ignore ('PoolSvc::setObjPtr')
    MemoryTracker.ignore ('LWPool')
    MemoryTracker.ignore ('std::vector<LWPoolArea')
   
    atexit.register( MemoryTracker.atexit )

setup()
del setup

