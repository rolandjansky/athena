/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObjectBuffer.h 726071 2016-02-25 09:23:05Z krasznaa $
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
