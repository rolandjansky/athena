/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/TrigConfCoolHLTPayloadConverters.h"
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"

#include "CoolKernel/Record.h"
#include "CoolKernel/IFolder.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include "TrigConfL1Data/HelperFunctions.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTUtils.h"

using namespace std;
using namespace cool;
using namespace TrigConf;

Record
TrigConfCoolHLTPayloadConverters::createHltMenuPayload(cool::IFolderPtr fld, const HLTChain & ch, const string& concise ) {
   Record payload(fld->payloadSpecification());
   payload["ChainName"]      .setValue<cool::String255>(ch.chain_name());
   payload["ChainVersion"]   .setValue<cool::UInt32>(ch.chain_version());
   payload["ChainCounter"]   .setValue<cool::UInt32>(ch.chain_counter());
   payload["TriggerLevel"]   .setValue<cool::String255>(ch.level());
   payload["Prescale"]       .setValue<cool::Float>(ch.prescale());
   payload["PassThrough"]    .setValue<cool::Float>(ch.pass_through());
   payload["TriggerElements"].setValue<cool::String4k>(concise);
   if(payload["LowerChainName"].storageType()==cool::StorageType::String4k) {
      payload["LowerChainName"].setValue<cool::String4k>(ch.lower_chain_name());
   } else {
      payload["LowerChainName"].setValue<cool::String255>(ch.lower_chain_name());
   }

   // stream info
   stringstream streamCat;
   bool first(true);
   for(HLTStreamTag* stream : ch.streams()) {
      if(first) { first = false; } else { streamCat << ";"; } 
      streamCat << stream->stream() << "," << stream->type() << "," << stream->prescale();
   }
   payload["StreamInfo"].setValue<cool::String255>(streamCat.str());

   return payload;
}


Record
TrigConfCoolHLTPayloadConverters::createHltChainGroupPayload(cool::IFolderPtr fld, const HLTChain& ch) {
   Record payload(fld->payloadSpecification());
   string groups(ch.level());
   groups += ";";
   bool start(true);
   for(string group : ch.groups()) {
      if(start) { start=false; } else { groups += ","; }
      groups += group;
   }
   payload["Groups"].setValue<cool::String4k>(groups);
   payload["ChainCounter"].setValue<cool::UInt32>(ch.chain_counter());
   return payload;
}


Record
TrigConfCoolHLTPayloadConverters::createHltConfigKeysPayload(cool::IFolderPtr fld, uint masterKey,
                           uint hltPrescaleKey, const string& configSource ) {
   Record payload(fld->payloadSpecification());
   payload["MasterConfigurationKey"].setValue<cool::UInt32>(masterKey);
   payload["HltPrescaleConfigurationKey"].setValue<cool::UInt32>(hltPrescaleKey);
   payload["ConfigSource"].setValue<cool::String255>(configSource);
   return payload;
}

Record
TrigConfCoolHLTPayloadConverters::createMonConfigKeyPayload(cool::IFolderPtr fld, uint mck, string& info ) {
    Record payload(fld->payloadSpecification());
 	payload["MonConfigKey"].setValue<cool::UInt32>(mck);
    payload["Info"].setValue<cool::String4k>(info);
 	return payload;
}

Record
TrigConfCoolHLTPayloadConverters::createHltPrescalesPayload(cool::IFolderPtr fld, float prescale,
                          float passthrough, float rerunprescale) {
   Record payload(fld->payloadSpecification());
   payload["Prescale"].setValue<cool::Float>(prescale);
   payload["Passthrough"].setValue<cool::Float>(passthrough);
   payload["RerunPrescale"].setValue<cool::Float>(rerunprescale);
   return payload;
}


Record
TrigConfCoolHLTPayloadConverters::createHltPrescaleKeyPayload(cool::IFolderPtr fld, int psk) {
   Record payload(fld->payloadSpecification());
   payload["HltPrescaleKey"].setValue<cool::UInt32>(psk);
   return payload;
}


