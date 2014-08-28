/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoConfig_L1TopoXMLHelper
#define L1TopoConfig_L1TopoXMLHelper

#include "boost/property_tree/ptree.hpp"
#include <string>


namespace TXC {

   bool hasAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   std::string getAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   std::string getAttribute(const boost::property_tree::ptree & tree, const std::string & attr, const std::string & defval);

   unsigned int getUIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr);

   unsigned int getUIntAttribute(const boost::property_tree::ptree & tree, const std::string & attr, unsigned int defval);

   void printAttributes(const boost::property_tree::ptree & tree);

}

#endif
