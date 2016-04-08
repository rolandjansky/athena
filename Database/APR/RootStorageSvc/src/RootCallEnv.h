/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  class RootKeyContainer;
  class RootCallEnv;
  class DataCallBack;

  /** Helper structure to access context from streamer
      @author  M.Frank
      @date    1/8/2002
      @version 1.0
  */
  class RootCallEnv   {

  protected:
    DataCallBack*     call;
    RootDataPtr&      context;
  public:
    RootCallEnv(DataCallBack* cb, RootDataPtr& ctxt);
    ~RootCallEnv()    {     }
    void read (TBuffer& buff);
    void write(TBuffer& buff);
  };
}

#endif // POOL_ROOTDB_ROOTCALLENV_H
