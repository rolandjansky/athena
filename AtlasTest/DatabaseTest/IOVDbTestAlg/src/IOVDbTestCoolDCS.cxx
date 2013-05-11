/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbTestCoolDCS.cxx
// Algorithm to demonstrate simple reading of COOL DCS data into Athena
// Richard Hawkings started 9/9/05

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "IOVDbTestAlg/IOVDbTestCoolDCS.h"

IOVDbTestCoolDCS::IOVDbTestCoolDCS(const std::string& name,
				   ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  p_evtstore(0),
  p_detstore(0)
{
  declareProperty("AttrListFolders",par_atrlist);
  declareProperty("AttrListCollFolders",par_atrcollist);
}

IOVDbTestCoolDCS::~IOVDbTestCoolDCS()
{}

StatusCode IOVDbTestCoolDCS::initialize() {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "in initialize method" << endreq;

  // getr pointers to event and detector stores
  if (StatusCode::SUCCESS!=service("StoreGateSvc",p_evtstore)) {
    log << MSG::FATAL << "Event store not found" << endreq;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO <<  "DCS conditions data folders to be read as " << 
   "AthenaAttributeList (single channel)" << endreq;
  for (std::vector<std::string>::const_iterator itr=par_atrlist.begin();
       itr!=par_atrlist.end();++itr) log << MSG::INFO << *itr << endreq;
  log << MSG::INFO << "DCS conditions data folders to be read as " << 
  "CondAttrListCollection (multichannel)" << endreq;
  for (std::vector<std::string>::const_iterator itr=par_atrcollist.begin();
       itr!=par_atrcollist.end();++itr) log << MSG::INFO << *itr << endreq;

  return StatusCode::SUCCESS;
}

StatusCode IOVDbTestCoolDCS::execute() {
  MsgStream log(msgSvc(),name());
  // first print event number and time details
  const EventInfo* event;
  if (StatusCode::SUCCESS==p_evtstore->retrieve(event)) {
    int time=event->event_ID()->time_stamp();
    log << MSG::INFO << "In run/event [" << event->event_ID()->run_number() <<
      "," << event->event_ID()->event_number() << "] timestamp " << time << 
      endreq;
    // print the timestamp in UTC
    time_t ttime=static_cast<time_t>(time);
    log << MSG::INFO << "Timestamp UTC: " << asctime(gmtime(&ttime)) << endreq;
  } else {
    log << MSG::ERROR << "Could not get pointer to event" << endreq;
  }

  // print all the AthenaAttributeList
  const AthenaAttributeList* atrlist;
  for (std::vector<std::string>::const_iterator itr=par_atrlist.begin();
       itr!=par_atrlist.end();++itr) {
    if (StatusCode::SUCCESS==p_detstore->retrieve(atrlist,*itr)) {
      // the following code dumps the attribute list into a string for printing
      // to access individual elements by name, use e.g.
      // float var1=(*atrlist)["T04"].data<float>();
      // to get the value of a float column called T04 into var1
      std::ostringstream atrstring;
      atrlist->print(atrstring);
      log << MSG::INFO << "Values for folder " << *itr << ":" << endreq;
      log << MSG::INFO << atrstring.str() << endreq;
    } else {
      log << MSG::ERROR << "Could not retrieve AthenaAttributeList " <<
	*itr << endreq;
    }
  }
   
  // print all CondAttrListCollection
  const CondAttrListCollection* atrlistcol;
  for (std::vector<std::string>::const_iterator itr=par_atrcollist.begin();
       itr!=par_atrcollist.end();++itr) {
    if (StatusCode::SUCCESS==p_detstore->retrieve(atrlistcol,*itr)) {
      // loop over collection
      for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
	   citr!=atrlistcol->end();++citr) {
      // the following code dumps the attribute list into a string for printing
      // to access individual elements by name, use e.g.
      // float var1=(((*citr).second)["T04"]).data<float>();
      // to get the value of a float column called T04 into var1
	std::ostringstream atrstring;
	(*citr).second.toOutputStream(atrstring);
	log << MSG::INFO << "Channel " << (*citr).first << " "
	    << atrstring.str() << endreq;
      }
    } else {
      log << MSG::INFO << "Could not retrieve CondAttrListCollection " << 
	*itr << endreq;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbTestCoolDCS::finalize()
{
  return StatusCode::SUCCESS;
}
