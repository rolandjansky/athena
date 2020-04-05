/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <sys/stat.h>

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1Threshold.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void exampleL1Calo(const string & filename) {
   
   cout << endl
        << "===========================" << endl
        << "=====                 =====" << endl
        << "===== Example L1 Calo =====" << endl
        << "=====                 =====" << endl
        << "===========================" << endl << endl;

   TrigConf::L1Menu l1menu;
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.loadFile( filename, l1menu);
   cout << "Loaded the L1 menu " << l1menu.name() << endl;

   cout << "Information to configure eFEX" << endl;
   cout << "For producing eEM objects you need the working points" << endl;

   auto & ei_eEM = l1menu.thrExtraInfo().eEM();
   for( int ieta : { -30, -20, -10, 0, 10, 20, 30 } ) {
      unsigned int ptMinToTopo = ei_eEM.ptMinToTopo(); // the minimum energy to send to topo (not eta dependent yet)
      cout << "ptmin=" << ptMinToTopo << endl;
      auto iso_loose  = ei_eEM.isolation(TrigConf::Isolation::WP::LOOSE, ieta);
      auto iso_medium = ei_eEM.isolation(TrigConf::Isolation::WP::MEDIUM, ieta);
      auto iso_tight  = ei_eEM.isolation(TrigConf::Isolation::WP::TIGHT, ieta);
      int reta_loose = iso_loose.reta(); 
      int had_loose = iso_loose.had(); 
      int wstot_loose = iso_loose.wstot(); 
      cout << "ieta=" << ieta << "  loose => reta=" << reta_loose << ", had=" << had_loose << ", wstot=" << wstot_loose << endl;
      int reta_medium = iso_medium.reta(); 
      int had_medium = iso_medium.had(); 
      int wstot_medium = iso_medium.wstot(); 
      cout << "ieta=" << ieta << "  medium => reta=" << reta_medium << ", had=" << had_medium << ", wstot=" << wstot_medium << endl;
      int reta_tight = iso_tight.reta(); 
      int had_tight = iso_tight.had(); 
      int wstot_tight = iso_tight.wstot(); 
      cout << "ieta=" << ieta << "  tight => reta=" << reta_tight << ", had=" << had_tight << ", wstot=" << wstot_tight << endl;
   }

}

