/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <sys/stat.h>
#include <iomanip>

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1Threshold.h"

#include <boost/algorithm/string.hpp>

using namespace std;

using std::cout;
using std::endl;

void section(const std::string & sec) {
   cout << "************************************************************" << endl;
   cout << "**                                                        **" << endl;
   cout << "** " << sec << setw(58-sec.size()) << "**" << endl;
   cout << "**                                                        **" << endl;
   cout << "************************************************************" << endl;

}

void
exampleL1Calo(const string & filename) {
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
      auto iso_loose  = ei_eEM.isolation(TrigConf::Selection::WP::LOOSE, ieta);
      auto iso_medium = ei_eEM.isolation(TrigConf::Selection::WP::MEDIUM, ieta);
      auto iso_tight  = ei_eEM.isolation(TrigConf::Selection::WP::TIGHT, ieta);
      int reta_loose_fw = iso_loose.reta_fw(); 
      int rhad_loose_fw = iso_loose.rhad_fw(); 
      int wstot_loose_fw = iso_loose.wstot_fw(); 
      int reta_loose_d = iso_loose.reta_d();
      int rhad_loose_d = iso_loose.rhad_d();
      int wstot_loose_d = iso_loose.wstot_d();
      cout << "ieta=" << ieta << "  loose => reta_fw=" << reta_loose_fw << ", rhad_fw=" << rhad_loose_fw << ", wstot_fw=" << wstot_loose_fw << endl;
      cout << "ieta=" << ieta << "  loose => reta_d=" << reta_loose_d << ", rhad_d=" << rhad_loose_d << ", wstot_d=" << wstot_loose_d << endl;
      int reta_medium_fw = iso_medium.reta_fw();
      int rhad_medium_fw = iso_medium.rhad_fw();
      int wstot_medium_fw = iso_medium.wstot_fw();
      int reta_medium_d = iso_medium.reta_d(); 
      int rhad_medium_d = iso_medium.rhad_d(); 
      int wstot_medium_d = iso_medium.wstot_d(); 
      cout << "ieta=" << ieta << "  medium => reta_fw=" << reta_medium_fw << ", rhad_fw=" << rhad_medium_fw << ", wstot_fw=" << wstot_medium_fw << endl;
      cout << "ieta=" << ieta << "  medium => reta_d=" << reta_medium_d << ", rhad_d=" << rhad_medium_d << ", wstot_d=" << wstot_medium_d << endl;
      int reta_tight_fw = iso_tight.reta_fw(); 
      int rhad_tight_fw = iso_tight.rhad_fw(); 
      int wstot_tight_fw = iso_tight.wstot_fw();
      int reta_tight_d = iso_tight.reta_d();
      int rhad_tight_d = iso_tight.rhad_d();
      int wstot_tight_d = iso_tight.wstot_d(); 
      cout << "ieta=" << ieta << "  tight => reta_fw=" << reta_tight_fw << ", rhad_fw=" << rhad_tight_fw << ", wstot_fw=" << wstot_tight_fw << endl;
      cout << "ieta=" << ieta << "  tight => reta_d=" << reta_tight_d << ", rhad_d=" << rhad_tight_d << ", wstot_d=" << wstot_tight_d << endl;
   }
}


/*****************************************************
 *
 *  Items
 *
 *****************************************************/
bool
testL1Menu_Items(const TrigConf::L1Menu & l1menu) {
   section("Items");
   cout << endl << "L1 menu has " << l1menu.size() << " items, going to print the first:" << endl;
   int ni = 3; // print only first 1
   for(const auto & item : l1menu ) {
      cout << "   " << item.name() << "(ctpId " << item.ctpId() << ") has definition '" << item.definition() << "' and triggerType " << item.triggerType()<< endl;
      cout << "Full printout:" << endl;
      item.print();
      if(--ni==0) break;
   }
   return true;
}


/*****************************************************
 *
 *  Boards
 *
 *****************************************************/
