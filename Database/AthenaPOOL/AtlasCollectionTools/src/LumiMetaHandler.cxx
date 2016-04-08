/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "urldecode.h"
#include "LumiMetaHandler.h"

#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include <iostream>

using namespace std;

void LumiMetaHandler::process(const pool::ICollMetaHandler::CopyMode& mode,
                                    pool::ICollMetaHandler::MetaValue& value,
                                    pool::ICollMetaHandler::CollectionMetadata& out,
                              const ProvNode& prefix,
                                    MetaKey& key)
{
   std::cerr << "LumiMetaHandler: called for " <<prefix <<"/"<<key << std::endl;
   std::string outval;
   ICollMetaHandler::CollectionMetadata::const_iterator mit = out.find(key);
   if (mit!=out.end()) outval=mit->second;
   if      (mode==ICollMetaHandler::merge)   this->Merge  (value,outval);
   else if (mode==ICollMetaHandler::extract) this->Extract(value,outval);
   else if (mode==ICollMetaHandler::copy)    this->Copy   (value,outval);
   else {
      std::cerr << "ERROR, INVALID COPY MODE" << mode << std::endl;
   }
   if (mit!=out.end()) out[key] = outval;
   else out.insert(std::make_pair(key,outval));
}


void LumiMetaHandler::Merge  (MetaValue& value, MetaValue& out)
{
   Root::TGoodRunsListReader* reader =  new Root::TGoodRunsListReader();
   if (value.find('%') != std::string::npos) {
      // Try decoding it as if it is urlencoded
      std::string decoded_string;
      urldecode::decoder_state state;
      state.state = urldecode::ST_SYM;
      std::pair<std::string,int> decoded = urldecode::urldecode(&state, value, value.size());
      value = decoded.first;
      if (decoded.second < 0) {
         std::cerr << "Decoding failed" << std::endl;
         return;
      }
      else {
         value = decoded.first;
      }
   }
      
   try {
      reader->SetXMLString(out);
      bool xmlIsValid = false;
      xmlIsValid = reader->Interpret();
      if (xmlIsValid) {
         Root::TGoodRunsList grl_total = reader->GetMergedGoodRunsList();
         reader->SetXMLString(value);
         xmlIsValid = reader->Interpret();
         if (xmlIsValid) {
            Root::TGoodRunsList grl_new = reader->GetMergedGoodRunsList();
            grl_total = grl_total.GetSumWith(grl_new);   
            Root::TGoodRunsListWriter blue;
            blue.SetGoodRunsList(grl_total);
            out=blue.GetXMLString();
         }
         else {
            std::cerr << "WARNING: Unable to parse input luminosity record \n" << value << std::endl;
         }
      }
      else {
         std::cerr << "WARNING: Unable to parse output luminosity record \n" << out << std::endl;
      }
   }
   catch (...) {
      std::cerr << "GRL Exception: Unable to parse " << out <<", " << value << std::endl;
   }
}
void LumiMetaHandler::Extract(MetaValue& value, MetaValue& out)
{
   //key = prefix + "|" + key;
   this->Merge(value,out);
}

void LumiMetaHandler::Copy   (MetaValue& value, MetaValue& out)
{
   out = value;
}