bool testL1Menu(const string & filename) {

   bool printdetail { false };

   cout << endl
        << "==========================" << endl
        << "=====                =====" << endl
        << "===== Test L1 access =====" << endl
        << "=====                =====" << endl
        << "==========================" << endl << endl;

   TrigConf::L1Menu l1menu;
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.loadFile( filename, l1menu);
   cout << "Loaded the L1 menu " << l1menu.name() << endl;

   // items
   cout << endl << "L1 menu has " << l1menu.size() << " items, going to print the first:" << endl;
   int ni = 3; // print only first 1
   for(const auto & item : l1menu ) {
      cout << "   " << item.name() << "(ctpId " << item.ctpId() << ") has definition '" << item.definition() << "' and triggerType " << item.triggerType()<< endl;
      cout << "Full printout:" << endl;
      item.print();
      if(--ni==0) break;
   }

   // boards
   cout << "L1 menu has " << l1menu.boardNames().size() << " boards configured" << endl;
   string boardName("Topo1");
   auto & board = l1menu.board(boardName);
   cout << "Board " << boardName << " has " << board.size() << " connectors configured: ";
   for( auto & connName : board.connectorNames() ) { cout << connName << " "; }
   cout << endl;
   
   // connectors
   cout << "L1 menu has " << l1menu.connectorNames().size() << " connectors configured" << endl;
   for( const string & connName : {"Topo1Opt0", "EM1"} ) {
      auto & conn = l1menu.connector(connName);
      cout << "Connector " << connName << " has " << conn.size() << " trigger lines configured:" << endl;
      for( auto & tl : conn.triggerLines() ) {
         const string & tlName = tl.name();
         if( conn.type() == TrigConf::L1Connector::ConnectorType::CTPIN ) {
            cout << "   Triggerline " << tlName << " bits=["  << tl.startbit() << ".." << tl.endbit() << "] is a legacy threshold " << endl;            
         } else {
            auto & topoAlg = l1menu.algorithmFromOutput(tlName);
            cout << "   Triggerline " << tlName << " bits=["  << tl.startbit() << ".." << tl.endbit() 
                 << "] is being produced by topo algorithm " << topoAlg.name() << endl;
         }
      }
   }

   // thresholds
   cout << "L1 menu has " << l1menu.thresholdTypes().size() << " threshold types configured: " << endl << "   ";
   for( const string & tt : l1menu.thresholdTypes()) { cout << tt << " "; } cout << endl;
   for( const string & tt : { "EM", "eEM", "JET", "jJ", "MU" } ) {
      cout << "L1 menu has " << l1menu.thresholds(tt).size() << " " << tt 
           << " thresholds, going to print the first three." << endl;
      int ni = 3; // print the first 3
      for(const auto & thr : l1menu.thresholds(tt) ) {
         cout << "   " << thr->name() << " of type " << thr->type() << " (mapping " << thr->mapping() << ") " << endl;
         if(--ni==0) break;
      }
   }
   
   // EM threshold with varying thresholds
   const auto & thrEM = dynamic_cast<const TrigConf::L1Threshold_EM&>(l1menu.threshold("EM22VHI"));
   thrEM.print();
   cout << "XE30 cut: " << l1menu.threshold("XE30").thrValue() << endl;
   if(printdetail) {
      for ( const string & thrName : l1menu.thresholdNames() ) {
         cout << thrName << " threshold value: " << l1menu.threshold(thrName).thrValue() << endl;
      }
   }
   const auto & threEM = dynamic_cast<const TrigConf::L1Threshold_eEM&>(l1menu.threshold("eEM18VHI"));
   cout << "eEM18VHI isolation: rhad = " << (int)threEM.rhad() << ", reta = " << (int)threEM.reta() << ", wstot = " << (int)threEM.wstot() << endl;

   const auto & thrMU10 = dynamic_cast<const TrigConf::L1Threshold_MU&>(l1menu.threshold("MU10"));
   cout << "Threshold MU10 with "
        << "barrel pt=" << thrMU10.ptBarrel() << " (idx " << thrMU10.idxBarrel() << "), "
        << "endcap pt=" << thrMU10.ptEndcap() << " (idx " << thrMU10.idxEndcap() << "), and "
        << "forward pt=" << thrMU10.ptForward() << " (idx " << thrMU10.idxForward() << ")" << endl;

   /*****************************************************
    *
    *  extra information for certain threshold types
    *
    *****************************************************/
   cout << "These threshold types have extra information" << endl << "   ";
   for( const string & tt : l1menu.thresholdTypes()) {
      if( l1menu.thrExtraInfo().hasInfo(tt) ) {
         cout << tt << " ";
      }
   }
   cout << endl << "going to print details of the extra threshold information" << endl;

   {
      auto & exEM = l1menu.thrExtraInfo().EM();
      cout << "  EM" << endl;
      cout << "    emScale " << exEM.emScale() << endl;
      cout << "    ptMinToTopo " << exEM.ptMinToTopo() << endl;
      for( const std::string & tt : {"HAIsoForEMthr", "EMIsoForEMthr"} ) {
         cout << "    Isolation " << tt << endl;
         for(size_t bit = 1; bit <= 5; bit++) {
            auto & iso = exEM.isolation(tt, bit);
            cout << "      " << iso << endl;
         }
      }
   }

   auto & exJET = l1menu.thrExtraInfo().JET();
   cout << "  JET" << endl;
   cout << "    jetScale " << exJET.jetScale() << endl;
   cout << "    ptMinToTopoSmallWindow " << exJET.ptMinToTopoSmallWindow() << endl;
   cout << "    ptMinToTopoLargeWindow " << exJET.ptMinToTopoLargeWindow() << endl;

   auto & exTAU = l1menu.thrExtraInfo().TAU();
   cout << "  TAU" << endl;
   cout << "    ptMinToTopo " << exTAU.ptMinToTopo() << endl;
   cout << "    Isolation EMIsoForTAUthr" << endl;
   for(size_t bit = 1; bit <= 5; bit++) {
      cout << "      " << exTAU.isolation("EMIsoForTAUthr", bit) << endl;
   }

   auto & exeEM = l1menu.thrExtraInfo().eEM();
   cout << "  eEM" << endl;
   cout << "    ptMinToTopo " << exeEM.ptMinToTopo() << endl;
   cout << "    working point Loose" << endl;
   for(auto & iso : exeEM.isolation(TrigConf::Isolation::WP::LOOSE)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }
   cout << "    working point Medium" << endl;
   for(auto & iso : exeEM.isolation(TrigConf::Isolation::WP::MEDIUM)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }
   cout << "    working point Tight" << endl;
   for(auto & iso : exeEM.isolation(TrigConf::Isolation::WP::TIGHT)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }
   cout << "    working point Medium at eta = 20:" << endl
        << exeEM.isolation(TrigConf::Isolation::WP::TIGHT,20) << endl;

   auto & exjJ = l1menu.thrExtraInfo().jJ();
   cout << "  jJ" << endl;
   cout << "    ptMinToTopoSmall at eta=0 " << exjJ.ptMinToTopoSmall(0) << endl;
   cout << "    ptMinToTopoLarge at eta=0 " << exjJ.ptMinToTopoLarge(0) << endl;

   auto & exeTAU = l1menu.thrExtraInfo().eTAU();
   cout << "  eTAU" << endl;
   cout << "    ptMinToTopo " << exeTAU.ptMinToTopo() << endl;
   cout << "    working point Loose" << endl;
   for(auto & iso : exeTAU.isolation(TrigConf::Isolation::WP::LOOSE)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }
   cout << "    working point Medium" << endl;
   for(auto & iso : exeTAU.isolation(TrigConf::Isolation::WP::MEDIUM)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }
   cout << "    working point Tight" << endl;
   for(auto & iso : exeTAU.isolation(TrigConf::Isolation::WP::TIGHT)) {
      cout << "      range etaMin=" << iso.etaMin << ", etaMax=" << iso.etaMax 
           << ", priority=" << iso.priority << ", symmetric=" << (iso.symmetric ? "yes" : "no")
           << ", isolation=" << iso.value << endl;
   }

   auto & exMU = l1menu.thrExtraInfo().MU();
   cout << "  MU" << endl;
   cout << "    known pt values for rpc ";
   for(auto pt : exMU.knownRpcPtValues()) cout << pt << " ";
   cout << endl << "    known pt values for tgc ";
   for(auto pt : exMU.knownTgcPtValues()) cout << pt << " ";
   cout << endl;
   if( const auto & list = exMU.exclusionListNames(); std::find(list.begin(), list.end(), "rpcFeet")!=list.end() ) {
      cout << "    exclusionList 'rpcFeet'" << endl;
      for(auto & x : exMU.exlusionList("rpcFeet")) {
         cout << "     sector " << x.first << ": ";
         for( auto roi : x.second ) cout << roi << " ";
         cout << endl;
      }
   }

   return true;
}



