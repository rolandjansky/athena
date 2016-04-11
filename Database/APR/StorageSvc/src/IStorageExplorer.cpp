/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IStorageExplorer.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#include "PersistentDataModel/Guid.h"
#include "StorageSvc/IStorageExplorer.h"

// Declaration of the interface ID needs to be unique within ONE process
const Guid& pool::IStorageExplorer::interfaceID()  {
  static const Guid id(true);
  return id;
}
