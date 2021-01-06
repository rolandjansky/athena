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
   TrigConfMessaging( "JsonFileWriter")
{}


bool
TrigConf::JsonFileWriterHLT::writeJsonFile(const std::string & filename, const HLTMenu & menu) const
{
   json chains({});
   for ( const auto & chain : menu ) {
      json jChain({});
      jChain["counter"] = chain.counter();
      jChain["nameHash"] = chain.name();
      jChain["l1item"] = chain.l1item();
      jChain["legMultiplicities"] = chain.legMultiplicities();
      jChain["l1thresholds"] = chain.l1thresholds();
      jChain["groups"] = chain.groups();
      jChain["streams"] = chain.streams();
      jChain["seqeuncers"] = chain.sequencers();
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


   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;

   TRG_MSG_INFO("Saved file " << filename);
   return true;
}
