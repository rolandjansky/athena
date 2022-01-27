/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfMuctpi_MuctpiXMLHelper
#define TrigConfMuctpi_MuctpiXMLHelper

#include "TrigConfBase/TrigConfMessaging.h"
#include "boost/property_tree/ptree.hpp"
#include <string>


class MuctpiXMLHelper : TrigConf::TrigConfMessaging {
public:
   MuctpiXMLHelper();

   std::string readAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   bool hasAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   std::string getAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   std::string getAttribute(const boost::property_tree::ptree & tree, const std::string & attr, const std::string & defval);

   int getIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   int getIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr, int defval);

   unsigned int getUIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   unsigned int getUIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr, unsigned int & defval);

   float getFloatAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   float getFloatAttribute(const boost::property_tree::ptree & tree, const std::string & attr, float & defval);

   void printAttributes(const boost::property_tree::ptree & tree);

};

#endif
