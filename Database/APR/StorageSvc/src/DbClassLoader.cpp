/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbClassLoader.cpp 590734 2014-04-01 21:49:36Z gemmeren $
//====================================================================
//  DbDomainObj handle implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/DbClassLoader.h"
#include "StorageSvc/IClassHandler.h"
#include "StorageSvc/DbPrint.h"
// STL headers
#include <map>

using namespace pool;
using namespace std;
namespace pool {
  class ClassHandlers : public map<string, IClassHandler*> {
    bool m_release;
  public:
    ClassHandlers(bool rel=true) : m_release(rel) {}
    virtual ~ClassHandlers() {
      for ( iterator i=begin(); i != end(); ++i)  {
        if ( m_release && (*i).second ) (*i).second->release();
      }
      clear();
    }
  };
}

/// Constructor with initializing arguments
DbClassLoader::DbClassLoader()   {
  m_lcgHandlers = new ClassHandlers();
  m_natHandlers = new ClassHandlers(false);
}

/// Standard destructor
DbClassLoader::~DbClassLoader()   {
  delete m_lcgHandlers;
  delete m_natHandlers;
}

/// Release resource
void DbClassLoader::release()   {
  delete this;
}

/// Load the class unconditionally
DbStatus DbClassLoader::loadClass(const string& name)  {
  if ( !handler(name) )    {
    DbPrint log( "DbClassLoader" );
    log << DbPrintLvl::Warning << "No dictionary availible for class "
        << name << DbPrint::endmsg;
    return Error;
  }
  return Success;
}

/// Load the native class unconditionally
DbStatus DbClassLoader::loadNativeClass(const string& name)  {
  DbPrint log( "DbClassLoader" );
  log << DbPrintLvl::Warning << "No dictionary availible for native class " 
      << name << DbPrint::endmsg;
  return Error;
}

/// Load the native class unconditionally
DbStatus DbClassLoader::loadNativeClasses(const vector<string>& names) {
  for (vector<string>::const_iterator i=names.begin(); i!=names.end(); ++i)  {
    DbStatus sc = loadNativeClass(*i);
    if ( !sc.isSuccess() )  {
      return sc;
    }
  }
  return Success;
}

/// Unload the class
DbStatus DbClassLoader::unloadClass(const string& name)  {
  IClassHandler* h = handler(name);   // Classes cannot be unloaded...
  if ( h )    {
    return removeHandler(name);
  }
  return Error;
}

/// Access reflection information of a given class
IClassHandler* DbClassLoader::handler(const string& name, bool force_load) {
  ClassHandlers::iterator it = m_lcgHandlers->find(name);
  if ( it == m_lcgHandlers->end() && force_load )  {
    loadClass(name);
    it = m_lcgHandlers->find(name);
  }
  return (it == m_lcgHandlers->end()) ? 0 : (*it).second;
}

/// Access reflection information of a given class
IClassHandler* DbClassLoader::nativeHandler(const string& name, bool force_load) {
  ClassHandlers::iterator it = m_natHandlers->find(name);
  if ( it == m_natHandlers->end() && force_load )  {
    loadNativeClass(name);
    it = m_natHandlers->find(name);
  }
  return (it == m_natHandlers->end()) ? 0 : (*it).second;
}

/// Set a user handler in place to deal with special class treatment
DbStatus DbClassLoader::setHandler( const string& lcg_name, 
                                    const string& nat_name, 
                                    IClassHandler* hndl)
{
  IClassHandler* h = handler(lcg_name, false);   // Classes cannot be unloaded...
  if ( h ) h->release();
  if ( hndl )   {
    (*m_lcgHandlers)[lcg_name] = hndl;
    (*m_natHandlers)[nat_name] = hndl;
    return Success;
  }
  return Error;
}

/// Remove a user handler
DbStatus DbClassLoader::removeHandler(const string& name)  {
  ClassHandlers::iterator it = m_lcgHandlers->find(name);
  if ( it != m_lcgHandlers->end() )  {
    IClassHandler* h = (*it).second;
    m_lcgHandlers->erase(it);
    for (it=m_natHandlers->begin(); it != m_natHandlers->end(); ++it)  {
      if ( (*it).second == h )  {
        m_natHandlers->erase(it);
        break;
      }
    }
    if ( h ) h->release();
    return Success;
  }
  return Error;
}
