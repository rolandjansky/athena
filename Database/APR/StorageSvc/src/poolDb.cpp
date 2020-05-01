/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Package    : System (The POOL project)
//
//  Description: Misc methods
//
//  @author      M.Frank
//====================================================================

#include "StorageSvc/pool.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <map>

using namespace std;
using namespace pool;

static const bool s_debug = (0 != ::getenv("POOL_TRACE")) || (0 != ::getenv("POOLDB_TRACE"));

static void startDebugger()  {
#ifdef WIN32
    _asm int 3
#endif
}

bool pool::doTrace() {
  return s_debug;
}

/// Debug break with printout and exception chaining
void pool::debugBreak( const string& src, 
                       const string& msg,
                       const exception& e,
                       bool rethrow)
{
  string m = "Unknown Source> ";
  m += e.what();
  m += "\n";
  m += src + "> " + msg;
  cout << "ERROR (pool): " << endl << m << endl;
  debugBreak();
  if ( rethrow )    {
    throw runtime_error(m);
  }
}

void pool::debugBreak(const string& src, const string& msg, bool rethrow) {
  string m = src + "> " + msg;
  cout << "ERROR (pool): " << endl << m << endl;
  debugBreak();
  if ( rethrow )    {
    throw runtime_error(m);
  }
}

void pool::debugBreak()   {
  if ( s_debug )    {
    startDebugger();
  }
  else  {
    //cout << "debugbreak (poolDb) However, breaks are DISABLED" 
    //          << endl;
  }
}

/// Translate acce mode to string
const char* pool::accessMode(pool::DbAccessMode mode)   {
  if      ( mode & pool::READ      ) 
    return "READ     ";
  else if ( mode & pool::UPDATE && mode & pool::CREATE )
    return "CREA/UPDA";
  else if ( mode & pool::UPDATE    ) 
    return "UPDATE   ";
  else if ( mode & pool::CREATE    )
    return "CREATE   ";
  else if ( mode & pool::NOT_OPEN  ) 
    return "NOT_OPEN ";
  else                                 
    return "UNKNOWN  ";
}