bool
testL1Menu_Boards(const TrigConf::L1Menu & l1menu) {
   section("Boards");
   cout << "L1 menu has " << l1menu.boardNames().size() << " boards configured" << endl;
   string boardName("Topo1");
   auto & board = l1menu.board(boardName);
   cout << "Board " << boardName << " of type " << board.type() << " has " << board.size() << " connectors configured: ";
   for( auto & connName : board.connectorNames() ) { cout << connName << " "; }
   cout << endl;
   return true;
}


/*****************************************************
 *
 *  Connectors
 *
 *****************************************************/
bool
testL1Menu_Connectors(const TrigConf::L1Menu & l1menu) {
   section("Connnectors");
   cout << "L1 menu has " << l1menu.connectorNames().size() << " connectors configured" << endl;
   for( const string & connName : l1menu.connectorNames() ) {
      auto & conn = l1menu.connector(connName);
      if(connName == "LegacyTopoMerged") continue;
      cout << "Connector " << connName << (conn.legacy() ? " (legacy)": "") << " has " << conn.size() << " trigger lines configured:" << endl;
      if( connName == "MuCTPiOpt0" ) {
         for( auto & tl : conn.triggerLines() ) {
            cout << "   Triggerline " << tl.name() << " bits=["  << tl.startbit() << ".." << tl.endbit() << "] is a muon threshold " << endl;            
         }
      } else if( connName == "AlfaCtpin" ) {
         for( size_t clock : { 0, 1 } ) {
            for( auto & tl : conn.triggerLines(0, clock) ) {
               cout << "   Triggerline " << tl.name() << " (clock " << clock << ", bit "  << tl.startbit() << ") is an ALFA threshold " << endl;
            }
         }
      } else if( conn.connectorType() == TrigConf::L1Connector::ConnectorType::CTPIN ) {
         for( auto & tl : conn.triggerLines() ) {
            cout << "   Triggerline " << tl.name() << " bits=["  << tl.startbit() << ".." << tl.endbit() << "] is a legacy threshold " << endl;            
         }
      } else if( conn.connectorType() == TrigConf::L1Connector::ConnectorType::OPTICAL ) {
         for( auto & tl : conn.triggerLines() ) {
            const string & tlName = tl.name();
            auto & topoAlg = l1menu.algorithmFromTriggerline(tlName);
            cout << "   Triggerline " << tlName << " bits=["  << tl.startbit() << ".." << tl.endbit() 
                 << "] is produced by topo algorithm " << topoAlg.name() << endl;
         }
      } else if( conn.connectorType() == TrigConf::L1Connector::ConnectorType::ELECTRICAL ) {
         for( size_t fpga : { 0, 1 } ) {
            for( size_t clock : { 0, 1 } ) {
               for( auto & tl : conn.triggerLines(fpga, clock) ) {
                  const string & tlName = tl.name();
                  auto & topoAlg = l1menu.algorithmFromTriggerline(tlName);
                  cout << "   Triggerline " << tlName << " bits=["  << tl.startbit() << ".." << tl.endbit() 
                       << "] is produced by topo algorithm " << topoAlg.name() << endl;
               }
            }
         }
      }
   }
   return true;
}


/*****************************************************
 *
 *  Topo algorithms
 *
 *****************************************************/
