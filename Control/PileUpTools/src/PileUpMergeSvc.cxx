/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/ITriggerTime.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataStore.h"
#include "PileUpTools/IPileUpXingFolder.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm>  
#include <cstdlib>  /* random */
#include <functional>
#include <string>

using std::make_pair;
using std::bind1st;
using std::mem_fun;
using std::string;

/// Standard Constructor
PileUpMergeSvc::PileUpMergeSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc), 
    p_overStore("StoreGateSvc", "StoreGateSvc"),
    p_activeStore("ActiveStoreSvc", "ActiveStoreSvc"),
    m_intervals(this),
    m_pITriggerTime(""),
    m_returnTimedData(true)
{
  declareProperty("Intervals", m_intervals, "Folders specifying bunch xing intervals for different data objects");
  declareProperty("TriggerTimeTool", m_pITriggerTime, "allows to apply a trigger time offset");
  declareProperty("ReturnTimedData", m_returnTimedData, 
		  "determine whether the TimedData returned by retrieveSubEvts have non trivial PileUpTimeEventIndex. May be set to false for overlay with real events ");
}

/// setup PileUpIntervals
void PileUpMergeSvc::decodeIntervals() {
  if (m_intervals.retrieve().isSuccess()) {
    ToolHandleArray<IPileUpXingFolder>::const_iterator 
      iXing(m_intervals.begin()), iEnd(m_intervals.end());
    while (iXing != iEnd) {
      const IPileUpXingFolder& xing(**iXing);
      if (msg().level() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "decodeIntervals: adding IPileUpXingFolder "
	      << xing.name() << endmsg;
      }
      IPileUpXingFolder::const_iterator 
	item(xing.begin()), endItem(xing.end());
      while(item != endItem) {
	m_ranges[make_pair(item->id(), item->key())]=
	  Range(xing.firstXing(), xing.lastXing(), xing.cacheRefreshFrequency());
	if (msg().level() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE 
		<< "decodeInterval: accepting objects of CLID " << item->id()
		<< " key " << item->key() << " in the Xing range ["
		<< xing.firstXing() << ", " << xing.lastXing() << ']' 
		<< " with cache refresh frequency " 
		<< xing.cacheRefreshFrequency() << endmsg;
	}
	++item;
      }
      ++iXing;
    }
  }
}

//////////////////////////////////////////////////////////////
/// Service initialisation
StatusCode 
PileUpMergeSvc::initialize()    {
  msg() << MSG::INFO << "Initializing AthService " << name() 
	<< " - package version " << PACKAGE_VERSION << endmsg ;
  // set up the SG service:
  if ( !(p_overStore.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate default store"
          << endmsg;
      return StatusCode::FAILURE;
  }
  // set up the active store service:
  if ( !(p_activeStore.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate ActiveStoreSvc"
          << endmsg;
      return StatusCode::FAILURE;
  }
  if (!m_pITriggerTime.empty() && !(m_pITriggerTime.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate ITriggerTime tool"
          << endmsg;
      return StatusCode::FAILURE;
  }

  decodeIntervals();
  
  // Initialize service:
  return StatusCode::SUCCESS;

}

const PileUpEventInfo* PileUpMergeSvc::getPileUpEvent() const {
  const PileUpEventInfo* p(0);
  if (p_overStore->contains<PileUpEventInfo>("OverlayEvent")) p_overStore->retrieve(p);
  return p;
}

const InterfaceID& 
PileUpMergeSvc::interfaceID() {
  static const InterfaceID IID_IPileUpMergeSvc(9991, 1, 0); //FIXME
  return IID_IPileUpMergeSvc; 
} 

  // Query the interfaces.
  //   Input: riid, Requested interface ID
  //          ppvInterface, Pointer to requested interface
  //   Return: StatusCode indicating SUCCESS or FAILURE.
  // N.B. Don't forget to release the interface after use!!!
StatusCode 
PileUpMergeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  //FIXME
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = (PileUpMergeSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool
PileUpMergeSvc::isLive(CLID id, const string& dataKey, int iXing) {
  return m_ranges[make_pair(id, dataKey)].contains(iXing);
}

bool
PileUpMergeSvc::doRefresh(const Range& r, int iXing) {
  //  bool result(r.contains(iXing) && r.doRefresh(double(random())/RAND_MAX));
  //  std::cerr << "doRefresh: id " << id << " key " << dataKey 
  //	    << " xing " << iXing << " result " << result << std::endl;
  return (r.contains(iXing) && r.doRefresh(double(random())*(1./RAND_MAX)));
}

StatusCode
PileUpMergeSvc::clearDataCaches() {
  StatusCode sc(StatusCode::FAILURE);
  const PileUpEventInfo* pEvent;
  if (0 != (pEvent=getPileUpEvent())) {
    // access the sub events DATA objects...
    PileUpEventInfo::SubEvent::const_iterator iEvt = pEvent->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator endEvt = pEvent->endSubEvt();
    while (iEvt != endEvt) {
      StoreGateSvc* pSubEvtSG(iEvt->pSubEvtSG);
      assert(pSubEvtSG);
      //go object-by-object (driven by PileUpXingFolder settings)
      for (const auto& item : m_ranges) {
        SG::sgkey_t sgkey = pSubEvtSG->stringToKey (item.first.second,
                                                      item.first.first);
        SG::DataProxy* proxy = pSubEvtSG->proxy_exact (sgkey);
        //FIXME turning the double iEvt->time is fraught with peril. Luckily 
        //FIXME it just works, but we should have the beam xing in iEvt
        if (proxy && doRefresh (item.second, int(iEvt->time()))) {
          proxy->setObject ((DataObject*)0);
          if (msg().level() <= MSG::DEBUG) {
            msg() << MSG::DEBUG
                  << "clearDataCachesByFolder: object with clid "
                  << item.first.first << " and key " << item.first.second
                  << " removed from cache " 
                  << pSubEvtSG->name() << endmsg;
          }
        }
      }
      //even if we don't clear the store we need to empty the trash...
      iEvt->pSubEvtSG->emptyTrash();
#ifndef NDEBUG
      if (msg().level() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE
	      << "clearDataCachesByFolder: done with store " << iEvt->pSubEvtSG->name()
	      << endmsg;
      }
#endif
      ++iEvt;	
    } //stores loop
    sc=StatusCode::SUCCESS;
  } //NO PILEUP EVENT?!?
  return sc;
}
