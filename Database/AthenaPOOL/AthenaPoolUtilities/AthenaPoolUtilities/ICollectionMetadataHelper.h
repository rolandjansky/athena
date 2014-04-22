/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METADATAHELPER_H
#define METADATAHELPER_H

//#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "DBDataModel/CollectionMetadata.h"
#include "AthenaPoolUtilities/TagMetadataKey.h"

#include <string>

class ICollectionMetadataHelper
{
public:
   ICollectionMetadataHelper(const CollectionMetadata& cmd) : m_cmd(cmd) {}
   ICollectionMetadataHelper(const pool::ICollectionMetadata& cmd) : m_cmd(&cmd) {}
   bool              existsKey(const TagMetadataKey&) const;
   const std::string getValueForKey(const TagMetadataKey&) const;
private:
   CollectionMetadata m_cmd;
};

inline bool 
ICollectionMetadataHelper::existsKey(const TagMetadataKey& tmk) const
{
   bool res = false;
   if   (m_cmd.find(tmk.toString()) != m_cmd.end()) res = true;
   else if (m_cmd.find(tmk.key()) != m_cmd.end()) res = true;
   return res;
}

inline const std::string
ICollectionMetadataHelper::getValueForKey(const TagMetadataKey& tmk) const
{
   CollectionMetadata::const_iterator iter = m_cmd.find(tmk.toString());
   if(iter == m_cmd.end()) {
      iter = m_cmd.find(tmk.key());
   }
   if (iter != m_cmd.end()) return iter->second;
   return std::string();
}

#endif