bool testHLTMenu(const string & filename) {

   cout << "===========================" << endl
        << "=====                 =====" << endl
        << "===== Test HLT access =====" << endl
        << "=====                 =====" << endl
        << "===========================" << endl << endl;

   TrigConf::HLTMenu hltmenu;
   TrigConf::JsonFileLoader fileLoader;
   fileLoader.loadFile( filename, hltmenu);
   cout << "Loaded the HLT menu " << hltmenu.name() << endl;
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
   return true;
}




/**
   Main function just to get the filename and which type
 */

int main(int argc, char** argv) {
   string filename(""); 
   if(argc==1) {
      // no filename specified, going to take the L1 menu from the release
      const char* env_AV = std::getenv("AtlasVersion");
      const char* env_xmlpath = std::getenv("XMLPATH");
      if( env_AV != nullptr and env_xmlpath != nullptr ) {
         vector<string> paths;
         string xmlpath(env_xmlpath);
         boost::algorithm::split(paths, xmlpath, boost::is_any_of(":"));
         for( const string & p : paths) {
            string testFN = p + "/TriggerMenuMT/L1Menu_LS2_v1_" + string(env_AV) + ".json";
            struct stat buffer;
            if (stat (testFN.c_str(), &buffer) == 0) {
               filename = testFN;
               break;
            }
         }
      }
      if(filename == "") {
         cout << "No filename specified and no default L1 menu file found in the release" << endl;
         cout << "Please use " << argv[0] << " <filename.json>" << endl;
         return 1;
      }
   } else if (argc==2) {
      struct stat buffer;
      if (stat (argv[1], &buffer) == 0) {
         filename = string(argv[1]);
      }
      if(filename == "") {
         cout << "Can't find file " << argv[1] << endl;
         return 1;
      }
   }

   // file loader
   TrigConf::JsonFileLoader fileLoader;
   string filetype = fileLoader.getFileType( filename );
   int result(1);
   if(filetype == "l1menu") {
      result = testL1Menu(filename);
   } else if(filetype == "hltmenu") {
      result = testHLTMenu(filename);
   } else {
      cout << "File " << filename << " is neither an L1 or an HLT menu json file" << endl;
   }

   return result;
}
