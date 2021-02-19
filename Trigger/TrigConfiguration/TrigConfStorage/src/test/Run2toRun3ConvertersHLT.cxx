/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <numeric>
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/DataStructure.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/json_parser.hpp>
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigConfIO/JsonFileWriterHLT.h"

template<typename COLL>
boost::property_tree::ptree asArray( const COLL& data) {
   using ptree = boost::property_tree::ptree;
   ptree array;
   for ( const auto& el: data ) {
      ptree one;
      one.put("", el);
      array.push_back(std::make_pair("", one));
   }
   return array;
}

std::vector<int> legMult(const TrigConf::HLTChain* cptr) {
   std::vector<int> maxMult;
   for ( const auto sig: cptr->signatures() ) {
      std::vector<std::string> names;
      std::vector<int> mult;
      for ( const auto te: sig->outputTEs() ) {
         auto found = std::find(names.begin(), names.end(), te->name());
         if ( found == names.end() ) {
            names.push_back(te->name());
            mult.push_back(1);
         } else {
            int index = std::find(names.begin(), names.end(), te->name()) - names.begin();
            mult[index]++;
         }
      }
      if ( maxMult.size() < mult.size() ) {
         maxMult = mult;
      }
   }
   return maxMult;
}

std::vector<std::string> l1thresholds(const TrigConf::HLTFrame* frame, const TrigConf::HLTChain* cptr) {
   std::set<std::string> names;
   for ( const auto sig: cptr->signatures() ) {
      for ( const auto te: sig->outputTEs() ) {
         auto sequence = frame->sequences().getSequence(te->name());
         for ( const auto inTE: sequence->inputTEs() ) {
            if ( not ( inTE->name().find("L2_") == 0 or inTE->name().find("EF_") == 0 or inTE->name().find("HLT_") == 0 ) ) {
               names.insert(inTE->name());
            }
         }
      }
   }
   return std::vector<std::string>( names.begin(), names.end() );
}

void printJSON(boost::property_tree::ptree& p) {
   // this code is useful to debug JSON before it is pushed to menu objects expecting certain structure in place
   std::stringstream ss;
   boost::property_tree::json_parser::write_json(ss, p);
   std::cout << ss.str() << std::endl;
}

bool convertHLTMenu(const TrigConf::HLTFrame* frame, TrigConf::HLTMenu& menu) {
   using ptree = boost::property_tree::ptree;
   ptree top;
   top.put("filetype", "hltmenu");
   top.put("name", frame->name());
   ptree pChains;

   std::map<std::string, const TrigConf::HLTStreamTag*> allStreams;

   for ( auto cptr : frame->chains() ) {
      ptree pChain;
      pChain.put("counter", cptr->chain_counter());
      pChain.put("nameHash", cptr->chain_hash_id());
      pChain.put("l1item", cptr->lower_chain_name());
      pChain.add_child("l1thresholds", asArray(l1thresholds(frame, cptr)));
      pChain.add_child("legMultiplicities", asArray(legMult(cptr)) );
      pChain.add_child("sequencers", asArray(std::vector<std::string>({"missing"})));

      std::vector<std::string> strNames;
      for ( const auto st: cptr->streams()) {
         strNames.push_back(st->stream());
         allStreams[st->stream()] = st;
      }
      pChain.add_child("streams", asArray(strNames));

      pChain.add_child("groups", asArray(cptr->groups()));

      pChains.push_back(std::make_pair(cptr->chain_name(), pChain));
   }
   ptree pStreams;
   for ( auto [sname, stream]: allStreams ) {
      ptree pStream;
      pStream.put("name", sname);
      pStream.put("type", stream->type());
      pStream.put("obeyLB", stream->obeyLB());
      pStream.put("forceFullEventBuilding", true);  // TODO understand how to get this information from old menu
      pStreams.push_back(std::make_pair(sname, pStream));
   }

   top.add_child("chains", pChains);

   top.add_child("streams", pStreams);
   ptree pSequencers;
   pSequencers.add_child("missing", asArray(std::vector<std::string>({""})));
   top.add_child("sequencers", pSequencers);

   menu.setData(std::move(top));
   menu.setSMK(frame->smk());
   return true;
}

void convertRun2HLTMenuToRun3(const TrigConf::HLTFrame* frame, const std::string& filename) {
  TrigConf::HLTMenu menu;
  convertHLTMenu(frame, menu);

  TrigConf::JsonFileWriterHLT writer;
  writer.writeJsonFile(filename, menu);

}

void convertRun2HLTPrescalesToRun3(const TrigConf::HLTFrame* frame, const std::string& filename) {
   using ptree = boost::property_tree::ptree;
   ptree top;
   top.put("filetype", "hltprescale");
   top.put("name", frame->name());
   ptree pChains;
   for ( auto cptr : frame->chains() ) {
      ptree pChain;
      pChain.put("name", cptr->chain_name());
      pChain.put("counter", cptr->chain_counter());
      pChain.put("hash", cptr->chain_hash_id());
      pChain.put("prescale", cptr->prescale());
      pChain.put("enabled", (cptr->prescale()>0 ? true: false));

      pChains.push_back(std::make_pair(cptr->chain_name(), pChain));
   }
   top.add_child("prescales", pChains);
   TrigConf::HLTPrescalesSet psk(std::move(top));

   TrigConf::HLTMenu menu;
   convertHLTMenu(frame, menu);

   TrigConf::JsonFileWriterHLT writer;
   writer.writeJsonFile(filename, menu, psk);
}