bool
testL1Menu_Topo(const TrigConf::L1Menu & l1menu, bool printdetail) 
{
   section("Topo algorithms");
   {
      cout << "Number of topo algorithms: " << l1menu.topoAlgorithmNames("TOPO").size() << endl;
      cout << "Number of topo algorithm outputs: " << l1menu.topoAlgorithmOutputNames("TOPO").size() << endl;

      auto tl = l1menu.connector("LegacyTopo1").triggerLines(0,0)[0];
      auto & topoAlg = l1menu.algorithmFromTriggerline(tl.name());
      topoAlg.print();

      auto & topoAlgInvmassLeg = l1menu.algorithmFromTriggerline("R2TOPO_900INVM9999-AJ30s6-AJ20s6");
      topoAlgInvmassLeg.print();
      cout << "Explicit access to 'NumResultBits' as unsigned int: " << topoAlgInvmassLeg.genericParameter<unsigned int>("NumResultBits") << endl;
      
      auto & topoAlgEMJ = l1menu.algorithmFromOutput("0DR03-eEM7ab-CjJ15ab", "TOPO");
      topoAlgEMJ.print();

      auto & topoMultAlg = l1menu.algorithm("Mult_eEM22M","MULTTOPO");
      topoMultAlg.print();
      cout << "  threshold definition: " << topoMultAlg.getAttribute("threshold") << endl;

      if(printdetail) {
         for( auto & algName : l1menu.topoAlgorithmNames("TOPO") ) {
            auto & alg = l1menu.algorithm( algName, "TOPO" );
            cout << algName << " has inputs ";
            for( auto & inp : alg.inputs() ) {
               cout << inp;
            }
            cout << endl;
         }
      }
   }
   return true;
}


/*****************************************************
 *
 *  Thresholds
 *
 *****************************************************/
