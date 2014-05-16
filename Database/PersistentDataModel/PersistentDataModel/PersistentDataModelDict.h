/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_PERSISTENTDATAMODELDICT_H
#define PERSISTENTDATAMODEL_PERSISTENTDATAMODELDICT_H

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"

// Need to instantiate iterators
namespace PersistentDataModelDict {
  struct tmp {
    std::vector<DataHeaderElement>::iterator m_dhit;
    std::vector<DataHeaderElement>::const_iterator m_dhcit;
  };
}

#endif
