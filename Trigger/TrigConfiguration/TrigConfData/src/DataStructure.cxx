/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigConfData/DataStructure.h"

using TV = boost::property_tree::ptree::value_type;  // tree-value type
//using namespace std;

TrigConf::DataStructure::DataStructure()
{}


TrigConf::DataStructure::DataStructure(const ptree & data) :
   m_initialized(true),
   m_dataPtr(&data)
{}


TrigConf::DataStructure::DataStructure(ptree && data) :
   m_dataSPtr(std::make_shared<ptree>(move(data)))
{}


TrigConf::DataStructure::~DataStructure()
{}


void
TrigConf::DataStructure::setData(const ptree & data)
{
   // need to add check that the data is not shared
   m_initialized = true;
   m_dataSPtr.reset();
   m_dataPtr = &data;
   update();
}


void
TrigConf::DataStructure::setData(ptree&& data)
{
   m_initialized = true;
   m_dataSPtr = std::make_shared<ptree>(move(data));
   m_dataPtr = &data;
   update();
}


void
TrigConf::DataStructure::clear()
{
   m_initialized = false;
   update();
}


bool
TrigConf::DataStructure::isValue() const {
   return data().empty();  // just a key->value pair, no children
}


std::string
TrigConf::DataStructure::getValue() const {
   const std::string value = data().get_value<std::string>();
   return value;
}


bool
TrigConf::DataStructure::hasAttribute(const std::string & key) const {
   const auto & child = data().get_child_optional( key );
   if( ! bool(child) ) // key does not exist
      return false; 
   return child.get().empty(); // if empty then it is an attribute, otherwise a child note
}

std::string
TrigConf::DataStructure::className() const {
   return "DataStructure";
}


bool
TrigConf::DataStructure::hasChild(const std::string & path) const {
   const auto & child = data().get_child_optional( path );
   return bool(child);
}


std::string 
TrigConf::DataStructure::operator[](const std::string & key) const
{
   const auto & obj = data().get_child(key);
   // check if the key points to a plain string value
   if ( !obj.empty() ) {
      if ( obj.front().first.empty() ) {
         throw std::runtime_error(std::string("Structure \"") + key + "\" is not a simple attribute but a list [], it needs to be accessed via getList(\"" + key + "\") -> vector<DataStructure>");
      } else { 
         throw std::runtime_error(std::string("Structure \"") + key + "\" is not a simple attribute but an object {}, it needs to be accessed via getObject(\"" + key + "\") -> DataStructure");
      }
   }
   return obj.data();
}

const std::string &
TrigConf::DataStructure::getAttribute(const std::string & key, bool ignoreIfMissing, const std::string & def) const
{
   const auto & obj = data().get_child_optional(key);
   if( !obj && ignoreIfMissing ) {
      return def;
   }
   // check if the key points to a plain string value
   if ( !obj.get().empty() ) {
      if ( obj.get().front().first.empty() ) {
         throw std::runtime_error(std::string("Structure \"") + key + "\" is not a simple attribute but a list [], it needs to be accessed via getList(\"" + key + "\") -> vector<DataStructure>");
      } else {
         throw std::runtime_error(std::string("Structure \"") + key + "\" is not a simple attribute but an object {}, it needs to be accessed via getObject(\"" + key + "\") -> DataStructure");
      }
   }
   return obj.get().data();
}

std::vector<TrigConf::DataStructure> 
TrigConf::DataStructure::getList(const std::string & pathToChild, bool ignoreIfMissing) const
{
   std::vector<TrigConf::DataStructure> childList;
   const auto & list = data().get_child_optional(pathToChild);
   if( ! list ) {
      if ( ignoreIfMissing ) {
         return childList;
      } else {
         throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" does not exist.");          
      }
   }

   // check if the pathToChild points to a list
   if ( list.get().empty() ) {
      throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" is not a list [] but a simple attribute, it needs to be accessed via [\"" + pathToChild + "\"] -> string");
   } else if ( ! list.get().front().first.empty() ) {
      throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" is not a list [] but an object {}, it needs to be accessed via getObject(\"" + pathToChild + "\") -> DataStructure");
   }

   childList.reserve(list.get().size());

   for( auto & childData : list.get() )
      childList.emplace_back( childData.second );

   return childList;
}


TrigConf::DataStructure
TrigConf::DataStructure::getObject(const std::string & pathToChild, bool ignoreIfMissing) const
{
   const auto & obj = data().get_child_optional(pathToChild);
   if( ! obj ) {
      if ( ignoreIfMissing ) {
         return DataStructure();
      } else {
         throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" does not exist.");          
      }
   }
   // check if the pathToChild points to an object
   if ( obj.get().empty() ) {
      throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" is not an object {} but a simple attribute, it needs to be accessed via [\"" + pathToChild + "\"] -> string");
   } else if ( obj.get().front().first.empty() ) {
      throw std::runtime_error(std::string("Structure \"") + pathToChild + "\" is not an object {} but a list [], it needs to be accessed via getList(\"" + pathToChild + "\") -> vector<DataStructure>");
   }
   return { obj.get() };
}


std::vector<std::string>
TrigConf::DataStructure::getKeys() const 
{
   std::vector<std::string> keys;
   if ( ! data().empty() &&
        ! data().front().first.empty() )
      {
         for( auto & entry : data() ) {
            keys.push_back(entry.first);
         }
      }
   return keys;
}


void
TrigConf::DataStructure::print(std::ostream & os) const
{
   printElement("", data(), 0, os);
   os << std::endl;

   //boost::property_tree::write_json( o, data() );
}

void
TrigConf::DataStructure::printElement(const std::string& key, const ptree & data, uint level, std::ostream & os)
{
   constexpr char del = '"';

   const std::string value = data.get_value<std::string>();
   
   if( data.empty() ) { // no children, so just a key->value pair
      uint n(4*level); while(n--) os << " ";
      os << del << key << del << ": " << del << value << del;
      return;
   }


   bool isArray ( data.begin()->first.empty() ); // dictionaries have keys, arrays don't
   { uint n(4*level); while(n--) os << " "; }
   if ( ! key.empty() )
      os << del << key << del << ": ";
   os << (isArray ? "[" : "{") << std::endl;

   size_t childCounter = data.size();
   for( const TV & x : data ) {
      printElement(x.first, x.second, level + 1, os);
      if( --childCounter ) os << ",";
      os << std::endl;
   }
   { uint n(4*level); while(n--) os << " "; }
   os << (isArray ? "]" : "}");

}
