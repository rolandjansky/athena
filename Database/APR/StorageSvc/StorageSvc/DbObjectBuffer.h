/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObjectBuffer.h 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//
//  Package    : StorageSvc (The pool framework)
//
//  Description: Management of the object Databases
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBOBJECTBUFFER_H
#define POOL_DBOBJECTBUFFER_H 1

/*
 *  POOL namespace
 */
namespace pool    {

  // Declaration of arbitrary object buffer class
  template <class T, int i> class DbObjectBuffer {
  public:
    /// Object buffer
    T buffer[i];
    /// Standard Constructor
    DbObjectBuffer()             { }
    /// Standard destructor
    virtual ~DbObjectBuffer()    { }
  };
}
#endif // POOL_DBOBJECTBUFFER_H
