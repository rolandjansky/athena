/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : EventInfoAccessTool
 *
 * @brief Tool to extract EventInfo
 *
 **********************************************************************************/

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "TrigSteering/EventInfoAccessTool.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "eformat/SourceIdentifier.h"

#include <iostream>
#include <algorithm>
using namespace HLT;


EventInfoAccessTool::EventInfoAccessTool(const std::string& name, const std::string& type,
                                         const IInterface* parent)
  : AthAlgTool(name, type, parent)
{
  declareInterface<HLT::IEventInfoAccessTool>( this );
  declareProperty( "ListOfChainsAddingStreamTag", m_listOfChainsAddingStreamTag,"List of chain:streamtag that can add streamtag after rerun (default is empty)");
}


StatusCode EventInfoAccessTool::initialize()
{
   ATH_MSG_DEBUG("initialized " << name());
   return StatusCode::SUCCESS;
}

StatusCode EventInfoAccessTool::finalize()
{
   return StatusCode::SUCCESS;
}


bool lessThan_StreamTag(const xAOD::EventInfo::StreamTag& a, const xAOD::EventInfo::StreamTag& b) {
  return (a.name() + a.type() < b.name() + b.type());
}

bool equal_StreamTag(const xAOD::EventInfo::StreamTag& a, const xAOD::EventInfo::StreamTag& b) {
  return (a.name() + a.type() == b.name() + b.type());
}

// Supply a pointer, it will be set to point to a const vector of stream tags
StatusCode HLT::EventInfoAccessTool::getStreamTags(std::vector< xAOD::EventInfo::StreamTag >& streamTags) {
  //get EventInfo
  const xAOD::EventInfo* constxEventInfo(0);
  CHECK(evtStore()->retrieve(constxEventInfo));  

  //get StreamTags
  const std::vector< xAOD::EventInfo::StreamTag > &streams = constxEventInfo->streamTags();
  if(streams.empty()) {
    ATH_MSG_DEBUG("[xAOD::EventInfo] Event has no trigger streams");
  } else {
    ATH_MSG_DEBUG("[xAOD::EventInfo] Event has " << streams.size() << " stream(s)");
    for(const xAOD::EventInfo::StreamTag& st : streams ) {
      ATH_MSG_DEBUG("xAOD::EventInfo::StreamTag stream = "<<st.name()<<" type = "<< st.type());
    }
  }

  streamTags = streams;
  return StatusCode::SUCCESS;
}

// Supply a vector of new stream tags to add to the event
StatusCode HLT::EventInfoAccessTool::addStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& new_streams) {
  ATH_MSG_DEBUG("Adding "<<new_streams.size()<<" streams to EventInfo:");

  // Get current
  const xAOD::EventInfo* constxEventInfo(0);
  CHECK(evtStore()->retrieve(constxEventInfo));
  const std::vector< xAOD::EventInfo::StreamTag > &streams = constxEventInfo->streamTags();

  // merge new and old
  std::vector< xAOD::EventInfo::StreamTag > xAODStreamTags;
  xAODStreamTags.reserve(streams.size());
for(const xAOD::EventInfo::StreamTag& st : streams ) {
    xAODStreamTags.push_back(st);
  }
  for(const xAOD::EventInfo::StreamTag& st : new_streams ) {
    xAODStreamTags.push_back(st);
  }

  //sort 
  std::sort(xAODStreamTags.begin(), xAODStreamTags.end(), lessThan_StreamTag);

  // Set to this merged list
  return setStreamTags( xAODStreamTags );
}

