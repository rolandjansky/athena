/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONFRAGMENTDATA_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONFRAGMENTDATA_H

#include <string>
#include <map>

namespace pool {

   namespace RelationalCollection {

      class FragmentData
      {
     public:
        FragmentData() : m_currentRowId(0), m_remainingIds(0) {}
      
        int                getCurrentRowId()         { return m_currentRowId; }
        int                getNextRowId();
        bool                hasAllocatedIds()         { return m_remainingIds > 0; }

        const std::string&        getDataTableName() const { return m_dataTableName; }
        const std::string&        getLinksTableName() const { return m_linksTableName; }
        
        std::string&        dataTableName() { return m_dataTableName; }
        std::string&        linksTableName() { return m_linksTableName; }
        
        void                setCurrentRowId(int id) { m_currentRowId = id; } 
        void                allocateIDs(int first, int range ) { m_currentRowId =  first; m_remainingIds = range; }
      
     protected:
        int                m_currentRowId;
        int                m_remainingIds;
        std::string        m_dataTableName;
        std::string        m_linksTableName;
      };

      

      class FragmentDataMap : public std::map< std::string, FragmentData >
      {
     public:
        const FragmentData& fragmentData(const std::string& fragmentName) const {
           return this->find( fragmentName )->second;
        }
      };
      
   }
}

inline
int pool::RelationalCollection::FragmentData::getNextRowId()
{
   if( hasAllocatedIds() ) {
      m_remainingIds--;
      return m_currentRowId++;
   }

   return -1;   // replace with exception
}

   
#endif