bool
testL1Menu_Thresholds(const TrigConf::L1Menu & l1menu, bool printdetail)
{
   section("Thresholds");

   cout << "L1 menu has " << l1menu.thresholdTypes().size() << " threshold types configured: " << endl << "   ";
   for( const string & tt : l1menu.thresholdTypes()) { cout << tt << " "; } cout << endl;
   for( const string & tt : l1menu.thresholdTypes()) {
      cout << "L1 menu has " << l1menu.thresholds(tt).size() << " thresholds of type " << tt;
      if(printdetail) {
         cout << " thresholds, going to print the first three." << endl;
         int ni = 3; // print the first 3
         for(auto thr : l1menu.thresholds(tt) ) {
            cout << "   " << thr->name() << " of type " << thr->type() << " (mapping " << thr->mapping() << ") " << endl;
            if(--ni==0) break;
         }
      } else {
         cout << endl;
      }
      if(tt != "internal") {
         for(auto thr : l1menu.thresholds(tt) ) {
            if(thr->name().find("MULT-CMU") != std::string::npos){ continue; } // tmp hack!
            const std::string & connName = l1menu.connectorNameFromThreshold(thr->name());
            if(! l1menu.connector(connName).hasLine(thr->name())) {
               throw std::runtime_error("Threshold " + thr->name() + " does not exist as triggerline of connector " + connName);
            }
         }
      }
   }
   
   const auto & thrEM = dynamic_cast<const TrigConf::L1Threshold_EM&>(l1menu.threshold("EM22VHI"));
   thrEM.print();

   const auto & thrTAU = dynamic_cast<const TrigConf::L1Threshold_TAU&>(l1menu.threshold("HA12IM"));
   if(thrTAU) {
      cout << thrTAU.name() << ":" << endl;
      for(int eta  : {0, 10, 20, -10,-20}) {
         cout << "   value at eta = " << eta << ": " << thrTAU.thrValue(eta) << " GeV, "<< thrTAU.thrValueCounts(eta) <<" counts, iso " << thrTAU.isolationMask() << endl;
      }
   }

   cout << "XE30 cut: " << l1menu.threshold("XE30").thrValue() << endl;
   if(printdetail) {
      for ( const string & thrName : l1menu.thresholdNames() ) {
         cout << thrName << " threshold value: " << l1menu.threshold(thrName).thrValue() << endl;
      }
   }

   auto thrJET = dynamic_pointer_cast<TrigConf::L1Threshold_JET>(l1menu.thresholds("JET")[0]);
   if(thrJET) {
      cout << thrJET->name() << ":" << endl;
      for(int eta  : {0, 20, 30, 40}) {
         cout << "   value at eta = " << eta << ": " << thrJET->thrValue(eta) << " GeV, "
              << thrJET->thrValueMeV(eta) << " MeV, " << thrJET->thrValueCounts(eta) << " counts" << endl;
      }
   }

   auto thrXE = dynamic_pointer_cast<TrigConf::L1Threshold_XE>(l1menu.thresholds("XE")[0]);
   if(thrXE) {
      cout << thrXE->name() << ": value " << thrXE->thrValue() << " GeV, " << thrXE->thrValueMeV() << " MeV, " << thrXE->thrValueCounts() << " counts" << endl;
   }

//   auto thrTERR = dynamic_cast<const TrigConf::L1Threshold_TE&>(l1menu.threshold("TE5.0ETA24"));
//   cout << thrTERR.name() << ":" << endl;
//   for(int eta  : {0, 20, 30, 40}) {
//      cout << "   value at eta = " << eta << ": " << thrTERR.thrValue(eta) << " GeV, " << thrTERR.thrValueMeV(eta) << " MeV, " << thrTERR.thrValueCounts(eta) << " counts" << endl;
//   }


   const auto & threEM = dynamic_cast<const TrigConf::L1Threshold_eEM&>(l1menu.threshold("eEM15M"));
   cout << "eEM15M isolation: rhad = " << (int)threEM.rhad() << ", reta = " << (int)threEM.reta() << ", wstot = " << (int)threEM.wstot() << endl;

   const auto & threTAU = dynamic_cast<const TrigConf::L1Threshold_eTAU&>(l1menu.threshold("eTAU12"));
   if(threTAU) {
      cout << threTAU.name() << ":" << endl;
      for(int eta  : {0, 10, 20, -10,-20}) {
         cout << "   value at eta = " << eta << ": " << threTAU.thrValue(eta) << " GeV, " << threTAU.thrValueCounts(eta) << " counts, iso " << (int)threTAU.rCore() << endl;
      }
   }

   const auto & thrjEM = dynamic_cast<const TrigConf::L1Threshold_jEM&>(l1menu.threshold("jEM15M"));
   cout << "jEM15M isolation: iso = " << (int)thrjEM.iso() << ", frac = " << (int)thrjEM.frac() << ", frac2 = " << (int)thrjEM.frac2() << endl;

   const auto & thrjJET = dynamic_cast<const TrigConf::L1Threshold_jJ&>(l1menu.threshold("jJ12p0ETA25"));
   if(thrjJET) {
      cout << thrjJET.name() << ":" << endl;
      for(int eta  : {0, 20, 30, 40}) {
         cout << "   value at eta = " << eta << ": " << thrjJET.thrValue(eta) << " GeV, "
              << thrjJET.thrValueMeV(eta) << " MeV, " << thrjJET.thrValueCounts(eta) << " counts" << endl;
      }
   }


   const auto & thrMU8VF = dynamic_cast<const TrigConf::L1Threshold_MU&>(l1menu.threshold("MU8VF"));
   cout << "Threshold MU8VF with "
        << "barrel pt=" << thrMU8VF.ptBarrel() << " (idx " << thrMU8VF.idxBarrel() << "), "
        << "endcap pt=" << thrMU8VF.ptEndcap() << " (idx " << thrMU8VF.idxEndcap() << "), and "
        << "forward pt=" << thrMU8VF.ptForward() << " (idx " << thrMU8VF.idxForward() << ")" << endl;
   return true;
}


/*****************************************************
 *
 *  extra information for certain threshold types
 *
 *****************************************************/