// Supply a vector of existing stream tags to remove from the event
StatusCode HLT::EventInfoAccessTool::removeStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& remove_streams) {
  ATH_MSG_DEBUG("Removing "<<remove_streams.size()<<" streams from EventInfo:");

  // Get current
  const xAOD::EventInfo* constxEventInfo(0);
  CHECK(evtStore()->retrieve(constxEventInfo));
  const std::vector< xAOD::EventInfo::StreamTag > &streams = constxEventInfo->streamTags();

  // Re-add old, checking against remove_streams
  std::vector< xAOD::EventInfo::StreamTag > xAODStreamTags;
  for(const xAOD::EventInfo::StreamTag& st : streams ) {
    bool matched = false;
    // Stream tags do not have a built in comparator so we need to use our own
    for(const xAOD::EventInfo::StreamTag& remove_st : remove_streams ) {
      if ( equal_StreamTag(st, remove_st) ) {
        ATH_MSG_DEBUG("Stream "<<st.name()<<"/"<< st.type()<<" located. Removing it.");
        matched = true;
        break;
      }
    }
    if (matched == true) continue;
    // Else we keep it
    xAODStreamTags.push_back(st);
  }

  //sort 
  std::sort(xAODStreamTags.begin(), xAODStreamTags.end(), lessThan_StreamTag);

  // Set to this merged list
  return setStreamTags( xAODStreamTags );
}

// Supply a vector of stream tags to replace the current set of stram tags. Be careful.
StatusCode HLT::EventInfoAccessTool::setStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& set_streams) {
  //get EventInfo
  const xAOD::EventInfo* constxEventInfo(0);
  CHECK(evtStore()->retrieve(constxEventInfo));  

  // Update EventInfo and xAODEventInfo
  xAOD::EventInfo* xeventInfo = const_cast<xAOD::EventInfo*>(constxEventInfo);
  xeventInfo->setStreamTags(set_streams);
  ATH_MSG_VERBOSE("Updated xAOD::StreamTags in xAOD::EventInfo:");
  ATH_MSG_DEBUG("After update, event has " << xeventInfo->streamTags().size() << " stream(s) in xAOD::EventInfo");

  for(const xAOD::EventInfo::StreamTag& st : set_streams ) {
    ATH_MSG_DEBUG("xAOD::EventInfo::StreamTag stream = "<<st.name()<<" type = "<< st.type());
  }

  // record non xAOD - backwards compatability
  //create StreamTags from xAODStreamTag
  std::vector < TriggerInfo::StreamTag > StreamTags;
  for (const auto& streamtag : set_streams){
    //copy into streamtag 
    TriggerInfo::StreamTag newst(streamtag.name(), streamtag.type(), streamtag.obeysLumiblock(), streamtag.robs(), streamtag.dets());
    StreamTags.push_back(newst);
  }
  //get EventInfo
  const EventInfo* constEventInfo(0);
  CHECK(evtStore()->retrieve(constEventInfo));

  EventInfo* eventInfo = const_cast<EventInfo*>(constEventInfo);
  TriggerInfo* triggerInfo = eventInfo->trigger_info();
  triggerInfo->setStreamTags(StreamTags);
  ATH_MSG_VERBOSE("Updated TriggerInfo::StreamTags in EventInfo:");
  ATH_MSG_DEBUG("Back compatiblity: After update, event has " << triggerInfo->streamTags().size() << " stream(s) in EventInfo");
  for(const TriggerInfo::StreamTag& st : StreamTags ) {
    ATH_MSG_DEBUG("TriggerInfo::StreamTag stream = "<<st.name()<<" type = "<< st.type());
  }

  return StatusCode::SUCCESS;
}

