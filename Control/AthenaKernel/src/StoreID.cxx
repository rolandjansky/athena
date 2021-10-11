/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaKernel/StoreID.h"
#include <string>
#include <vector>
#include <utility>

static const std::vector<std::string> STORENAMES {
    "StoreGateSvc",
    "DetectorStore",
    "ConditionStore",
    "MetaDataStore",
    "InputMetaDataStore",
    "PileupStore",
    "UnknownStore"
      };


StoreID::type 
StoreID::findStoreID(const std::string& storeNamePrefix) {
  // Account for AthenaMT stores that start with {digits}_
  size_t ist (0);
  if (::isdigit(storeNamePrefix.at(0))) {
    ist = storeNamePrefix.find('_',0) +1;
  }
  //slightly faster

  auto c(storeNamePrefix.at(ist));
  switch(c){
  case 'C':
    {
      return StoreID::CONDITION_STORE;
      break;
    }
  case 'D':
    {
      return StoreID::DETECTOR_STORE;
      break;
    }
  case 'E':
    {
      return StoreID::EVENT_STORE;
      break;
    }
  case 'I':
    {
      return StoreID::METADATA_STORE;
      break;
    }
  case 'M':
    {
      return StoreID::METADATA_STORE;
      break;
    }
  case 'P':
    {
      return StoreID::PILEUP_STORE;
      break;
    }
  case 'S':
    {
      return StoreID::EVENT_STORE;
      break;
    }
  default:
    {
      return StoreID::UNKNOWN;
      break;
    }
  }
  
  return StoreID::UNKNOWN;
}

const std::string&
StoreID::storeName(const StoreID::type& id) {
  return STORENAMES[ (unsigned int)id ];
}
  
