/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_IPERSISTENCYSVCFACTORY_H
#define PERSISTENCYSVC_IPERSISTENCYSVCFACTORY_H

#include "PersistencySvc/IPersistencySvc.h"

namespace pool {

  // This class signature has been kept only for backward compatibility.
  class IPersistencySvcFactory
  {
  public:
    static IPersistencySvcFactory* get();

    IPersistencySvc* create(const std::string& className, IFileCatalog& catalog );
  private:
    IPersistencySvcFactory();
  };

}

#endif
