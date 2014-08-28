/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/Exception.h"
#include "L1TopoConfig/L1TopoXMLHelper.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using boost::property_tree::ptree;

void
TXC::printAttributes(const ptree & tree) {
  
   // initialize attributes ptree 
   ptree attributes = tree.get_child("<xmlattr>", ptree());
  
   // iterate through elements
   for(const ptree::value_type & a: attributes) {
      string attrName = a.first;
      string attrVal = a.second.data();
      cout << attrName << " : " << attrVal << endl;
   }
}

bool
TXC::hasAttribute(const ptree & tree, const string & attr) {

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


namespace {
   string
   readAttribute(const ptree & tree, const string & attr) {
 
      // initialize attributes ptree 
      ptree attributes = tree.get_child("<xmlattr>", ptree());
  
      // iterate through children
      for(const ptree::value_type &a : attributes) {
         if(a.first != attr) continue;
         return a.second.data(); //a.second.data is the value!
      }
      return "";
   }
}


std::string
TXC::getAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TCS_EXCEPTION("getAttribute(): Attribute " << attr << " does not exist");
   }
   return readAttribute(tree,attr);
}

std::string
TXC::getAttribute(const ptree & tree, const string & attr, const std::string & defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   return readAttribute(tree,attr);
}


unsigned int
TXC::getUIntAttribute(const ptree & tree, const string & attr) {
   if( ! hasAttribute(tree, attr) ) {
      TCS_EXCEPTION("getUIntAttribute(): Attribute " << attr << " does not exist");
   }
   unsigned int ret_value{0};
   try {
   ret_value = boost::lexical_cast<unsigned int, string>(readAttribute(tree,attr));
   }
   catch(const boost::bad_lexical_cast & bc) {
      TCS_EXCEPTION("getUIntAttribute(): Attribute '" << attr << "' is not an unsigned int (it is " << readAttribute(tree,attr) << ")");
   }
   return ret_value;
}

unsigned int
TXC::getUIntAttribute(const ptree & tree, const string & attr, unsigned int defval) {
   if( ! hasAttribute(tree, attr) )
      return defval;
   return boost::lexical_cast<unsigned int, string>(readAttribute(tree,attr));
}