bool
testL1Menu_Extrainfo(const TrigConf::L1Menu & l1menu)
{
   section("Extra information for threholds");
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
      cout << "    energy resolution (MeV) " << exEM.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << exEM.ptMinToTopo() << endl;
      for( const std::string tt : {"HAIsoForEMthr", "EMIsoForEMthr"} ) {
         cout << "    Isolation " << tt << endl;
         for(size_t bit = 1; bit <= 5; bit++) {
            auto & iso = exEM.isolation(tt, bit);
            cout << "      " << iso << endl;
         }
      }
   }
   {
      auto & ex = l1menu.thrExtraInfo().JET();
      cout << "  JET" << endl;
      cout << "    jetScale " << ex.jetScale() << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopoSmallWindow " << ex.ptMinToTopoSmallWindow() << endl;
      cout << "    ptMinToTopoLargeWindow " << ex.ptMinToTopoLargeWindow() << endl;
      cout << "    ptMinToTopoSmallWindow (MeV) " << ex.ptMinToTopoSmallWindowMeV() << endl;
      cout << "    ptMinToTopoLargeWindow (MeV) " << ex.ptMinToTopoLargeWindowMeV() << endl;
      cout << "    ptMinToTopoSmallWindow (counts) " << ex.ptMinToTopoSmallWindowCounts() << endl;
      cout << "    ptMinToTopoLargeWindow (counts) " << ex.ptMinToTopoLargeWindowCounts() << endl;
   }
   {
      auto & ex = l1menu.thrExtraInfo().TAU();
      cout << "  TAU" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo() << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV() << endl;
      cout << "    ptMinToTopo (counts)" << ex.ptMinToTopoCounts() << endl;
      cout << "    Isolation EMIsoForTAUthr" << endl;
      for(size_t bit = 1; bit <= 5; bit++) {
         cout << "      " << ex.isolation("EMIsoForTAUthr", bit) << endl;
      }
   }
   {
      auto & ex = l1menu.thrExtraInfo().eEM();
      cout << "  eEM" << endl;
      cout << "    iso maxEt (GeV) " << ex.maxEt() << endl;
      cout << "    iso maxEt (MeV) " << ex.maxEtMeV() << endl;
      cout << "    iso maxEt (Counts) " << ex.maxEtCounts(ex.resolutionMeV()) << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo() << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV() << endl;
      cout << "    ptMinToTopo (counts)" << ex.ptMinToTopoCounts() << endl;
      cout << "    working point Loose" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::LOOSE)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Medium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::MEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Tight" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::TIGHT)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      //cout << "    working point Medium at eta = -20:" << ex.isolation(TrigConf::Selection::WP::MEDIUM,-20) << endl;
      cout << "    working point Medium at eta = 20:" << ex.isolation(TrigConf::Selection::WP::LOOSE,20) << endl;

      for( int ieta : { -30, -20, -10, 0, 10, 20, 30 } ) {
         auto iso_loose  = ex.isolation(TrigConf::Selection::WP::LOOSE, ieta);
         int reta_loose_fw = iso_loose.reta_fw();
         int rhad_loose_fw = iso_loose.rhad_fw();
         int wstot_loose_fw = iso_loose.wstot_fw();
         int reta_loose_d = iso_loose.reta_d();
         int rhad_loose_d = iso_loose.rhad_d();
         int wstot_loose_d = iso_loose.wstot_d();
         cout << "ieta=" << ieta << "  loose => reta_fw=" << reta_loose_fw << ", rhad_fw=" << rhad_loose_fw << ", wstot_fw=" << wstot_loose_fw << endl;
         cout << "ieta=" << ieta << "  loose => reta_d=" << reta_loose_d << ", rhad_d=" << rhad_loose_d << ", wstot_d=" << wstot_loose_d << endl;
      }
     
   }
   {
      auto & ex = l1menu.thrExtraInfo().jEM();
      cout << "  jEM" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo("1A") << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV("1A") << endl;
      cout << "    ptMinToTopo (counts)" << ex.ptMinToTopoCounts("1A") << endl;
      cout << "    ptMinxTOB " << ex.ptMinxTOB("1A") << endl;
      cout << "    ptMinxTOB (MeV) " << ex.ptMinxTOBMeV("1A") << endl;
      cout << "    ptMinxTOB (counts) " << ex.ptMinxTOBCounts("1A") << endl;
      cout << "    working point Loose" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::LOOSE)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Medium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::MEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Tight" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::TIGHT)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      //cout << "    working point Medium at eta = -20:" << ex.isolation(TrigConf::Selection::WP::MEDIUM,-20) << endl;
      cout << "    working point Medium at eta = 20:" << ex.isolation(TrigConf::Selection::WP::LOOSE,20) << endl;
   }
   {
      auto & ex = l1menu.thrExtraInfo().jJ();
      cout << "  jJ" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo("1A") << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV("1A") << endl;
      cout << "    ptMinToTopo (counts) " << ex.ptMinToTopoCounts("1A") << endl;
      cout << "    ptMinxTOB " << ex.ptMinxTOB("1A") << endl;
      cout << "    ptMinxTOB (MeV) " << ex.ptMinxTOBMeV("1A") << endl;
      cout << "    ptMinxTOB (counts) " << ex.ptMinxTOBCounts("1A") << endl;
   }
   {
      auto & ex = l1menu.thrExtraInfo().jLJ();
      cout << "  jLJ" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo("1A") << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV("1A") << endl;
      cout << "    ptMinToTopo (counts) " << ex.ptMinToTopoCounts("1A") << endl;
      cout << "    ptMinxTOB " << ex.ptMinxTOB("1A") << endl;
      cout << "    ptMinxTOB (MeV) " << ex.ptMinxTOBMeV("1A") << endl;
      cout << "    ptMinxTOB (counts) " << ex.ptMinxTOBCounts("1A") << endl;
   }
   {  
      auto & ex = l1menu.thrExtraInfo().gLJ();
      cout << "  gLJ" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo (eta range "<<std::to_string(1)<<") in GeV " << ex.ptMinToTopo(1) << endl; 
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV(1) << endl; 
      cout << "    ptMinToTopo (counts) " << ex.ptMinToTopoCounts(1) << endl;
      cout << "    seedThr(A) " << ex.seedThr('A') << endl; 
      cout << "    seedThr(A) (MeV) " << ex.seedThrMeV('A') << endl; 
      cout << "    rhoTowerMin(B) " << ex.rhoTowerMin('B') << endl;   
      cout << "    rhoTowerMin(B) (MeV) " << ex.rhoTowerMinMeV('B') << endl;  
      cout << "    rhoTowerMax(C) " << ex.rhoTowerMax('C') << endl;
      cout << "    rhoTowerMax(C) (MeV) " << ex.rhoTowerMaxMeV('C') << endl;
   }
   {
      auto & ex = l1menu.thrExtraInfo().gXE();
      cout << "  gXE" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    seedThr(A) " << ex.seedThr('A') << endl;
      cout << "    seedThr(A) (MeV) " << ex.seedThrMeV('A') << endl;
      cout << "    XERHO_sigmaPosA " << ex.XERHO_param('A',true) << endl;
      cout << "    XERHO_sigmaNegB " << ex.XERHO_param('B',false) << endl;
      cout << "    XEJWOJ_a_C " << ex.JWOJ_param('C','a') << endl;
      cout << "    XEJWOJ_b_B " << ex.JWOJ_param('B','b') << endl;
      cout << "    XEJWOJ_c_A " << ex.JWOJ_param('A','c') << endl;
   }
   {
      auto & ex = l1menu.thrExtraInfo().eTAU();
      cout << "  eTAU" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo() << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV() << endl;
      cout << "    ptMinToTopo (counts)" << ex.ptMinToTopoCounts() << endl;
      cout << "    working point Loose" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::LOOSE)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Medium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::MEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Tight" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::TIGHT)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax() 
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point HadMedium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::HADMEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
   }
   {
      auto & ex = l1menu.thrExtraInfo().jTAU();
      cout << "  jTAU" << endl;
      cout << "    energy resolution (MeV) " << ex.resolutionMeV() << endl;
      cout << "    ptMinToTopo " << ex.ptMinToTopo("1A") << endl;
      cout << "    ptMinToTopo (MeV) " << ex.ptMinToTopoMeV("1A") << endl;
      cout << "    ptMinToTopo (counts)" << ex.ptMinToTopoCounts("1A") << endl;
      cout << "    ptMinxTOB " << ex.ptMinxTOB("1A") << endl;
      cout << "    ptMinxTOB (MeV) " << ex.ptMinxTOBMeV("1A") << endl;
      cout << "    ptMinxTOB (counts) " << ex.ptMinxTOBCounts("1A") << endl;
      cout << "    working point Loose" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::LOOSE)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Medium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::MEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Tight" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::TIGHT)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
   }
   {
      auto & ex = l1menu.thrExtraInfo().cTAU();
      cout << "  cTAU" << endl;
      cout << "    working point Loose" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::LOOSE)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Medium" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::MEDIUM)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
      cout << "    working point Tight" << endl;
      for(auto & iso : ex.isolation(TrigConf::Selection::WP::TIGHT)) {
         cout << "      range etaMin=" << iso.etaMin() << ", etaMax=" << iso.etaMax()
              << ", priority=" << iso.priority() << ", symmetric=" << (iso.symmetric() ? "yes" : "no")
              << ", isolation=" << iso.value() << endl;
      }
   }

   auto & exMU = l1menu.thrExtraInfo().MU();
   cout << "  MU" << endl;
   cout << "    known pt values for rpc ";
   for(auto pt : exMU.knownRpcPtValues()) cout << pt << " ";
   cout << endl << "    known pt values for tgc ";
   for(auto pt : exMU.knownTgcPtValues()) cout << pt << " ";
   cout << endl;
   cout << " RPC pt value for index 2: "<< exMU.ptForRpcIdx(2) << endl;
   cout << " TGC pt value for index 2: "<< exMU.ptForTgcIdx(2) << endl;
   cout << " TGC index for RPC index 2: "<< exMU.tgcIdxForRpcIdx(2) << endl;
   if( const auto & list = exMU.exclusionListNames(); std::find(list.begin(), list.end(), "rpcFeet")!=list.end() ) {
      cout << "    exclusionList 'rpcFeet'" << endl;
      for(auto & x : exMU.exclusionList("rpcFeet")) {
         cout << "     sector " << x.first << ": ";
         for( auto roi : x.second ) cout << roi << " ";
         cout << endl;
      }
   }
   return true;
}


