/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1TopoMenu.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

int main(int /*argc*/, char** /*argv*/) {

   // HLT menu test file
   const std::string l1topo_filename = "TrigConfIO/L1Topoconfig_Physics_pp_v7_21.1.40.json";
   const std::string l1_filename = "TrigConfIO/LVL1config_Physics_pp_v7_21.1.40.json";
   const std::string hlt_filename = "TrigConfIO/HLTconfig_Physics_pp_v7_21.1.40.json";

   // file loader
   TrigConf::JsonFileLoader fileLoader;

   cout << "===========================" << endl
        << "=====                 =====" << endl
        << "===== Test HLT access =====" << endl
        << "=====                 =====" << endl
        << "===========================" << endl << endl;

  
   // HLT
   TrigConf::HLTMenu hltmenu;
   fileLoader.loadFile( hlt_filename, hltmenu);
      
   cout << "Loaded the menu " << hltmenu.name() << endl;
   cout << "Menu has " << hltmenu.size() << " chains, going to print the first 3." << endl;
   int np = 3;
   for( auto & ch : hltmenu ) {
      cout << endl << "Chain '" << ch.name() << "' with counter " << ch["counter"] << " and seeded by " << ch["l1item"] << endl;
      cout << "has the following streams:" << endl;
      for( auto & s : ch.streams() ) {
         cout << "  " << s["type"] << "_" << s["name"] 
              << (s["obeyLB"]=="yes" ? " (obeys LB" : " (does not obey LB") << " and has prescale " << s["prescale"] << ")" << endl;
      }
      cout << "and groups: ";
      for( auto & g : ch.groups() ) {
         cout << " " << g;
      }
      cout << endl;
      cout << "for comparison generic access to groups: ";
      for( auto & g : ch.getList("groups") ) {
         cout << " " << g["name"];
      }
      cout << endl;
      if(--np==0) break;
   }
  
   // LVL1
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
   cout << endl << "L1 menu has " << l1menu.thresholds().size() << " thresholds, going to print only the first." << endl;
   ni = 1; // print only first 1
   for(const auto & thr : l1menu.thresholds() ) {
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
  
   // L1Topo
   cout << endl
        << "==============================" << endl
        << "=====                    =====" << endl
        << "===== Test L1Topo access =====" << endl
        << "=====                    =====" << endl
        << "==============================" << endl << endl;

   TrigConf::L1TopoMenu l1topomenu;
   fileLoader.loadFile( l1topo_filename, l1topomenu);
   cout << "Loaded the L1Topo menu " << l1topomenu.name() << endl;

   // output lines
   cout << endl << "L1Topo menu has " << l1topomenu.size() << " outputs, going to print the first 5." << endl;
   ni = 5;
   for(auto & output : l1topomenu) {
      cout << "  " << output.algName() << "[" << output.algId() << "] on board " << output.board() << ", fpga " << output.fpga() << " and clock " << output.clock()
           << " feeds lines " << output.triggerLines() << ", starting at bit " << output.firstBit() << endl; 
      if(--ni==0) break;
   }   

   // sorting algorithms
   const auto & sortAlgs = l1topomenu.getList("sortingAlgorithms"); 
   cout << endl << "L1Topo menu has " << sortAlgs.size() << " sorting algorithms. Going to print first 2:" << endl;
   ni=2;
   for(auto & alg : sortAlgs ) {
      cout << endl << "Sorting algorithm " << alg["name"] << "[" << alg["algId"] << "] uses inputs" << endl;
      for( const auto &x : alg.getList("fixedParameters.inputs") ) {
         cout << "   " << x["name"] << " " << x["value"] << endl;
      }
      cout << "and produces output ";
      const auto & x = alg.getObject("fixedParameters.outputs");
      cout << alg["fixedParameters.outputs.name"] << " " << x["value"] << endl; // two ways to access name and value
      if(--ni==0) break;
   }

   // decision algorithm
   const auto & decAlgs = l1topomenu.getList("decisionAlgorithms"); 
   cout << endl << "L1Topo menu has " << decAlgs.size() << " decision algorithms. Going to print first" << endl;
   ni=1;
   for(auto & alg : decAlgs ) {
      cout << endl << "Decision algorithm " << alg["name"] << "[" << alg["algId"] << "] uses inputs" << endl;
      for( const auto &x : alg.getList("fixedParameters.inputs") ) {
         cout << "   " << x["name"] << " at " << x["position"] << ": " << x["value"] << endl;
      }
      const auto & o = alg.getObject("fixedParameters.outputs");
      cout << "and produces " << o["bits"] << " outputs:" << endl;
      for(const auto & b : o.getList("Bit") ) {
         cout << "  " << b["selection"] << " " << b["name"] << endl;
      }
      if(--ni==0) break;
   }

   return 0;
}
