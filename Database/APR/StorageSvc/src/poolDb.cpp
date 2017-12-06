/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: poolDb.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbFileFactory implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Management of the open Db files
//
//  @author      M.Frank
//====================================================================

#include "StorageSvc/pool.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbPrint.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <map>
using namespace std;

using namespace pool;
DbPrintLvl::MsgLevel DbPrintLvl::outputLvl = None;

static bool s_debug = (0 != ::getenv("POOL_TRACE")) || (0 != ::getenv("POOLDB_TRACE"));

static void startDebugger()  {
#ifdef WIN32
    _asm int 3
#endif
}

bool pool::doTrace() {
  return s_debug;
}

void pool::enableBreak(bool yes_no)   {
  s_debug = yes_no;
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
