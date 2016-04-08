/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLCORE_CONTEXT_H
#define POOLCORE_CONTEXT_H

#include "POOLCore/Properties.h"


namespace pool {

  /// Class defining the entry point to the framework component model for POOL.
  class Context {

    public:

    Context();

    virtual ~Context();

    Properties& properties();
    
    private:

    Properties m_properties;
    
  };

}

inline
pool::Properties&
pool::Context::properties(){
  return m_properties;
}


#endif