//add the stream tag of the required chain, even if prescaled
StatusCode HLT::EventInfoAccessTool::updateStreamTag(const std::vector<SteeringChain*>& activeChains)
{
  ATH_MSG_DEBUG(" updateStreamTag, list has size " <<m_listOfChainsAddingStreamTag.value().size());
  if (m_listOfChainsAddingStreamTag.value().size() == 0) return StatusCode::SUCCESS;

  // const std::vector<std::string>& incol2 = m_listOfChainsAddingStreamTag.value();
  // for (unsigned int i=0; i< m_listOfChainsAddingStreamTag.value().size(); i++){
  //   ATH_MSG_DEBUG(incol2[i]);
  // }

  ATH_MSG_DEBUG("Found "<<activeChains.size() <<" active chains");

  std::vector< xAOD::EventInfo::StreamTag > streams;
  CHECK( getStreamTags(streams) ); 

  std::vector< xAOD::EventInfo::StreamTag > new_streams;

  // // print here the chains tht are active: 
  // for( const HLT::SteeringChain* chain : activeChains ) {
  //   std::cout <<" FPP Active chain print ---> " << (*chain) << std::endl;
  // }

  // retireve the chains we want to activate streaming of
  const std::vector<std::string>& incol = m_listOfChainsAddingStreamTag.value();
  for (unsigned int i=0; i< m_listOfChainsAddingStreamTag.value().size(); i++){
    std::size_t pos = incol[i].find(':');
    if ( pos ==std::string::npos ){
      ATH_MSG_ERROR(" Wrong format of the string, that must be chainname:streamname. String is instead: " << incol[i]);
      continue;
    }
    std::string chain_to_add = incol[i].substr(0, pos);
    std::string stream_to_add = incol[i].substr(pos+1);

    // check if the stream is already in
    bool already_in=false;
    for(const xAOD::EventInfo::StreamTag& st : streams ) {
      if(stream_to_add == st.name()) {
        already_in=true;
        break;
      }
    }
    for(const xAOD::EventInfo::StreamTag& st : new_streams ) {
      if(stream_to_add == st.name()) {
        already_in=true;
        break;
      }
    }

    if (already_in) continue;
    ATH_MSG_DEBUG("This chain wants to add its streamtag: "<< chain_to_add <<" stream = "<< stream_to_add);

    bool addStream=false;
    for( const HLT::SteeringChain* chain : activeChains ) {
      if (chain->getChainName() != chain_to_add){
        continue;
      }

      ATH_MSG_DEBUG("Check if this chain is good candidate to add streamtag: passedRaw="<<chain->chainPassedRaw() 
		    <<" passed="      << chain->chainPassed()
		    <<" resurrected=" << chain->isResurrected() 
		    <<" run="         << chain->runInFirstPass()
		    <<" rerun="       << chain->runInSecondPass()
		    <<" already_in =" << already_in);
      ATH_MSG_DEBUG(" print ---> " << (*chain));

      //take only those passing in rerun
     if ( !(chain->runInSecondPass() && chain->chainPassedRaw() ) ) {
       ATH_MSG_DEBUG("Chain " << chain->getChainName() <<" did not passed in rerun, no streamtag added");
       continue;
     }

 
      // check that the requested streamtag is defined for this chain      
      for ( const HLT::StreamTag& chain_stream : chain->getStreamTags() ) {
        if (chain_stream.getStream() == stream_to_add) {
          addStream=true;
          //Now add the streamtag
          //getPEB
          PartialEventBuildingInfo chain_pebi;
          if ( chain->getPEBInfo() ) {
            chain_pebi.merge(*chain->getPEBInfo());
          }
          ATH_MSG_DEBUG("YUPPIE Adding to EventInfo: stream "<<chain_stream.getStream() <<" type="<< chain_stream.getType() <<" getObeyLB="<< chain_stream.getObeyLB() 
            <<" ROBs="<<chain_pebi.getROBs().size() <<" subID="<<chain_pebi.getSubDetectors().size());
            new_streams.emplace_back(chain_stream.getStream(), 
            chain_stream.getType(),  
            chain_stream.getObeyLB() , 
            chain_pebi.getROBs(), 
            chain_pebi.getSubDetectors());

          break;
        }
      //else ATH_MSG_DEBUG("StreamTag "<< chain_stream.getStream() << " did not match");
      }
      if (addStream) break;
    }
    if (!addStream) ATH_MSG_DEBUG("Stream tag "<< stream_to_add <<" not added");
  }
 
  if (new_streams.size() == 0 ) {
    ATH_MSG_DEBUG("No streams added");
    return StatusCode::SUCCESS;
  }

  return addStreamTags( new_streams );
}



StatusCode
EventInfoAccessTool::queryInterface( const InterfaceID& riid,
                                     void** ppvIf )
{
   if ( riid == IEventInfoAccessTool::interfaceID() )  {
      *ppvIf = (IEventInfoAccessTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }
   
   return AlgTool::queryInterface( riid, ppvIf );
}
