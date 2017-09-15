#include "AthenaKernel/StoreID.h"
#include <string>
#include <vector>
#include <utility>

static const std::vector<std::string> STORENAMES {
    "StoreGateSvc",
    "DetectorStore",
    "ConditionStore",
    "InputMetaDataStore",
    "MetaDataStore",
    "SpareStore",
    "PileupStore",
    "UnknownStore"
      };


StoreID::type 
StoreID::findStoreID(const std::string& storeNamePrefix) {
  // Account for AthenaMT stores that start with {digits}_
  size_t ist (0);
  if (::isdigit(storeNamePrefix.at(0))) {
    ist = storeNamePrefix.find("_",0) +1;
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
      return StoreID::SIMPLE_STORE;
      break;
    }
  case 'P':
    {
      return StoreID::PILEUP_STORE;
      break;
    }
  case 'S':
    {
      if (storeNamePrefix.at(ist+1)=='p'){
	return StoreID::SPARE_STORE;
      }else{
	return StoreID::EVENT_STORE;
      }
      break;
    }
  case 'T':
    {
      return StoreID::METADATA_STORE;
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
  
