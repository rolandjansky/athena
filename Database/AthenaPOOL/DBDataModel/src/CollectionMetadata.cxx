/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CollectionMetadata.cxx
 *  @brief This file contains the implementation for the CollectionMetadata class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "DBDataModel/CollectionMetadata.h"

#include "CollectionBase/ICollectionMetadata.h"

//______________________________________________________________________________
CollectionMetadata::CollectionMetadata(const pool::ICollectionMetadata* ptr) {
   if (ptr != 0) {
      for (pool::ICollectionMetadata::const_iterator iter = ptr->begin(), last = ptr->end();
                      iter != last; ++iter) {
         this->insert(std::pair<std::string, std::string>(iter.key(), iter.value()));
      }
   }
}
//______________________________________________________________________________
CollectionMetadata::CollectionMetadata(const std::map<std::string, std::string>& keyValuePair) :
		std::map<std::string, std::string>(keyValuePair) {
}
//______________________________________________________________________________
CollectionMetadata::~CollectionMetadata() {}
