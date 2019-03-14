/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTDB_ROOTCALLENV_H
#define POOL_ROOTDB_ROOTCALLENV_H

#include "RootDataPtr.h"
class TBuffer;

/*
 *   POOL namespace
 */
namespace pool  {
  class DbTypeInfo;

  /** Helper structure to access context from streamer
      @author  M.Frank
  */
  class RootCallEnv   {

  protected:
    RootDataPtr         m_object;
    const DbTypeInfo    *m_objType;

 public:
    RootCallEnv( RootDataPtr object, const DbTypeInfo *typ );
    ~RootCallEnv()    {     }
    void read (TBuffer& buff);
    void write(TBuffer& buff);
  };
}

#endif // POOL_ROOTDB_ROOTCALLENV_H
