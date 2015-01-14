/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLL1TopoMenuLoader.h"
#include "L1TopoConfig/L1TopoXMLParser.h"

#include <utility>

using namespace std;
using namespace TrigConf;

bool
XMLL1TopoMenuLoader::load( TXC::L1TopoMenu& menu ) {
   
   if ( !is_a("TOPO_MENU") ) return false;

   try {
      TXC::L1TopoXMLParser parser;
      
      parser.setConfiguration(pt());

      parser.parseConfiguration();

      menu = parser.takeMenu();

      
   }
   catch(std::exception & e) {
      TRG_MSG_ERROR("Caught exception from the topo menu parser, no topo menu will be available! Exception message: " << e.what());
   }

   return true;
}
