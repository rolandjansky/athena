/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MuctpiXMLHelper.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using boost::property_tree::ptree;

MuctpiXMLHelper::MuctpiXMLHelper() :
  TrigConf::TrigConfMessaging("MuctpiXML")
{}

void
MuctpiXMLHelper::printAttributes(const ptree & tree) {
  
   // initialize attributes ptree 
   ptree attributes = tree.get_child("<xmlattr>", ptree());
  
   // iterate through elements
   for(const ptree::value_type & a: attributes) {
      string attrName = a.first;
      string attrVal = a.second.data();
      std::cout << attrName << " : " << attrVal << std::endl;
   }
}

bool
MuctpiXMLHelper::hasAttribute(const ptree & tree, const string & attr) {

   // initialize attributes ptree 
   ptree attributes = tree.get_child("<xmlattr>", ptree());
  
   if(attributes.empty()) return false;
  
   for(const ptree::value_type &a: attributes) {
      string attrName = a.first;
      if(attrName == attr) {
         return true;
      }
   }
   return false;
}


string
MuctpiXMLHelper::readAttribute(const ptree & tree, const string & attr) {
 
      // initialize attributes ptree 
      ptree attributes = tree.get_child("<xmlattr>", ptree());
  
      // iterate through children
      for(const ptree::value_type &a : attributes) {
         if(a.first != attr) continue;
         return a.second.data(); //a.second.data is the value!
      }
      return "";
}


std::string
MuctpiXMLHelper::getAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TRG_MSG_WARNING("attribute " << attr << " does not exist");
      return "";
   }
   return readAttribute(tree,attr);
}

std::string
MuctpiXMLHelper::getAttribute(const ptree & tree, const string & attr, const std::string & defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   return readAttribute(tree,attr);
}


int
MuctpiXMLHelper::getIntAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TRG_MSG_WARNING("attribute " << attr << " does not exist");
      return 0;
   }

   int ret_value{0};
   try {
      ret_value = boost::lexical_cast<int, string>(readAttribute(tree,attr));
   }
   catch(const boost::bad_lexical_cast & bc) {
      TRG_MSG_ERROR("attribute '" << attr << "' is not an int (it is '" << readAttribute(tree,attr) << "')");
   }
   return ret_value;
}

int
MuctpiXMLHelper::getIntAttribute(const ptree & tree, const string & attr, int defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   int ret_value{0};
   try {
      ret_value = boost::lexical_cast<int, string>(readAttribute(tree,attr));
   }
   catch(const boost::bad_lexical_cast & bc) {
     TRG_MSG_ERROR("attribute '" << attr << "' is not an int (it is '" << readAttribute(tree,attr) << "')");
   }
   return ret_value;
}


unsigned int
MuctpiXMLHelper::getUIntAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TRG_MSG_WARNING("attribute " << attr << " does not exist");
      return 0;
   }
   unsigned int ret_value{0};
   try {
      ret_value = boost::lexical_cast<unsigned int, string>(readAttribute(tree,attr));
   }
   catch(const boost::bad_lexical_cast & bc) {
      TRG_MSG_ERROR("attribute '" << attr << "' is not an unsigned int (it is " << readAttribute(tree,attr) << ")");
   }
   return ret_value;
}

unsigned int
MuctpiXMLHelper::getUIntAttribute(const ptree & tree, const string & attr, unsigned int & defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   return boost::lexical_cast<unsigned int, string>(readAttribute(tree,attr));
}


float
MuctpiXMLHelper::getFloatAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TRG_MSG_WARNING("attribute " << attr << " does not exist");
      return 0;
   }
   float ret_value{0};
   try {
      ret_value = boost::lexical_cast<float, string>(readAttribute(tree,attr));
   }
   catch(const boost::bad_lexical_cast & bc) {
      TRG_MSG_ERROR("attribute '" << attr << "' is not an float (it is " << readAttribute(tree,attr) << ")");
      printAttributes(tree);
   }
   return ret_value;
}

float
MuctpiXMLHelper::getFloatAttribute(const ptree & tree, const string & attr, float & defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   return boost::lexical_cast<float, string>(readAttribute(tree,attr));
}
