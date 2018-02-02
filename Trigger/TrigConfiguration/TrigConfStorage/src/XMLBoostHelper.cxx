/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "./XMLBoostHelper.h"
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace boost::property_tree;


ptree
TrigConf::getAttributes(const boost::property_tree::ptree& pt) {
   return pt.get_child("<xmlattr>", boost::property_tree::ptree());
}

bool
TrigConf::readAttribute(const boost::property_tree::ptree& pt, const char* attrname, int& value) {
   string path = string("<xmlattr>.") + attrname;
   try {
      value = pt.get<int>(path);
   }
   catch(const ptree_bad_path&) {
      return false;
   }
   catch(const ptree_bad_data& e) {
      cerr << "Attribute '" << attrname << "' is not an int value: " << e.data<string>() << endl;
      throw;
   }
   return true;
}

bool
TrigConf::readAttribute(const boost::property_tree::ptree& pt, const char* attrname, unsigned int& value) {
   string path = string("<xmlattr>.") + attrname;
   try {
      value = pt.get<unsigned int>(path);
   }
   catch(const ptree_bad_path&) {
      return false;
   }
   catch(const ptree_bad_data& e) {
      cerr << "Attribute '" << attrname << "' is not an int value: " << e.data<string>() << endl;
      throw;
   }
   return true;
}

bool
TrigConf::readAttribute(const boost::property_tree::ptree& pt, const char* attrname, float& value) {
   string path = string("<xmlattr>.") + attrname;
   try {
      value = pt.get<float>(path);
   }
   catch(const ptree_bad_path&) { 
      return false;
   }
   catch(const ptree_bad_data& e) {
      cerr << "Attribute '" << attrname << "' is not a float value: " << e.data<string>() << endl;
      throw;
   }
   return true;
}

bool
TrigConf::readAttribute(const boost::property_tree::ptree& pt, const char* attrname, std::string& value) {
   string path = string("<xmlattr>.") + attrname;
   try {
      value = pt.get<string>(path);
   }
   catch(const ptree_bad_path&) {
      return false;
   }
   catch(const ptree_bad_data& e) {
      cerr << "Attribute '" << attrname << "' is not an string value: " << e.data<string>() << endl;
      throw;
   }
   return true;
}

bool
TrigConf::readAttribute(const boost::property_tree::ptree& pt, const char* attrname, bool& value) {
   string path = string("<xmlattr>.") + attrname;
   try {
      string x = pt.get<string>(path);
      value = 
         ( x == "1" ) ||
         ( x == "t" ) ||
         ( x == "T" ) ||
         ( x == "y" ) ||
         ( x == "Y" ) ||
         ( x == "true" ) ||
         ( x == "True" ) ||
         ( x == "yes" ) ||
         ( x == "YES" );
   }
   catch(const ptree_bad_path&) { return false; }

   return true;
}


bool
TrigConf::getTextContent(const boost::property_tree::ptree& pt, int& value) {
   value = pt.get_value<int>();
   return true;
}

bool
TrigConf::getTextContent(const boost::property_tree::ptree& pt, float& value) {
   value = pt.get_value<float>();
   return true;
}

bool
TrigConf::getTextContent(const boost::property_tree::ptree& pt, std::string& value) {
   value = boost::algorithm::trim_copy(pt.get_value<string>());
   return true;
}