bool
testL1Menu(const TrigConf::L1Menu & l1menu, bool printdetail = false)
{
   cout << endl
        << "==========================" << endl
        << "=====                =====" << endl
        << "===== Test L1 access =====" << endl
        << "=====                =====" << endl
        << "==========================" << endl << endl;

   cout << "Printing detail " << (printdetail ? "yes" : "no") << endl;
   section("Menu loading");
   cout << "Loaded the L1 menu " << l1menu.name() << endl;

   bool result = true;
   result &= testL1Menu_Items(l1menu);
   result &= testL1Menu_Boards(l1menu);
   result &= testL1Menu_Connectors(l1menu);
   result &= testL1Menu_Topo(l1menu, printdetail);
   result &= testL1Menu_Thresholds(l1menu, printdetail);
   result &= testL1Menu_Extrainfo(l1menu);
   return result;
}



bool testHLTMenu(const TrigConf::HLTMenu & hltmenu) {

   cout << "===========================" << endl
        << "=====                 =====" << endl
        << "===== Test HLT access =====" << endl
        << "=====                 =====" << endl
        << "===========================" << endl << endl;

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
      cout << "streams:" << endl << "  ";
      printComma = false;
      for( auto & s : ch.streams() ) {
         if (printComma)
            cout << ", ";
         cout << s;
         printComma = true;
      }
      cout << endl;
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
   vector<TrigConf::DataStructure> streams = hltmenu.streams();
   cout << "Menu has " << streams.size() << " streams, going to print the first 3." << endl;
   np = 3;
   for( auto & s : streams ) {
      cout << "  " << s["type"] << "_" << s["name"]
           << (s["obeyLB"]=="true" ? " (obeys LB" : " (does not obey LB") << ")"
           << (s["forceFullEventBuilding"]=="true" ? " (forces FullEventBuilding" : " (does not force FullEventBuilding") << ")"
           << endl;
      if(--np==0) break;
   }
   return true;
}




