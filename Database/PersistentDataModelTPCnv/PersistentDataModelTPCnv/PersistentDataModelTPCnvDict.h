/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_PERSISTENTDATAMODELTPCNVDICT_H
#define PERSISTENTDATAMODEL_PERSISTENTDATAMODELTPCNVDICT_H

#include "PersistentDataModelTPCnv/DataHeader_p3.h"
#include "PersistentDataModelTPCnv/DataHeader_p4.h"
#include "PersistentDataModelTPCnv/DataHeader_p5.h"
#include "PersistentDataModelTPCnv/DataHeader_p6.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p5.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p6.h"

// Need to instantiate iterators
namespace PersistentDataModelTPCnvDict {
   // template instantiations
   std::pair<Guid, unsigned int> dummy_dbRecord;
   std::vector<std::pair<Guid, unsigned int> > dummy_dbRecords;
   std::pair<std::string, unsigned int> dummy_sgRecord;
   std::pair<Guid, std::pair<std::string, unsigned int> > dummy_objRecord;
   std::vector<std::pair<Guid, std::pair<std::string, unsigned int> > >dummy_objRecords;
}

#endif
