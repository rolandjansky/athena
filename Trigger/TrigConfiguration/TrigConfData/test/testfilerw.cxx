/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/DataStructure.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include <iostream>

using namespace std;

int main() {

   string testfilepath = string(getenv("TESTFILEPATH")) + "/";
   const string file = testfilepath + "testfile.json";

   boost::property_tree::ptree data;

   try {
      boost::property_tree::read_json(file, data);
   }
   catch (const boost::property_tree::json_parser_error& e) {
      cerr << "Could either not locate or parse the file " << file << endl;
      return 1;
   }

   boost::property_tree::write_json(cout, data);

   TrigConf::DataStructure ds(data);
   cout << "emptyAtt: '" << ds.getAttribute("emptyAtt") << "'" << endl;
   cout << "nonEmptyAtt: '" << ds.getAttribute("nonEmptyAtt") << "'" << endl;
   for( auto & item : ds.getList("fullList") ) {
      cout << item.getValue() << endl;
   }
   for( auto & item : ds.getList("emptyList") ) {
      cout << item.getValue() << endl;
   }


   return 0;
}