HLTChain * 
TrigConfCoolHLTPayloadConverters::createHLTChain( const coral::AttributeList & al, HLTSequenceList* sequences ) {
   TrigConf::HLTChain * ch = new TrigConf::HLTChain();

   // this information of a chain is not in COOL: 
   // int lower_chain_counter, vector<HLTTriggerType*>& trigList

   ch->set_chain_name      ( al["ChainName"].     data<cool::String255>() );
   ch->set_chain_version   ( al["ChainVersion"].  data<cool::UInt32>()   );
   ch->set_chain_counter   ( al["ChainCounter"].  data<cool::UInt32>()   );
   ch->set_level           ( al["TriggerLevel"].  data<cool::String255>() );
   if( typeid(cool::UInt32) == al["Prescale"].specification().type()) {
      ch->set_prescale        ( al["Prescale"].      data<cool::UInt32>()   );
      ch->set_pass_through    ( al["PassThrough"].   data<cool::UInt32>()   );
   } else if( typeid(cool::Float) == al["Prescale"].specification().type()) {
      ch->set_prescale        ( al["Prescale"].      data<cool::Float>()   );
      ch->set_pass_through    ( al["PassThrough"].   data<cool::Float>()   );
   }
   if(al["LowerChainName"].specification().type()==typeid(cool::String4k)) {
      ch->set_lower_chain_name( al["LowerChainName"].data<cool::String4k>() );
   } else {
      ch->set_lower_chain_name( al["LowerChainName"].data<cool::String255>() );
   }
   // read the trigger elements
   string fullTeString = al["TriggerElements"].data<cool::String4k>();

   if(fullTeString.find("|")==string::npos ) {
      // old style
      int sc(0); // signature counter
      vector<string> singleSigStrings = split(fullTeString,";");
      for(const string& sigdef : singleSigStrings ) {
         if(sigdef=="") { ++sc; continue; } // empty signature (when signaturCounter is not continuious)
         // the information available in the signature will be the signature counter and the list of TEs
         HLTSignature * signature = new HLTSignature();
         ch->signatureList().push_back(signature);
         signature->set_signature_counter(++sc);

         vector<string> teStrings = split(sigdef,",");
         for( const string& te : teStrings) {
            HLTTriggerElement * outte = new HLTTriggerElement( te );
            signature->outputTEs().push_back( outte );
            if(sequences && !sequences->hasTE(te) )
               sequences->addHLTSequence(new HLTSequence( vector<HLTTriggerElement*>(), outte, vector<string>()) );
         }
      }
   } else {

      // new style
      vector<string> singleSigStrings = split(fullTeString,"|");
      if(singleSigStrings.size()==2) {

         // the part in front of the '|'
         vector< string > splitRecursive = HLTUtils::splitGroups( singleSigStrings[0] );
         for( string outtename : splitRecursive ) {
            if(outtename.find(',')!=string::npos)
               outtename.erase(0,outtename.rfind(',')+1);

            if(sequences && !sequences->hasTE(outtename)) {
               HLTSequence* seq = HLTUtils::buildSequence( outtename );
               sequences->addHLTSequence( seq );
            }
         }

         // the part after the '|'
         int sc(0); // signature counter
         vector<string> signatureDesc = split(singleSigStrings[1],";");
         for(const string& sigdef : signatureDesc) {
            if(sigdef=="") { sc++; continue; } // empty signature (when signaturCounter is not continous)
            // the information available in the signature will be the signature counter and the list of TEs
            HLTSignature * signature = new HLTSignature();
            ch->signatureList().push_back(signature);
            signature->set_signature_counter(++sc);

            vector<string> sequenceDescriptions = HLTUtils::splitGroups(sigdef);
            for(string outtename : sequenceDescriptions) {
               
               if(outtename.find(',') != string::npos)
                  outtename.erase(0,outtename.rfind(',')+1);

               HLTTriggerElement * outte = new HLTTriggerElement( outtename );
               signature->outputTEs().push_back( outte );
               if(sequences && !sequences->hasTE(outtename)) {
                  HLTSequence* seq = HLTUtils::buildSequence(outtename);
                  sequences->addHLTSequence( seq );
               }
            }
         }
      }
   } // new style

   // read the stream tag info
   string fullStreamString = al["StreamInfo"].data<cool::String255>();
   vector<string> singleStreams = split( fullStreamString, ";" );

   for( const string& stream_def : singleStreams ) {
      HLTStreamTag * stream = new HLTStreamTag();
      vector<string> streamInfo = split(stream_def,",");
      if(streamInfo.size()==2) {
         stream->set_stream(streamInfo[0]); // old version: name,prescale
         stream->set_prescale(atoi(streamInfo[1].c_str()));
      } else if(streamInfo.size()==3) { // new version: name,type,version
         stream->set_stream(streamInfo[0]);
         stream->set_type(streamInfo[1]);
         stream->set_prescale(atoi(streamInfo[2].c_str()));
      }
      ch->addStream(stream);
   }

   return ch;
}


void
TrigConfCoolHLTPayloadConverters::addGroupsToHltChain( const coral::AttributeList & al,
                     const HLTChainList& chl)
{
   int cc = al["ChainCounter"].data<cool::UInt32>();
   vector<string> lvlGrp = split(al["Groups"].data<cool::String4k>(),";");
   string level = lvlGrp[0];
   if(lvlGrp.size()==2) {
      vector<string> grV = split(lvlGrp[1],",");
      for(HLTChain* ch : chl) {
         if( ch->chain_counter()==cc && ch->level()==level) {
            for(string gr : grV) ch->addGroup(gr);
            break;
         }
      }
   }
}


void
TrigConfCoolHLTPayloadConverters::readHltConfigKeys( const coral::AttributeList & al, unsigned int & masterConfigKey,
                   unsigned int & hltPrescaleKey, string & configSource)
{
   masterConfigKey = al["MasterConfigurationKey"].data<cool::UInt32>();
   hltPrescaleKey  = al["HltPrescaleConfigurationKey"].data<cool::UInt32>();
   configSource    = al["ConfigSource"].data<cool::String255>();
}


void
TrigConfCoolHLTPayloadConverters::readHltPrescale( const coral::AttributeList & al,
                 float& ps, float& pt, float& rrps)
{
   ps   = al["Prescale"].data<cool::Float>();
   pt   = al["Passthrough"].data<cool::Float>();
   rrps = al["RerunPrescale"].data<cool::Float>();
}


int
TrigConfCoolHLTPayloadConverters::readHltPrescaleKey( const coral::AttributeList & al)
{
   return al["HltPrescaleKey"].data<cool::UInt32>();
}

