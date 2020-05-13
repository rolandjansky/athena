/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#ifndef TRIGCONFIO_TRIGDBHELPER_H
#define TRIGCONFIO_TRIGDBHELPER_H

#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/IQuery.h"

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/iostreams/stream.hpp"

#include <vector>
#include <string>
#include <memory>
#include <set>

#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/SchemaException.h"

/** helper class to store a query */
namespace TrigConf {
   class QueryDefinition {
   public:

      void addToTableList(const std::string & table, const std::string & table_short = "");

      void extendCondition(const std::string & condext);

      template<typename T>
      void extendOutput(const std::string & fieldName);

      template<typename T>
      void extendBinding(const std::string & fieldName);

      template<typename T>
      void setBoundValue(const std::string & fieldName, const T & value);

      std::unique_ptr< coral::IQuery >
      createQuery( coral::ISessionProxy * session );

      void setDataName(const std::string & dataName) {
         m_dataName = dataName;
      }

      const std::string & dataName() {
         return m_dataName;
      }
 
   private:
      std::vector<std::pair<std::string,std::string>> m_tables{}; // tables needed in the query
      std::string m_condition{""};  // where clause
      coral::AttributeList m_attList;  // select variables
      coral::AttributeList m_bindList; // bound variables
      std::string m_dataName; // the name of the field with the datablob
      std::set<std::string> m_bound; // bound variables that were set
   };

   template<typename T>
   void QueryDefinition::extendOutput(const std::string & fieldName) {
      m_attList.extend<T>( fieldName );
   }
   
   template<typename T>
   void QueryDefinition::extendBinding(const std::string & fieldName) {
      m_bindList.extend<T>( fieldName );
   }
   
   template<typename T>
   void QueryDefinition::setBoundValue(const std::string & fieldName, const T & value) {
      m_bindList[fieldName].setValue(value);
      m_bound.insert(fieldName);
   }

   void blobToPtree( const coral::Blob & blob, boost::property_tree::ptree & pt );

}

#endif
