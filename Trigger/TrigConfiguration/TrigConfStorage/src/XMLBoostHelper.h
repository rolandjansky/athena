/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLBoostHelper
#define TrigConf_XMLBoostHelper

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "boost/foreach.hpp"

#include <string>

namespace TrigConf {

   boost::property_tree::ptree getAttributes(const boost::property_tree::ptree& pt);

   bool readAttribute(const boost::property_tree::ptree& pt, const char* attrname, int& value);

   bool readAttribute(const boost::property_tree::ptree& pt, const char* attrname, unsigned int& value);

   bool readAttribute(const boost::property_tree::ptree& pt, const char* attrname, float& value);

   bool readAttribute(const boost::property_tree::ptree& pt, const char* attrname, std::string& value);

   bool readAttribute(const boost::property_tree::ptree& pt, const char* attrname, bool& value);

   bool getTextContent(const boost::property_tree::ptree& pt, int& value);

   bool getTextContent(const boost::property_tree::ptree& pt, float& value);

   bool getTextContent(const boost::property_tree::ptree& pt, std::string& value);
}




#endif
