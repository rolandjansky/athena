/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TrigConfData/L1Menu.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"
#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"

#include "RelationalAccess/ConnectionServiceException.h"

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main(int argc, char** argv) {

   std::string dbalias("TRIGGERDB");
   unsigned int smk(3000);
   unsigned int l1psk(0);
   unsigned int bgpsk(0);
   unsigned int hltpsk(0);

   std::vector<std::string> args;
   int c(0);
   if(const char* env = getenv("CLArgs")) {
      string s(env); s+=" ";
      auto pos{string::npos};
      while( (pos = s.find_first_not_of(" ")) != string::npos) {
         auto end = s.find(" ",pos);
         args.push_back(s.substr(pos,end));
         s = s.substr(end+1);
         if(++c>10) break;
      }
   } else {
      for(int i=1; i<argc; ++i) { // skip executable name
         args.push_back(argv[i]);
      }
   }

   if(args.size()>=1) {
      cout << "dbalias = " << args[0] << endl;
      dbalias = args[0];
   }
   if(args.size()>=2) {
      cout << "smk = " << args[1] << endl;
      smk = std::stoi(args[1]);
   }
   if(args.size()>=3) {
      cout << "l1psk = " << args[2] << endl;
      l1psk = std::stoi(args[2]);
   }
   if(args.size()>=4) {
      cout << "hltpsk = " << args[3] << endl;
      hltpsk = std::stoi(args[3]);
   }
   if(args.size()>=5) {
      cout << "bgpsk = " << args[4] << endl;
      bgpsk = std::stoi(args[4]);
   }
   cout << "Using dbalias " << dbalias << ", smk " << smk << ", l1psk " << l1psk << ", hltpsk " << hltpsk << ", bgpsk " << bgpsk << endl;
   cout << "To reproduce run:" << endl;
   cout << argv[0] << " " << dbalias << " " << smk << " " << l1psk << " " << hltpsk << " " << bgpsk << endl;

   TrigConf::TrigDBMenuLoader            dbloaderMenu(dbalias);
   TrigConf::TrigDBL1PrescalesSetLoader  dbloaderL1PS(dbalias);
   TrigConf::TrigDBL1BunchGroupSetLoader dbloaderBGRP(dbalias);
   TrigConf::TrigDBHLTPrescalesSetLoader dbloaderHLTPS(dbalias);

   bool onceL1Menu{true};
   bool onceL1Prescales{true};
   bool onceBunchbroups{true};
   bool onceHLTMenu{true};
   bool onceHLTPrescales{true};
   bool twiceL1Menu{true};
   bool twiceL1Prescales{true};
   bool twiceBunchbroups{true};
   bool twiceHLTMenu{true};
   bool twiceHLTPrescales{true};

   // L1 menu
   {
      TrigConf::L1Menu l1menu;
      try {
         dbloaderMenu.loadL1Menu(smk, l1menu, "L1Menu1.json");
      }
      catch(coral::ReplicaNotAvailableException & ex) {
         cout << "Cannot connect to database " << dbalias << ". This test will be ignored. (" << ex.what() << ")" << endl;
         ofstream out;
         out.open("NoDBConnection.txt");
         out << "No Trigger DB connection '" << dbalias << "'. Test will be ignored." << endl;
         out.close();
         return 0;
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 menu from " << dbalias << " and smk " << smk << ". An exception occurred: " << ex.what() << endl;
         onceL1Menu=false;
      }
      try {
         dbloaderMenu.loadL1Menu(smk, l1menu, "L1Menu2.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 menu a second time from " << dbalias << " and smk " << smk << ". An exception occurred: " << ex.what() << endl;
         twiceL1Menu=false;
      }
   }

   // L1 prescales
   {
      TrigConf::L1PrescalesSet l1ps;
      try {
         dbloaderL1PS.loadL1Prescales(l1psk, l1ps, "L1Prescales1.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 prescales from " << dbalias << " and psk " << l1psk << ". An exception occurred: " << ex.what() << endl;
         onceL1Prescales=false;
      }
      try {
         dbloaderL1PS.loadL1Prescales(l1psk, l1ps, "L1Prescales2.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 prescale a second time from " << dbalias << " and psk " << l1psk << ". An exception occurred: " << ex.what() << endl;
         twiceL1Prescales=false;
      }
   }

   // L1 bunchgroups
   {
      TrigConf::L1BunchGroupSet bgps;
      try {
         dbloaderBGRP.loadBunchGroupSet(bgpsk, bgps, "Bunchgroups1.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load bunchgroups from " << dbalias << " and psk " << bgpsk << ". An exception occurred: " << ex.what() << endl;
         onceBunchbroups=false;
      }
      try {
         dbloaderBGRP.loadBunchGroupSet(bgpsk, bgps, "Bunchgroups2.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load bunchgroups a second time from " << dbalias << " and psk " << bgpsk << ". An exception occurred: " << ex.what() << endl;
         twiceBunchbroups=false;
      }
   }

   // HLT menu
   {
      TrigConf::HLTMenu hltmenu;
      try {
         dbloaderMenu.loadHLTMenu(smk, hltmenu, "HLTMenu1.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load HLT menu from " << dbalias << " and smk " << smk << ". An exception occurred: " << ex.what() << endl;
         onceHLTMenu=false;
      }
      try {
         dbloaderMenu.loadHLTMenu(smk, hltmenu, "HLTMenu2.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load HLT menu a second time from " << dbalias << " and smk " << smk << ". An exception occurred: " << ex.what() << endl;
         twiceHLTMenu=false;
      }
   }

   // HLT prescales
   {
      TrigConf::HLTPrescalesSet hltpss;
      try {
         dbloaderHLTPS.loadHLTPrescales(hltpsk, hltpss, "HLTPrescales1.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 prescales from " << dbalias << " and psk " << hltpsk << ". An exception occurred: " << ex.what() << endl;
         onceHLTPrescales=false;
      }
      try {
         dbloaderHLTPS.loadHLTPrescales(hltpsk, hltpss, "HLTPrescales2.json");
      }
      catch(TrigConf::IOException & ex) {
         cout << "Could not load L1 prescale a second time from " << dbalias << " and psk " << hltpsk << ". An exception occurred: " << ex.what() << endl;
         twiceHLTPrescales=false;
      }
   }

   assert(
      onceL1Menu && onceL1Prescales && onceBunchbroups && onceHLTMenu && onceHLTPrescales &&
      twiceL1Menu && twiceL1Prescales && twiceBunchbroups && twiceHLTMenu && twiceHLTPrescales
   );


   return 0;
}
