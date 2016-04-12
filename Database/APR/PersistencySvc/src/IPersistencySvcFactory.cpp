/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistencySvc/IPersistencySvcFactory.h"
#include "PersistencySvc.h"

pool::IPersistencySvcFactory::IPersistencySvcFactory(){}

pool::IPersistencySvc* pool::IPersistencySvcFactory::create(const std::string& className, IFileCatalog& catalog ){
  IPersistencySvc* ret = 0;
  if(className=="PersistencySvc") ret = new pool::PersistencySvc::PersistencySvc(catalog);
  return ret;
}

pool::IPersistencySvcFactory*
pool::IPersistencySvcFactory::get()
{
  static pool::IPersistencySvcFactory s_instance;
  return &s_instance;
}
