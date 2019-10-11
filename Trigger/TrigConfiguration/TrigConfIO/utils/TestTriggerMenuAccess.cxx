/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1TopoMenu.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

int main(int /*argc*/, char** /*argv*/) {

   const char* env_p = std::getenv("AtlasVersion");
   std::string version(env_p);

   // HLT menu test file
   std::string l1topo_filename = "";
   std::string l1_filename = "";
   std::string hlt_filename = "./HLTmenu_LS2_v1_";
   hlt_filename += (version+".json");
   

   // file loader
   TrigConf::JsonFileLoader fileLoader;

   // HLT
   if( hlt_filename != "" ) {
      cout << "===========================" << endl
           << "=====                 =====" << endl
           << "===== Test HLT access =====" << endl
           << "=====                 =====" << endl
           << "===========================" << endl << endl;
  
      TrigConf::HLTMenu hltmenu;
      fileLoader.loadFile( hlt_filename, hltmenu);
      
      cout << "Loaded the menu " << hltmenu.name() << endl;
      cout << "Menu has " << hltmenu.size() << " chains, going to print the first 3." << endl;
      int np = 3;
      for( auto & ch : hltmenu ) {
         cout << endl << "Chain '" << ch.name() << "' with counter " << ch["counter"] << " and seeded by " << ch["l1item"] << endl;
         cout << "has the following L1 thresholds: " << endl << "  ";
         bool printComma = false;
         for( auto & thr : ch.l1thresholds() ) {
            if (printComma)
               cout << ", ";
            cout << thr; 
            printComma = true;
         }
         cout << endl;
         cout << "streams:" << endl;
         for( auto & s : ch.streams() ) {
            cout << "  " << s["type"] << "_" << s["name"] 
                 << (s["obeyLB"]=="yes" ? " (obeys LB" : " (does not obey LB") << " and has prescale " << s["prescale"] << ")" << endl;
         }
         cout << "and groups:" << endl << "  ";
         printComma = false;
         for( auto & g : ch.groups() ) {
            if (printComma)
               cout << ", ";
            cout << g;
            printComma = true;
         }
         cout << endl;
         if(--np==0) break;
      }
   }

   // LVL1
   if( l1_filename != "" ) {
      cout << endl
           << "==========================" << endl
           << "=====                =====" << endl
           << "===== Test L1 access =====" << endl
           << "=====                =====" << endl
           << "==========================" << endl << endl;

      TrigConf::L1Menu l1menu;
      fileLoader.loadFile( l1_filename, l1menu);
      cout << "Loaded the L1 menu " << l1menu.name() << endl;

      // items
      cout << endl << "L1 menu has " << l1menu.size() << " items, going to print only the first." << endl;
      int ni = 1; // print only first 1
      for(const auto & item : l1menu ) {
         cout << endl << item.name() << "(ctpId " << item.ctpId() << ") has definition '" << item.definition() << "' and triggerType " << item.triggerType()<< endl;
         cout << "Full printout:" << endl;
         item.print();
         if(--ni==0) break;
      }

      // thresholds
      cout << endl << "L1 menu has " << l1menu.thresholds("EM").size() << " EM thresholds, going to print only the first." << endl;
      ni = 1; // print only first 1
      for(const auto & thr : l1menu.thresholds("EM") ) {
         const auto & thrValues = thr.thresholdValues();
         cout << endl << thr.name() << " of type " << thr.type() << " (mapping " << thr.mapping() << ") has " << thrValues.size() << " thresholdValues. Cabling info:" << endl;
         if( const auto & cable = thr.cable() ) {
            cout << "Cabling info: " << cable["input"] << " " << cable["connector"] << endl;
         } else {
            cout << "No cabling info available for this threshold" << endl;
         }
         if(--ni==0) break;
      }
   


      cout << endl
           << "=====" << endl
           << "===== Test access to sub config" << endl
           << "=====" << endl << endl;

      // CaloInfo, etc
      cout << "Loading CaloInfo.MuctpiInfo" << endl;
      TrigConf::DataStructure muctpiConf;
      fileLoader.loadFile( l1_filename, muctpiConf, "MuctpiInfo");
      muctpiConf.print();

      cout << "Loading CaloInfo.METSignificance" << endl;
      TrigConf::DataStructure metSignificance;
      fileLoader.loadFile( l1_filename, metSignificance, "CaloInfo.METSignificance");
      metSignificance.print();

      cout << "Loading CaloInfo" << endl;
      TrigConf::DataStructure caloInfo;
      fileLoader.loadFile( l1_filename, caloInfo, "CaloInfo"); // only works when the data that CaloInfo points to is a well-formed
      boost::property_tree::write_json( "testOutput_CaloInfo.json", caloInfo.data() );
      cout << "EM scale : " << caloInfo["global_em_scale"] << endl;
      for( auto & iso : caloInfo.getList("Isolation") ) {
         cout << iso["thrtype"] << endl;
         for( auto & par : iso.getList("Parametrization") ) {
            cout << par["isobit"] << " with slope " << par["slope"] << " and upperlimit " << par["upperlimit"] << endl;
         }
      }
  
   }




   return 0;
}
