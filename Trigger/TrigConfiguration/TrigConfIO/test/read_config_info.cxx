/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileLoader.h"

#include "TrigConfData/L1TopoMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTMenu.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

bool testSize( const TrigConf::L1TopoMenu & l1topo, uint l1toposize,
               const TrigConf::L1Menu & l1, uint l1size,
               const TrigConf::HLTMenu & hlt, uint hltsize )
{
   return 
      ( l1topo.size() == l1toposize ) &&
      ( l1.size() == l1size ) &&
      ( hlt.size() == hltsize );
}

int main() {
   
   string testfilepath = string(getenv("TESTFILEPATH")) + "/"; 
   
   const string l1topofn = testfilepath + "L1Topoconfig_Physics_pp_v7.json";
   const string l1fn     = testfilepath + "LVL1config_Physics_pp_v7.json";
   const string hltfn    = testfilepath + "HLTconfig_Physics_pp_v7.json";
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.setLevel(TrigConf::MSGTC::WARNING); 
   
   TrigConf::L1TopoMenu l1topomenu;
   TrigConf::L1Menu l1menu;
   TrigConf::HLTMenu hltmenu;
   bool loadTestL1Topo = fileLoader.loadFile( l1topofn, l1topomenu ).isSuccess();
   cout << "Test loading of L1Topo file      ... " << ( loadTestL1Topo ? "passed" : "failed" ) << endl;

   bool loadTestL1 = fileLoader.loadFile( l1fn, l1menu ).isSuccess();
   cout << "Test loading of L1 file          ... " << ( loadTestL1 ? "passed" : "failed" ) << endl;

   bool loadTestHLT = fileLoader.loadFile( hltfn, hltmenu ).isSuccess();
   cout << "Test loading of HLT file         ... " << ( loadTestHLT ? "passed" : "failed" ) << endl;
   
   if ( ( loadTestL1Topo && loadTestL1 && loadTestHLT ) == false ) return 1; // can't not continue with the other tests

   bool allSucceeded = true;

   bool sizeTest = testSize( l1topomenu, 3, l1menu, 2, hltmenu, 2);
   if (l1menu.thresholds().size() != 2)
      sizeTest = false;
   if ( ! sizeTest ) allSucceeded = false;
   cout << "Test size of menus               ... " << ( sizeTest ? "passed" : "failed" ) << endl;


   bool itemAccessTest = true;
   for(const auto & item : l1menu ) {
      item.name(); item.ctpId(); item.definition(); item.triggerType();
   }
   for(const auto & thr : l1menu.thresholds() ) {
      try {
         thr.thresholdValues();
         thr.name(); thr.type(); thr.mapping();
         if( const auto & cable = thr.cable() ) {
            cable["input"]; cable["connector"];
         }
      }
      catch(...) {
         itemAccessTest = false;
      }
   }
   if ( ! itemAccessTest ) allSucceeded = false;
   cout << "Test access of items             ... " << ( itemAccessTest ? "passed" : "failed" ) << endl;



   bool chainAccessTest = true;
   for( auto & ch : hltmenu ) {
      ch.name();
      ch.counter();
      ch.l1item();
      for( auto & s : ch.streams() ) {
         try {
            s["type"];
            s["name"];
            s["obeyLB"];
            s["prescale"];
         }
         catch(...) {
            chainAccessTest = false;
         }
      }
      std::string g1(""),g2("");
      for( auto & g : ch.groups() ) {
         g1 += g;
      }
      for( auto & g : ch.getList("groups") ) {
         g2 += g["name"];
      }
      if(g1 != g2)
         chainAccessTest = false;
   }
      
   if ( ! chainAccessTest ) allSucceeded = false;
   cout << "Test access of chains            ... " << ( chainAccessTest ? "passed" : "failed" ) << endl;





   /* this will be implemented later
   const std::string fn1{"testout_boostWriter.json"};
   const std::string fn2{"testout_manualWriter.json"};

   boost::property_tree::write_json( fn1, menu.data() );
   
   std::ofstream of2;
   of2.open(fn2);
   menu.print(of2);
   of2.close();

   menu.print();
   */

   return allSucceeded ? 0 : 1;

}
