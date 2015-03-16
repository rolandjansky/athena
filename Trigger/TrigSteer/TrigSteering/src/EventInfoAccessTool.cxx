/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "boost/foreach.hpp"

#include <iostream>
using namespace HLT;


EventInfoAccessTool::EventInfoAccessTool(const std::string& name, const std::string& type,
                                         const IInterface* parent)
  : AthAlgTool(name, type, parent),
    m_log(0),
    m_storeGate("StoreGateSvc", name)
{
  declareInterface<HLT::IEventInfoAccessTool>( this );
  declareProperty( "ListOfChainsAddingStreamTag", m_listOfChainsAddingStreamTag,"List of chain:streamtag that can add streamtag after rerun (default is empty)");
}


StatusCode EventInfoAccessTool::initialize()
{
   m_log = new MsgStream( msgSvc(), name());

   CHECK(m_storeGate.retrieve());

   (*m_log) << MSG::DEBUG <<"initialized " << name() << endreq;
   return StatusCode::SUCCESS;
}

StatusCode EventInfoAccessTool::finalize()
{
   delete m_log; m_log=0;
   return StatusCode::SUCCESS;
}


bool lessThan_StreamTag(const xAOD::EventInfo::StreamTag& a, const xAOD::EventInfo::StreamTag& b) {
  return (a.name() + a.type() < b.name() + b.type());
}

bool equal_StreamTag(const xAOD::EventInfo::StreamTag& a, const xAOD::EventInfo::StreamTag& b) {
  return (a.name() + a.type() == b.name() + b.type());
}

