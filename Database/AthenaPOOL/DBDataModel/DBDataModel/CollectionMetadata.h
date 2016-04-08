/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBDATAMODEL_COLLECTIONMETADATA_H
#define DBDATAMODEL_COLLECTIONMETADATA_H

/** @file CollectionMetadata.h
 *  @brief This file contains the class definition for the CollectionMetadata class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "DataModel/DataVector.h"

#include <string>
#include <map>

// Forward declarations
namespace pool {
   class ICollectionMetadata;
}

/** @class CollectionMetadata 
 *  @brief This class is the StoreGate data object for collection metadata. 
 **/
class CollectionMetadata : public std::map<std::string, std::string> {
public: // Constructor and Destructor
   CollectionMetadata(const std::map<std::string, std::string>& keyValuePairs);
   CollectionMetadata(const pool::ICollectionMetadata* ptr);
   virtual ~CollectionMetadata();
};

typedef DataVector<CollectionMetadata> CollectionMetadataContainer;

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(CollectionMetadata, 1253814833, 1)
CLASS_DEF(CollectionMetadataContainer, 1296887519, 1)
#endif
