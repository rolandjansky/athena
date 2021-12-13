/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileWriterHLT.h"

#include <iomanip>
#include <fstream>
#include <algorithm>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

TrigConf::JsonFileWriterHLT::JsonFileWriterHLT() :
   TrigConfMessaging( "JsonFileWriterHLT")
{}


/// Helper function ptree key->[] to std::vector<T>
template<typename T>
std::vector<T> ToVector(const TrigConf::DataStructure& ds, const std::string& child){
   using ptree = boost::property_tree::ptree;
   std::vector<T> return_vector;
   for( const ptree::value_type& entry : ds.data().get_child(child) ) {
      return_vector.push_back( entry.second.get_value<T>() );
   }
   return return_vector;
}

/// Helper function ptree key->[[]] to std::vector<std::vector<T>>
template<typename T>
std::vector<std::vector<T>> ToVectorVector(const TrigConf::DataStructure& ds, const std::string& child){
   using ptree = boost::property_tree::ptree;
   std::vector<std::vector<T>> return_vector;
   for( const ptree::value_type& outer : ds.data().get_child(child) ) {
      return_vector.push_back(std::vector<T>());
      for (const ptree::value_type& inner : outer.second) {
         return_vector.back().push_back( inner.second.get_value<T>() );
      }
   }
   return return_vector;
}


bool
TrigConf::JsonFileWriterHLT::writeJsonFile(const std::string & filename, const HLTMenu & menu) const
{
   json chains({});
   for ( const auto & chain : menu ) {
      json jChain({});
      jChain["counter"] = chain.counter();
      jChain["nameHash"] = chain.namehash();
      jChain["l1item"] = chain.l1item();
      jChain["legMultiplicities"] = chain.legMultiplicities();
      jChain["l1thresholds"] = chain.l1thresholds();
      jChain["groups"] = chain.groups();
      jChain["streams"] = chain.streams();
      jChain["seqeuncers"] = chain.sequencers();

      // Optional Run2 payload
      if (chain.hasChild("signature")) {
         json jSig({});
         jSig["counters"] = ToVector<uint32_t>(chain, "signature.counters");
         jSig["logics"] = ToVector<int>(chain, "signature.logics");
         jSig["labels"] = ToVector<std::string>(chain, "signature.labels");
         jSig["outputTEs"] = ToVectorVector<std::string>(chain, "signature.outputTEs");
         jChain["signature"] = jSig;
      }

      chains[chain.name()] = jChain;
   }

   json sequencers({});
   for ( const auto& [seqName, algsList]: menu.sequencers() ) {
      json jSeq( algsList );
      sequencers[seqName] = jSeq;
   }
   json streams({});
   for ( const auto& stream: menu.streams() ) {
      json jStream({});
      jStream["name"] = stream["name"];
      jStream["type"] = stream["type"];
      jStream["obeyLB"] = stream.getAttribute<bool>("obeyLB");
      jStream["forceFullEventBuilding"] = stream.getAttribute<bool>("forceFullEventBuilding");
      streams[stream["name"]] = jStream;
   }


   json j({});
   j["filetype"] = "hltmenu";
   j["name"] = menu.name();
   j["chains"] = chains;
   j["sequencers"] = sequencers;
   j["streams"] = streams;

   // Optional Run2 payload
   if (menu.hasChild("sequence_run2")) {
      json jSequence({});
      jSequence["outputTEs"] = ToVector<std::string>(menu, "sequence_run2.outputTEs");
      jSequence["inputTEs"] = ToVectorVector<std::string>(menu, "sequence_run2.inputTEs");
      jSequence["algorithms"] = ToVectorVector<std::string>(menu, "sequence_run2.algorithms");
      j["sequence_run2"] = jSequence;
   }

   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;

   TRG_MSG_INFO("Saved file " << filename);
   return true;
}
bool
TrigConf::JsonFileWriterHLT::writeJsonFile(const std::string & filename, const HLTMenu & menu, const HLTPrescalesSet & ps) const
{
   json chains({});
   for ( const auto & chain : menu ) {
      json jChain({});
      jChain["name"] = chain.name();
      jChain["counter"] = chain.counter();
      jChain["hash"] = chain.namehash();
      jChain["prescale"] = ps.prescale(chain.name()).prescale;
      jChain["enabled"] = ps.prescale(chain.name()).enabled;
      chains[chain.name()] = jChain;
   }
   json j({});
   j["filetype"] = "hltprescale";
   j["name"] = ps.name();
   j["prescales"] = chains;
   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;

   TRG_MSG_INFO("Saved file " << filename);
   return true;
}