//add the stream tag of the required chain, even if prescaled
StatusCode HLT::EventInfoAccessTool::updateStreamTag(const std::vector<SteeringChain*>& activeChains)
{
  (*m_log) << MSG::DEBUG <<" updateStreamTag, list has size " <<m_listOfChainsAddingStreamTag.value().size() << endreq;
  if (m_listOfChainsAddingStreamTag.value().size() == 0) return StatusCode::SUCCESS;

  // const std::vector<std::string>& incol2 = m_listOfChainsAddingStreamTag.value();
  // for (unsigned int i=0; i< m_listOfChainsAddingStreamTag.value().size(); i++){
  //   (*m_log) << MSG::DEBUG << incol2[i] <<endreq;
  // }



  //get EventInfo
  const xAOD::EventInfo* constxEventInfo(0);
  StatusCode sc = m_storeGate->retrieve(constxEventInfo);  
  if(sc.isFailure()){
    (*m_log) << MSG::FATAL << "Can't get EventInfo object for update event information"  << endreq;
    return StatusCode::FAILURE;
  } 

  //get StreamTags
   const std::vector< xAOD::EventInfo::StreamTag > &streams = constxEventInfo->streamTags();
  if(streams.empty()) {
    (*m_log) << MSG::DEBUG << "[xAOD::EventInfo] Event has no trigger streams" << endreq;
  } else {
    (*m_log) << MSG::DEBUG << "[xAOD::EventInfo] Event has " << streams.size() << " stream(s)" << endreq;
    BOOST_FOREACH(const xAOD::EventInfo::StreamTag st, streams ) {
      (*m_log) << MSG::DEBUG <<"stream = "<<st.name()<<" type = "<< st.type() << endreq;
    }
  }

  (*m_log) << MSG::DEBUG <<"Found "<<activeChains.size() <<" active chains" <<endreq;

  std::vector< xAOD::EventInfo::StreamTag > new_streams;

  // // print here the chains tht are active: 
  // BOOST_FOREACH( const HLT::SteeringChain* chain, activeChains ) {
  //   std::cout <<" FPP Active chain print ---> " << (*chain) << std::endl;
  // }

  // retireve the chains we want to activate streaming of
  const std::vector<std::string>& incol = m_listOfChainsAddingStreamTag.value();
  for (unsigned int i=0; i< m_listOfChainsAddingStreamTag.value().size(); i++){
    std::size_t pos = incol[i].find(':');
    if ( pos ==std::string::npos ){
      (*m_log) << MSG::ERROR <<" Wrong format of the string, that must be chainname:streamname. String is instead: " << incol[i] << endreq;
      continue;
    }
    std::string chain_to_add = incol[i].substr(0, pos);
    std::string stream_to_add = incol[i].substr(pos+1);

    // check if the stream is already in
    bool already_in=false;
    BOOST_FOREACH(const xAOD::EventInfo::StreamTag st, streams ) {
      if(stream_to_add == st.name()) {
	already_in=true;
	break;
      }
    }
    BOOST_FOREACH(const xAOD::EventInfo::StreamTag st, new_streams ) {
      if(stream_to_add == st.name()) {
	already_in=true;
	break;
      }
    }

    if (already_in) continue;
    (*m_log) << MSG::DEBUG <<"This chain wants to add its streamtag: "<< chain_to_add <<" stream = "<< stream_to_add  <<endreq;
   

    bool addStream=false;
    BOOST_FOREACH( const HLT::SteeringChain* chain, activeChains ) {
      if (chain->getChainName() != chain_to_add){
	continue;
      }

      //take only those in rerun
      if (! chain->isResurrected()) {
	(*m_log) << MSG::DEBUG<<"Chain did not passed in rerun" <<endreq;
	continue;
      }

       (*m_log) << MSG::DEBUG<<"This chain is good candidate to add streamtag: passed="<<chain->chainPassed() <<" resurrected=" << chain->isResurrected() <<" already_in ="<<already_in<<std::endl;
       (*m_log) << MSG::DEBUG<<" print ---> " << (*chain) << endreq;

      // check that the requested streamtag is defined for this chain      
      BOOST_FOREACH ( const HLT::StreamTag& chain_stream, chain->getStreamTags() ) {
	if (chain_stream.getStream() == stream_to_add) {
	  addStream=true;
	  //Now add the streamtag
	  //getPEB
	  PartialEventBuildingInfo chain_pebi;
	  if ( chain->getPEBInfo() ) {
	    chain_pebi.merge(*chain->getPEBInfo());
	  }
	  (*m_log) << MSG::DEBUG <<"YUPPIE Adding "<<chain_stream.getStream() <<" type="<< chain_stream.getType() <<" getObeyLB="<< chain_stream.getObeyLB() 
		   <<" ROBs="<<chain_pebi.getROBs().size() <<" subID="<<chain_pebi.getSubDetectors().size() <<endreq;
	  new_streams.emplace_back(chain_stream.getStream(), 
				   chain_stream.getType(),  
				   chain_stream.getObeyLB() , 
				   chain_pebi.getROBs(), 
				   chain_pebi.getSubDetectors());

	  break;
	}
	//else (*m_log) << MSG::DEBUG << "StreamTag "<< chain_stream.getStream() << " did not match" <<endreq;
      }
      if ( addStream) break;
    }
    if (!addStream) (*m_log) << MSG::DEBUG <<"Stream tag "<< stream_to_add <<" not added" <<endreq;
  }
 

  if (new_streams.size() == 0 ) {
    (*m_log) << MSG::DEBUG <<"No streams added"<<endreq;
    return StatusCode::SUCCESS;
  }

  (*m_log) << MSG::DEBUG <<"Adding "<<new_streams.size()<<" streams to EventInfo:" <<endreq;

  // merge new and old
  std::vector< xAOD::EventInfo::StreamTag > xAODStreamTags;
  BOOST_FOREACH(const  xAOD::EventInfo::StreamTag& st, streams ) {
    xAODStreamTags.push_back(st);
  }
  BOOST_FOREACH(const  xAOD::EventInfo::StreamTag& st, new_streams ) {
    xAODStreamTags.push_back(st);
  }

  //sort 
  std::sort(xAODStreamTags.begin(), xAODStreamTags.end(), lessThan_StreamTag);

  // Update EventInfo and xAODEventInfo
  xAOD::EventInfo* xeventInfo = const_cast<xAOD::EventInfo*>(constxEventInfo);
  xeventInfo->setStreamTags(xAODStreamTags);
  (*m_log) << MSG::VERBOSE << "Updated xAOD::StreamTags into xAOD::EventInfo:" << endreq;
  (*m_log) << MSG::DEBUG << "After stream purge event has " << xeventInfo->streamTags().size() << " stream(s) in xAOD::EventInfo" <<endreq;
  
  // record non xAOD
  //create StreamTags from xAODStreamTag
  std::vector < TriggerInfo::StreamTag > StreamTags;
  for (auto streamtag : xAODStreamTags){
    //copy into streamtag 
    TriggerInfo::StreamTag newst(streamtag.name(), streamtag.type(), streamtag.obeysLumiblock(), streamtag.robs(), streamtag.dets());
    StreamTags.push_back(newst);
  }
  //get EventInfo
  const EventInfo* constEventInfo(0);
  sc = m_storeGate->retrieve(constEventInfo);
  if(sc.isFailure()){
    (*m_log) << MSG::FATAL << "Can't get EventInfo object for update of the StreamTag and TriggerInfo" << endreq;
    return StatusCode::FAILURE;
  }

  EventInfo* eventInfo = const_cast<EventInfo*>(constEventInfo);
  TriggerInfo* triggerInfo = eventInfo->trigger_info();
  triggerInfo->setStreamTags(StreamTags);
  (*m_log) << MSG::VERBOSE << "Updated TriggerInfo::StreamTags into EventInfo:" << endreq;
  (*m_log) << MSG::DEBUG << "After stream purge event has " << triggerInfo->streamTags().size() << " stream(s) in EventInfo" <<endreq;


   return StatusCode::SUCCESS;
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