/**
   Main function just to get the filename and which type
 */

void usage() {

  cout << "The program needs to be run with the following specifications:\n\n";
  cout << "TestTriggerMenuAccess <options>\n";
  cout << "\n";
  cout << "[Input options]\n";
  cout << "  -f|--file             file1        ... input json file to test\n";
  cout << "  --smk                 smk          ... smk \n";
  cout << "  --db                  dbalias      ... dbalias (default TRIGGERDBDEV1) \n";
  cout << "[Other options]\n";
  cout << "  -h|--help                                           ... this help\n";
  cout << "\n";
  cout << "If no input is specified, the default LS2_v1 menu file will be taken from the release\n\n";
}

int main(int argc, char** argv) {
   bool help { false };
   string filename{""};
   unsigned int smk{0};
   std::string  dbalias {"TRIGGERDBDEV1"};
   std::vector<std::string> knownParameters { "file", "f", "smk", "db", "help", "h" };

   std::string currentParameter("");
   std::string listofUnknownParameters = "";
   std::string listofUnknownArguments = "";
   for(int i=1; i<argc; i++) {

      std::string currentWord(argv[i]);
      bool isParam = currentWord[0]=='-'; // string starts with a '-', so it is a parameter name

      // get the parameter name
      int firstChar = currentWord.find_first_not_of('-');
      string paramName = currentWord.substr(firstChar);

      // check if the parameter is known
      if ( isParam && std::find(knownParameters.begin(), knownParameters.end(), paramName) == knownParameters.end() ) {
         listofUnknownParameters += " " + currentWord;
         continue;
      }

      if(isParam) {
         currentParameter = "";
         // check the boolean parameters
         if(paramName == "h" || paramName == "help" ) { help = true; continue; }
         currentParameter = paramName;
         continue;
      }

      // inputs
      if(currentParameter == "file" || currentParameter == "f") {
         filename = currentWord;
         continue;
      }
      if(currentParameter == "smk") {
         smk = stoul(currentWord);
         continue;
      }
      if(currentParameter == "db") {
         dbalias = currentWord;
         continue;
      }
      listofUnknownArguments += " " + currentWord;
   }

   if ( not listofUnknownParameters.empty() ) {
      cerr << "Unknown parameter(s):" << listofUnknownParameters << endl;
      usage();
      return 1;
   }

   if ( not listofUnknownArguments.empty() ) {
      cerr << "Unknown argument(s):" << listofUnknownArguments << endl;
      usage();
      return 1;
   }

   if( filename.empty() && smk==0 ) {
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
         cout << "No filename or smk specified and no default L1 menu file found in the release" << endl;
         usage();
         return 1;
      }
   }

   if(help) {
      usage();
      return 0;
   }

   bool success(false);
   if(smk!=0) {
      // load from db
      TrigConf::L1Menu l1menu;
      TrigConf::TrigDBMenuLoader dbLoader(dbalias);
      dbLoader.loadL1Menu( smk, l1menu);
      success = testL1Menu(l1menu);
   } else {
      // load from file
      TrigConf::JsonFileLoader fileLoader;
      string filetype = fileLoader.getFileType( filename );
      if(filetype == "l1menu") {
         TrigConf::L1Menu l1menu;
         fileLoader.loadFile( filename, l1menu);
         success = testL1Menu(l1menu);
      } else if(filetype == "hltmenu") {
         TrigConf::HLTMenu hltmenu;
         fileLoader.loadFile( filename, hltmenu);
         success = testHLTMenu(hltmenu);
      } else {
         cout << "File " << filename << " is neither an L1 or an HLT menu json file" << endl;
      }
   }

   cout << "Finished " << (success ? "successfully" : "with failures") << endl;
   return success ? 0 : 1;
}
