/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVSvc/IOVSvcTool.h"
/*****************************************************************************
 *
 *  IOVSvcTool.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVSvcTool.cxx,v 1.15 2009-03-25 19:41:46 ivukotic Exp $
 *
 *  Tool to provide automatic updating and callbacks for time dependent data
 *
 *****************************************************************************/


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Guards.h"
#include "GaudiKernel/ConcurrencyFlags.h"

#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IOVRange.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"

#include "IOVSvc/IOVEntry.h"
#include "IOVSvc/IOVAddress.h"
#include "IOVSvc/CBTree.h"
#include "IOVSvc/IOVCallbackError.h"

#include <stdint.h>
#include <ctype.h>
#include <stdexcept>

using SG::DataProxy;
using SG::TransientAddress;

std::string toUpper(const std::string& str) {
  const char *cstr = str.c_str();
  std::string str2("");
  for (unsigned int i=0; i < str.length(); ++i) {
    str2 += toupper(*(cstr+i));
  }

  return str2;
}

namespace {
  bool s_firstRun(true);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

using namespace std;

//
///////////////////////////////////////////////////////////////////////////
//

bool
SortTADptr::operator() ( const SG::TransientAddress* x, 
                         const SG::TransientAddress* y) const {

  if ( x->clID() == y->clID() ) {
    return ( x->name() < y->name() );
  } else {
    return ( x->clID() < y->clID() );
  }

}

//
///////////////////////////////////////////////////////////////////////////
//

bool
SortDPptr::operator() (const SG::DataProxy* a, const SG::DataProxy *b) const {
  if (a&&b) {
    if (a->name()!=b->name()) return a->name()<b->name();
    if (a->clID()!=b->clID()) return a->clID()<b->clID();
  }
  //Fall back to ptr comp (in principle random, but similar name and
  //clid means that the path in COOL is likely to be similar):
  return a<b;
}

//
///////////////////////////////////////////////////////////////////////////
//


IOVSvcTool::IOVSvcTool(const std::string& type, const std::string& name,
                       const IInterface* parent): 
  AthAlgTool( type, name, parent ),
  m_storeName("StoreGateSvc"), 
  m_log(msgSvc(), "IOVSvcTool"), 
  p_sgSvc(0), 
  p_cndSvc("DetectorStore",name),
  p_incSvc("IncidentSvc",name), p_PPSvc("ProxyProviderSvc",name),
  p_CLIDSvc("ClassIDSvc",name), p_toolSvc("ToolSvc",name),
  m_curTime(0),
  p_startSet(nullptr),
  p_stopSet(nullptr),
  m_first(true), m_checkOnce(false),
  m_triggered(false), m_firstEventOfRun(false), m_resetAllCallbacks(false)

{

  // declare my special interface
  declareInterface<IIOVSvcTool>(this);


  declareProperty("preLoadRanges",m_preLoadRanges=false);
  declareProperty("preLoadData",m_preLoadData=false);
  declareProperty("partialPreLoadData",m_partialPreLoadData=true);
  declareProperty("updateInterval", m_updateInterval="Event");
  declareProperty("sortKeys",m_sortKeys=true);
  declareProperty("forceResetAtBeginRun",m_forceReset=false);

  m_trigTree = new CBTree();

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IOVSvcTool::~IOVSvcTool() {

  // cleanup

  std::map<const DataProxy*, IOVEntry*>::iterator itr;
  for (itr = m_entries.begin(); itr != m_entries.end(); ++itr) {
    IOVEntry *ent = itr->second;
    delete (ent);
  }

  ObjMap::iterator oitr;
  for (oitr = m_objMap.begin(); oitr != m_objMap.end(); ++oitr) {
    delete ( oitr->second );
  }

  for (std::map<CallBackID, BFCN*>::iterator i = m_cbidMap.begin();
       i != m_cbidMap.end();
       ++i)
    {
      delete i->second;
    }

  std::set< const TransientAddress*, SortTADptr >::const_iterator titr;
  for (titr = m_preLoad.begin(); titr != m_preLoad.end(); ++titr)
    {
      delete *titr;
    }
  
  delete m_trigTree;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::initialize() {

  StatusCode status;

  static const bool CREATEIF(true);

  IIOVSvc* p_iovSvc(0);
  status = service("IOVSvc", p_iovSvc,CREATEIF);
  if (status.isFailure()) {
    m_log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
    return status;
  }
  IProperty* iovSvcProp = dynamic_cast<IProperty*>( p_iovSvc );
  if (iovSvcProp == 0) {
    m_log << MSG::ERROR << "Unable to dcast the IOVSvc to an IProperty" 
          << endmsg;
    return StatusCode::FAILURE;
  }

  setProperty( iovSvcProp->getProperty("preLoadRanges") ).ignore();
  setProperty( iovSvcProp->getProperty("preLoadData") ).ignore();
  setProperty( iovSvcProp->getProperty("partialPreLoadData") ).ignore();
  setProperty( iovSvcProp->getProperty("updateInterval") ).ignore();
  setProperty( iovSvcProp->getProperty("sortKeys") ).ignore();
  setProperty( iovSvcProp->getProperty("forceResetAtBeginRun") ).ignore();
  setProperty( iovSvcProp->getProperty("OutputLevel") ).ignore();
  m_log.setLevel( msg().level() );

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Initializing IOVSvcTool version " 
          << PACKAGE_VERSION << endmsg;
  }
#endif

  if (m_storeName == "StoreGateSvc") {
    status = service("StoreGateSvc", p_sgSvc);
  } else {
    string sgn = "StoreGateSvc/" + m_storeName;
    status = service(sgn,p_sgSvc);
  }


  if (status.isFailure()) {
    m_log << MSG::ERROR << "Unable to get the StoreGateSvc" << endmsg;
    return status;
  }

  int pri=100;

  std::string updi = toUpper(m_updateInterval);

  if (updi== "JOB") {
    m_checkOnce = true;
    m_checkTrigger = "BeginRun";
    p_incSvc->addListener( this, "BeginRun", pri, true);
    m_log << MSG::INFO;
    m_log.setColor(MSG::GREEN);
    m_log << "IOVRanges will be checked only ";
    m_log.setColor(MSG::CYAN);
    m_log << "once";
    m_log.setColor(MSG::GREEN);
    m_log << " at the start of the job" << endmsg;
  } else if (updi == "RUN") {
    m_checkTrigger = "BeginRun";
    p_incSvc->addListener( this, "BeginRun", pri, true);
    m_log << MSG::INFO;
    m_log.setColor(MSG::GREEN);
    m_log << "IOVRanges will be checked at every ";
    m_log.setColor(MSG::CYAN);
    m_log << "Run" << endmsg;
  } else if (updi == "EVENT") {
    m_checkTrigger = "BeginEvent";
    p_incSvc->addListener( this, "BeginEvent", pri, true);
    p_incSvc->addListener( this, "BeginRun", pri, true);
    m_log << MSG::INFO;
    m_log.setColor(MSG::GREEN);
    m_log << "IOVRanges will be checked at every ";
    m_log.setColor(MSG::CYAN);
    m_log << "Event" << endmsg;
  } else {
    m_log << MSG::FATAL << "jobOption \"updateInterval\" must be one of "
          << "\"event\" \"run\" or \"job\"" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_preLoadData) {
    m_log << MSG::INFO;
    m_log.setColor(MSG::GREEN);
    m_log << "IOV Data will be preloaded at the same interval" << endmsg;
  }
    
  // For hybrid MP/MT
  p_incSvc->addListener( this, "ReloadProxies", pri, true);
 
#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Tool initialized" << endmsg;
  }
#endif

  return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::finalize() {


  // FIXME: do some cleanup

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::handle(const Incident &inc) {

  uint32_t event, run;
  
  std::string objname;

  bool first = m_first;

  // hybrid MP/MT: need to reload everything after workers fork
  if (inc.type() == "ReloadProxies") {
    m_resetAllCallbacks = true;
  }
  
  // Don't bother doing anything if we're handled the first run, and
  // preLoadData has been set, or if we only want to check once at the
  // beginning of the job
  if (!m_first && m_preLoadData && m_checkOnce) {
    return;
  }

  if (m_first) {
    for (auto e : m_ignoredProxyNames) {
      const DataProxy* proxy = p_cndSvc->proxy(e.first,e.second);

      if (proxy == 0) {
        m_log << MSG::ERROR << "ignoreProxy: could not retrieve proxy " 
              << fullProxyName(e.first,e.second) << " from store" << endmsg;
      } else {
        ignoreProxy( proxy );
        m_log << MSG::DEBUG << "will ignore resetting proxy "
              << fullProxyName(proxy)
              << endmsg;
      }
    }
  }

  set< const DataProxy*, SortDPptr > proxiesToReset;

  // Forcing IOV checks on the first event in the run for AthenaMP (ATEAM-439)
  if(Gaudi::Concurrency::ConcurrencyFlags::numProcs()==0) {
    if (inc.type() == IncidentType::BeginRun) {
      m_firstEventOfRun = true;
    }

    if (inc.type() == IncidentType::BeginEvent && m_firstEventOfRun) {
      m_firstEventOfRun = false;
      if (m_checkTrigger == "BeginEvent") {
	return;
      }
    }
  }

  if ( inc.type() == m_checkTrigger || inc.type() == IncidentType::BeginRun ) {

    EventContext context;
    const EventInfo* evt = p_sgSvc->tryConstRetrieve<EventInfo>();

    if( !evt ) {
      // If EventInfo is not in the event store, check whether 
      // whether we can get event context via BeginRun incident
      const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
      if(!eventInc) {
	m_log << MSG::ERROR 
	      << " Unable to get event information from either EventStore (via EventInfo) or " 
	      << inc.type() << " incident (via EventContext)" 
	      << endmsg;
	return;
      } else {
	context = inc.context();
	if (m_log.level() <= MSG::DEBUG) {
           m_log << MSG::DEBUG << "Got event context from " << inc.type() << " incident" << endmsg;
	}
      }
    }

    m_curTime.reset();
    
    const EventIDBase* eventID = (evt ? evt->event_ID() : &context.eventID());
    event = eventID->lumi_block();
    run   = eventID->run_number();
    
    m_log << MSG::DEBUG << "Got event info: " << "run="<< run << ", event=" << event << endmsg;
    m_curTime.setRunEvent(run,event);

    // get ns timestamp from event
    m_curTime.setTimestamp(1000000000L*(uint64_t)eventID->time_stamp() + eventID->time_stamp_ns_offset());

    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG;
      m_log.setColor(MSG::YELLOW,MSG::RED);
      m_log << inc.type() << ": [R/LB] = " << m_curTime << endmsg;
    }

    if (inc.type() == IncidentType::BeginRun) {
      // Signal BeginRun directly to IOVDbSvc
      IIOVDbSvc *iovDB = 0;
      if (StatusCode::SUCCESS != service("IOVDbSvc", iovDB, false)) {
        m_log << MSG::DEBUG << "Unable to get the IOVDbSvc" << endmsg;
        return;
      }
      if (StatusCode::SUCCESS != iovDB->signalBeginRun(m_curTime,
                                                       inc.context()))
      {
        m_log << MSG::ERROR << "Unable to signal begin run to IOVDbSvc" << endmsg;
        return;
      }
      else {
        if (m_log.level() <= MSG::DEBUG) {
          m_log << MSG::DEBUG << "Signaled begin run to IOVDbSvc " << m_curTime << endmsg;
        }
      }
    }
    
    if (m_first) {
      m_first = false;

      std::set< const TransientAddress*, SortTADptr >::const_iterator titr;
      for (titr = m_preLoad.begin(); titr != m_preLoad.end(); ++titr) {
        const TransientAddress *tad = *titr;
        StatusCode sc = regProxy(tad->clID(), tad->name());
        if (StatusCode::SUCCESS != sc) {
          m_log << MSG::ERROR << "handle: Could not register proxy for " <<
            fullProxyName(tad->clID(), tad->name()) << endmsg;
          return;
        }
      }

      if (m_log.level() <= MSG::VERBOSE) {
        m_log << MSG::VERBOSE;
        PrintProxyMap();
        m_log << endmsg;
      }

      if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "Callback Tree:" << endmsg;
        m_trigTree->printTree();
      }

      // preLoad the ranges and data if requested.
      if (preLoadProxies().isFailure()) {
        m_log << MSG::ERROR << "Problems preloading IOVRanges" << endmsg;
        throw( std::runtime_error("IOVSvcTool::preLoadProxies") );
      }

      // Signal EndProxyPreload directly to IOVDbSvc
      IIOVDbSvc *iovDB = 0;
      if (StatusCode::SUCCESS == service("IOVDbSvc", iovDB, false)) {
        iovDB->signalEndProxyPreload();
        if (m_log.level() <= MSG::DEBUG) {          
          m_log << MSG::DEBUG << "Signaled end proxy preload to IOVDbSvc " 
                << m_curTime << endmsg;
        }
      }
    }
    
    // If preLoadData has been set, never check validity of data again.
    if (m_preLoadData && m_checkOnce) {
      return;
    }

    // Otherwise, do the normal check for validity


    if (m_log.level() <= MSG::DEBUG) { 
      m_log << MSG::DEBUG;
      PrintStartSet();
      PrintStopSet();
      m_log << endmsg;
    }
    
    std::map<BFCN*, std::list<std::string> > resetKeys;

    //
    ////// Scan start and stop Sets for validity
    ////// We need to check both R/E and Clocktime sets
    //

    if (inc.type() == IncidentType::BeginRun && m_forceReset && !s_firstRun) {
      if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG 
              << "Resetting all proxies on BeginRun incident for store \""
              << m_storeName << "\"" << endmsg;
        m_log << MSG::VERBOSE;
        std::set< const SG::DataProxy* >::const_iterator pit;
        for (pit = m_proxies.begin(); pit != m_proxies.end(); ++pit) {
          m_log << "   " << m_names[*pit] << std::endl;
        }
        m_log << endmsg;
      }
      proxiesToReset = m_proxies;
      m_triggered = false;
    } else {
      scanStartSet(m_startSet_Clock,"(ClockTime)",proxiesToReset);
      scanStartSet(m_startSet_RE,"(R/E)",proxiesToReset);

      scanStopSet(m_stopSet_Clock,"(ClockTime)",proxiesToReset);
      scanStopSet(m_stopSet_RE,"(R/E)",proxiesToReset);
    }

    for (auto p : m_ignoredProxies) {
      auto itr = proxiesToReset.find(p);
      if (itr != proxiesToReset.end()) {
        proxiesToReset.erase( itr );
      }
    }

    // If MT, must not call any callback functions after first event
    if (!m_first && proxiesToReset.size() > 0 &&
        ( (Gaudi::Concurrency::ConcurrencyFlags::numThreads() +
           Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents()) > 0 ) ) {
      m_log << MSG::FATAL 
            << "Cannot update Conditions via callback functions in MT after the first event"
            << endmsg;
      throw GaudiException("Cannot update Conditions via callback functions in MT after the first event",name(),StatusCode::FAILURE);
    }

    //
    //// Reset DataProxies, and call associated callback functions
    //// 
    //
    set< const DataProxy* >::iterator itr;
    for (itr=proxiesToReset.begin(); itr!=proxiesToReset.end(); ++itr) {
      DataProxy *prx = const_cast<DataProxy*>( *itr );
      if (m_log.level() <= MSG::VERBOSE) {
        m_log << MSG::VERBOSE << "clearing proxy payload for " << m_names[prx]
              << endmsg;
      }

      // Reset proxy except when one wants to reset callbacks
      
      if (!m_resetAllCallbacks) p_cndSvc->clearProxyPayload( prx );

      m_trigTree->cascadeTrigger(true, *itr);

      // Load data if preload requested.

      if ( (m_partialPreLoadData && 
            m_partPreLoad.find(TADkey(*prx)) != m_partPreLoad.end())
           ||
           m_preLoadData ) {       
        if (m_log.level() <= MSG::VERBOSE) {
          m_log << MSG::VERBOSE << "preloading data" << endmsg;
        }

        Gaudi::Guards::AuditorGuard auditor(m_names[prx], auditorSvc(), "preLoadProxy");
        if (prx->accessData() == 0) {
          m_log << MSG::ERROR << "problems preloading data for "
                << m_names[prx] << endmsg;
        }
      }

      std::list<std::string> keys;
      pair<pmITR,pmITR> fitr = m_proxyMap.equal_range( prx );
      for (pmITR p=fitr.first; p!=fitr.second; ++p) {
        BFCN *f = p->second;
        std::string key = prx->name();
        if (resetKeys.find(f) == resetKeys.end()) {
          resetKeys[f] = std::list<std::string>();
        }
        resetKeys[f].push_back(key);
      }
    }

    /// Trigger Callback functions

    // Check to see if it's first event, and if preLoadProxies has already
    // called the functions

    IOVCallbackError* perr(0);
    
    if (! (first && m_triggered) ) {
      for (int i=2; i<= m_trigTree->maxLevel(); ++i) {
        CBTree::nodeSet::const_iterator itt, itt_s, itt_e;
        m_trigTree->listNodes( i, itt_s, itt_e );
        for (itt = itt_s; itt != itt_e; ++itt) {
          CBNode* node = *itt;

          if (node->trigger()) {
            BFCN *ff = node->fcn();
            auditorSvc()->before("Callback",m_fcnMap[ff].name());
            if ((*ff)(i,resetKeys[ff]).isFailure()) {
              auditorSvc()->after("Callback",m_fcnMap[ff].name());
              m_log << MSG::ERROR << "Problems calling " << m_fcnMap[ff].name()
                    << std::endl << "Skipping all subsequent callbacks."
                    << endmsg;
              // this will cause a mem leak, but I don't care
              perr = new IOVCallbackError(m_fcnMap[ff].name());
              break;            }
            auditorSvc()->after("Callback",m_fcnMap[ff].name());

          }
        }
        if (perr != 0) break;
      }
    }


    /// Clear trigger tree
    m_trigTree->clearTrigger();

    ///
    /// On reinitialize, one sets a flag to force reset of all
    /// callbacks. After executing the callbacks, reset flag and
    /// return - no proxies reset and don't need to read in new ranges 
    ///
    if (m_resetAllCallbacks) {
      m_resetAllCallbacks = false;
      if (perr != 0) throw (*perr);
      return;
    }
    
    
    /// Read in the next set of IOVRanges
    std::map<const DataProxy*, IOVEntry*>::iterator pitr;
    for (itr=proxiesToReset.begin(); itr!=proxiesToReset.end(); ++itr) {
      DataProxy *prx = const_cast<DataProxy*>( *itr );

      pitr = m_entries.find( prx );
      if ( pitr != m_entries.end() && pitr->second->range()->isInRange(m_curTime) ) {
        if (m_log.level() <= MSG::VERBOSE) {
          m_log << MSG::VERBOSE << "range still valid for " << m_names[prx]
                << endmsg;
        }
      } else { 
        if (m_log.level() <= MSG::DEBUG) {
          m_log << MSG::DEBUG << "calling provider()->udpateAddress(TAD) for " 
                << m_names[prx]    << endmsg;
        }
        if (!prx->updateAddress()) {
          m_log << MSG::ERROR << "handle: Could not update address" << endmsg;
          if (perr != 0) throw (*perr);
          return;
        }
      }
      
      if (m_log.level() <= MSG::VERBOSE) {
        IOpaqueAddress *ioa = prx->address();
        // Print out some debug info if this is an IOVAddress (coming 
        // from IOVASCIIDbSvc) 
        IOVAddress *iova  = dynamic_cast<IOVAddress*>(ioa);
        if (iova != 0) {
          m_log << MSG::VERBOSE << "  range: " << iova->range() << endmsg;
        }
      }

    }

    if (perr != 0) throw (*perr);

  }  // end if(inc.type() == m_checkTrigger)

  if ( inc.type() == IncidentType::BeginRun) {
    s_firstRun = false;
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Register a DataProxy with the service
///
StatusCode 
IOVSvcTool::regProxy( const DataProxy *proxy, const std::string& key) {


  if (proxy == 0) {
    m_log << MSG::ERROR << "proxy == 0" << endmsg;
    return StatusCode::FAILURE;
  }

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "registering proxy " << fullProxyName(proxy)
          << " at " << proxy << endmsg;
  }
#endif

  if (m_proxies.find(proxy) != m_proxies.end()) {
#ifndef NDEBUG
    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "Proxy for " << fullProxyName(proxy)
            << " already registered: " << proxy->name()
            << endmsg;
    }
#endif
    return StatusCode::SUCCESS;
  }

  std::string tname, fullname;
  StatusCode sc = p_CLIDSvc->getTypeNameOfID(proxy->clID(), tname);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << "Unable to get type name from ClassIDSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  
  fullname = tname + "[" + key + "]";

  m_proxies.insert( proxy );
  m_names[ proxy ] = fullname;

  m_trigTree->addNode(proxy,fullname);

  return StatusCode::SUCCESS;

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Deregister a DataProxy with the service
///
StatusCode 
IOVSvcTool::deregProxy( const DataProxy *proxy) {


  if (proxy == 0) {
    m_log << MSG::ERROR << "proxy == 0" << endmsg;
    return StatusCode::FAILURE;
  }

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "removing proxy " << fullProxyName(proxy)
          << " at " << proxy << endmsg;
  }
#endif

  std::set<const SG::DataProxy*, SortDPptr>::iterator itr = m_proxies.find(proxy);
  if (itr == m_proxies.end()) {
#ifndef NDEBUG
    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "Proxy for " << fullProxyName(proxy)
            << " not registered: " << proxy->name()
            << endmsg;
    }
#endif
    return StatusCode::SUCCESS;
  }

  m_proxies.erase( itr );

  m_trigTree->delNode(proxy);

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace {

  template <class SET>
  void removeFromSet (IOVEntry* ent, SET& set)
  {
    typename SET::iterator it = set.lower_bound(ent);
    while (it != set.end() && !set.key_comp()(*it, ent) && !set.key_comp()(ent,*it)) {
      if (*it == ent)
        set.erase (it++);
      else
        ++it;
    }
  }


} // anonymous namespace

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// replace a registered DataProxy with a new version
///
StatusCode 
IOVSvcTool::replaceProxy( const SG::DataProxy *pOld,
                          const SG::DataProxy *pNew) {
  assert(0 != pOld);
  assert(0 != pNew);
    
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "replace proxy " << fullProxyName(pOld)
          << " @" << pOld << " with " << fullProxyName(pNew)
          << " @" << pNew << endmsg;
  }
  
  //start with the proxy list
  if (0 == m_proxies.erase(pOld))  {
    m_log << MSG::DEBUG << "unregProxy: original proxy " 
          << fullProxyName(pOld) << " not found. Will return now " << endmsg;
    return StatusCode::SUCCESS;
  } 
  m_proxies.insert(pNew);
  //new name (possibly identical to old)
  m_names.erase(pOld);
  std::string tname;
  if ((p_CLIDSvc->getTypeNameOfID(pNew->clID(), tname)).isFailure()) {
    m_log << MSG::ERROR << "Unable to get type name from ClassIDSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_names[pNew]=tname + "[" + pNew->name() + "]";

  if (pOld != pNew) {
    std::map< const SG::DataProxy*, IOVEntry*>::iterator ent =
      m_entries.find(pOld);
    if (ent != m_entries.end()) {
      removeFromSet (ent->second, m_startSet_Clock);
      removeFromSet (ent->second, m_startSet_RE);
      removeFromSet (ent->second, m_stopSet_Clock);
      removeFromSet (ent->second, m_stopSet_RE);

      setRange_impl (pNew, *ent->second->range());
      delete ent->second;
      m_entries.erase (ent);
    }
  }

  return (m_trigTree->replaceProxy(pOld, pNew) ?
          StatusCode::SUCCESS :
          StatusCode::FAILURE );

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Register a DataProxy with the service
///
StatusCode 
IOVSvcTool::regProxy( const CLID& clid, const std::string& key ) {

  const DataProxy* proxy = p_cndSvc->proxy(clid,key);

  if (proxy == 0) {
    m_log << MSG::ERROR << "regProxy could not retrieve proxy " 
          << fullProxyName(clid,key) << " from store" << endmsg;
    return StatusCode::FAILURE;
  }

  return ( regProxy(proxy, key) );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// Deregister a DataProxy with the service
///
StatusCode 
IOVSvcTool::deregProxy( const CLID& clid, const std::string& key ) {

  const DataProxy* proxy = p_cndSvc->proxy(clid,key);

  if (proxy == 0) {
    m_log << MSG::ERROR << "regProxy could not retrieve proxy " 
          << fullProxyName(clid,key) << " from store" << endmsg;
    return StatusCode::FAILURE;
  }

  return ( deregProxy(proxy) );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// add to a set of TADs that will be registered at start of first event
///
StatusCode 
IOVSvcTool::preLoadTAD( const TransientAddress *tad_in ) {

  // check to see if it's a duplicate in preLoad
  if (m_preLoad.find( tad_in ) != m_preLoad.end()) {
    m_log << MSG::WARNING << "preLoadTAD: TransientAddress (" 
          << tad_in->clID() << "/" << tad_in->name() 
          << ") alread in preLoad set. Not inserting" << endmsg;
    return StatusCode::SUCCESS;
  }

  // check to see if it's a duplicate in partPreLoad
  if (m_partPreLoad.find( TADkey(*tad_in) ) != m_partPreLoad.end()) {
    m_log << MSG::WARNING << "preLoadTAD: TransientAddress (" 
          << tad_in->clID() << "/" << tad_in->name() 
          << ") alread in partPreLoad set. Not inserting" << endmsg;
    return StatusCode::SUCCESS;
  }

  TransientAddress* tad = new TransientAddress (tad_in->clID(),tad_in->name());
  m_preLoad.insert( tad );

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// add to a set of TADs that who's data will be preLoaded
///
StatusCode 
IOVSvcTool::preLoadDataTAD( const TransientAddress *tad_in ) {

  if (m_preLoad.find(tad_in) != m_preLoad.end()) {
    m_log << MSG::WARNING << "preLoadDataTAD: TransientAddress "
          << fullProxyName( tad_in )
          << " alread in preLoad set. Not inserting" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_partPreLoad.find(TADkey(*tad_in)) != m_partPreLoad.end()) {
    m_log << MSG::WARNING << "preLoadDataTAD: TransientAddress " 
          << fullProxyName( tad_in )
          << " alread in partPreLoad set. Not inserting" << endmsg;
    return StatusCode::SUCCESS;
  }

  TransientAddress* tad = new TransientAddress (tad_in->clID(),tad_in->name());
  m_preLoad.insert( tad );
  m_partPreLoad.insert( TADkey(*tad) );

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void IOVSvcTool::setRange_impl (const SG::DataProxy* proxy, IOVRange& iovr)
{
  if (iovr.start().isTimestamp()) {
    p_startSet = &m_startSet_Clock;
    p_stopSet  = &m_stopSet_Clock;
  } else {
    p_startSet = &m_startSet_RE;
    p_stopSet  = &m_stopSet_RE;
  }

  IOVRange *range = new IOVRange(iovr);

  map<const DataProxy*, IOVEntry*>::iterator itr = m_entries.find(proxy);
  if ( itr != m_entries.end() ) {

    IOVEntry *ent = itr->second;
    IOVRange *irn = ent->range();
    string objname = m_names[proxy];

    if (*irn == iovr) {
#ifndef NDEBUG
      if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "Range has not changed. Returning" << endmsg;
      }
#endif
      delete range;
      return;
      // is this true? still in the start and stop sets? FIXME
    }


    startITR sitr = ent->getStartITR();
    if ( !ent->removedStart() ) {
      p_startSet->erase( sitr );
    }



    stopITR pitr = ent->getStopITR();
    if ( !ent->removedStop() ) {
      p_stopSet->erase( pitr );
    }

    delete ent;
  }

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "adding to start and stop sets" << endmsg;
  }
#endif
  IOVEntry *ent = new IOVEntry(proxy,range);
  
  m_entries[ proxy ] = ent;

  ent->setStartITR( p_startSet->insert( ent ) );
  ent->setStopITR(  p_stopSet->insert( ent ) );
}


StatusCode 
IOVSvcTool::setRange(const CLID& clid, const std::string& key, 
                     IOVRange& iovr)
{

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "setRange()  for clid: " << clid << "  key: " << key 
          << "  in IOVrange:" << iovr << endmsg;
  }
#endif

  if (!iovr.start().isValid() || !iovr.stop().isValid()) {
    m_log << MSG::ERROR << "IOVRange " << iovr << "is not valid. Start OK: " 
          << iovr.start().isValid() << " Stop OK: " << iovr.stop().isValid() 
          << " run/evt/time min/max " 
          << IOVTime::MINRUN << "/" << IOVTime::MAXRUN << " "
          << IOVTime::MINEVENT << "/" << IOVTime::MAXEVENT << " "
          << IOVTime::MINTIMESTAMP << "/" << IOVTime::MAXTIMESTAMP << " "
          << endmsg;
    return StatusCode::FAILURE;
  }

  DataProxy* proxy = p_cndSvc->proxy(clid,key);

  if (proxy == 0) {
    m_log << MSG::ERROR << "setRange: Could not locate proxy for "
          << fullProxyName(clid,key) << endmsg;
    return StatusCode::FAILURE;
  }

  setRange_impl (proxy, iovr);
  return StatusCode::SUCCESS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::getRange(const CLID& clid, const std::string& key, 
                     IOVRange& iov) const {

  DataProxy* dp = p_cndSvc->proxy(clid,key);

  std::map<const DataProxy*,IOVEntry*>::const_iterator itr(m_entries.find(dp));
  if (itr == m_entries.end()) {
    return StatusCode::FAILURE;
  }

  iov = *(itr->second->range());

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::getRangeFromDB(const CLID& clid, const std::string& key, 
                           IOVRange& range, std::string &tag, 
                           IOpaqueAddress*& ioa) const {

  if (m_curTime.isValid()) {
    return getRangeFromDB(clid, key, m_curTime, range, tag, ioa);
  } else {
    m_log << MSG::ERROR << "Current Event not defined" << endmsg;
    return StatusCode::FAILURE;
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::getRangeFromDB(const CLID& clid, const std::string& key,
                           const IOVTime& time, IOVRange& range, 
                           std::string& tag, IOpaqueAddress*& ioa) const {
  StatusCode sc(StatusCode::FAILURE);
  DataProxy* dp = p_cndSvc->proxy(clid,key);
  if (0 != dp) {    
    IIOVDbSvc *idb = 
      dynamic_cast<IIOVDbSvc*>(dp->provider());
    if (idb != 0) {
      sc = idb->getRange(clid, key, time, range, tag, ioa);
    } else {
      m_log << MSG::ERROR << "Provider is not an IIOVDbSvc" << endmsg;
    }
  } else {
    m_log << MSG::ERROR << "No proxy found for clid " << clid 
          << " key " << key << endmsg;
  }
  return sc;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::setRangeInDB(const CLID& clid, const std::string& key, 
                         const IOVRange& range, const std::string &tag) {


  if (!range.start().isValid() || !range.stop().isValid()) {
    m_log << MSG::ERROR << "IOVRange " << range << "is not valid." << endmsg;
    return StatusCode::FAILURE;
  }

  DataProxy* dp = p_cndSvc->proxy(clid,key);

  if (dp == 0) {
    m_log << MSG::ERROR << "no Proxy found for "
          << fullProxyName( clid, key ) << endmsg;
    return StatusCode::FAILURE;
  }

  std::map<const DataProxy*,IOVEntry*>::const_iterator itr(m_entries.find(dp));
  if (itr == m_entries.end()) {
    m_log << MSG::WARNING << fullProxyName(clid,key)
          << " not registered with the IOVSvc" << endmsg;
  }

  IAddressProvider *iadp = dp->provider();
  IIOVDbSvc *idb = dynamic_cast<IIOVDbSvc*>(iadp);

  if (idb != 0) {
    return idb->setRange(clid, key, range, tag);
  } else {
    m_log << MSG::ERROR << "Provider is not an IIOVDbSvc" << endmsg;
    return StatusCode::FAILURE;
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::preLoadProxies() {
 
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "preLoadProxies()" << endmsg;
  }

  StatusCode scr(StatusCode::SUCCESS);
 
  std::set<const DataProxy*>::const_iterator itr;
  std::map<BFCN*, std::list<std::string> > resetKeys;
  for(itr = m_proxies.begin(); itr != m_proxies.end(); ++itr) {

    DataProxy *dp = const_cast<DataProxy*> (*itr);
    Gaudi::Guards::AuditorGuard auditor(m_names[dp], auditorSvc(), "preLoadProxy");
    
    if (m_log.level() <= MSG::VERBOSE) {
      m_log << MSG::VERBOSE;
      m_log.setColor(MSG::CYAN);
      m_log << "loading proxy for CLID: " << dp->clID()
            << "  " << m_names[dp] << endmsg;
    }

    if (dp->provider() == 0) {
      m_log << MSG::FATAL << "No provider found for proxy " << m_names[dp]
            << ".  It is probably  not a conditions object" << endl;
      m_log << "Proxy Map: ";
      PrintProxyMap(dp);
      m_log << endmsg;
      scr = StatusCode::FAILURE;
      return (scr);
    }


    StatusCode sc;
    // preload IOVRanges for callback functions or if jobOption set
    // This gets us to an IAddressProvider (eg IOVDbSvc)
    pair<pmITR,pmITR> pi = m_proxyMap.equal_range(dp);
    if (pi.first != pi.second || m_preLoadRanges) {
      if (m_log.level() <= MSG::VERBOSE) {
        m_log << MSG::VERBOSE << "updating Range" << endmsg;
      }
      if (!dp->updateAddress())
        sc = StatusCode::FAILURE;
    }

    if ( ( m_partialPreLoadData && 
           m_partPreLoad.find(TADkey(*dp)) != m_partPreLoad.end() )
         ||
         m_preLoadData ) {
      if (m_log.level() <= MSG::VERBOSE) {
        m_log << MSG::VERBOSE << "preloading data for (" 
              << dp->clID() << "/"
              << dp->name() << ")" << endmsg;
      }
      sc =  ( dp->accessData() != 0 ? 
              StatusCode::SUCCESS : StatusCode::FAILURE );
    }

    if (sc.isFailure()) scr=sc;


    // accumulate callBacks
    pmITR pitr;
    for (pitr=pi.first; pitr!=pi.second; ++pitr) {
      BFCN *f = pitr->second;
      std::string key = dp->name();
      if (resetKeys.find(f) == resetKeys.end()) {
        resetKeys[f] = std::list<std::string>();
      }
      resetKeys[f].push_back(key);
    }
    
    CBNode* cn = m_trigTree->findNode( *itr );
    if (cn != 0) {
      m_trigTree->cascadeTrigger(1, cn);
    }

  }

  if (scr.isFailure()) {
    m_log << MSG::ERROR << "Problems preLoading proxies. No callbacks triggered."
          << endmsg;
    return scr;
  }

  /// Trigger Callback functions
  for (int i=2; i<= m_trigTree->maxLevel(); ++i) {
    CBTree::nodeSet::const_iterator itt, itt_s, itt_e;
    m_trigTree->listNodes( i, itt_s, itt_e );
    for (itt = itt_s; itt != itt_e; ++itt) {
      CBNode* node = *itt;
      
      if (node->trigger()) {
        BFCN *ff = node->fcn();
        if (m_sortKeys) { resetKeys[ff].sort(); }
        auditorSvc()->before("Callback",m_fcnMap[ff].name());
        if ((*ff)(i,resetKeys[ff]).isFailure()) {
          auditorSvc()->after("Callback",m_fcnMap[ff].name());
          m_log << MSG::ERROR << "Problems calling " << m_fcnMap[ff].name() 
                << endmsg;
          return StatusCode::FAILURE;
        }
        auditorSvc()->after("Callback",m_fcnMap[ff].name());
      }
    }
  }

  m_trigTree->clearTrigger();

  m_triggered = true;

  return scr;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::triggerCallback(IOVSvcCallBackFcn* fcn, const std::string& key ) {
 
#ifndef NDEBUG
  if (m_log.level() <= MSG::VERBOSE) {
    m_log << MSG::VERBOSE << "triggerCallback(BFCN*)"<< endmsg;
  }
#endif

  int I;
  std::list<std::string> klist;
  klist.push_back(key);
  if ( (*fcn)(I,klist).isFailure() ) {
    m_log << MSG::ERROR << "calling " << m_fcnMap[fcn].name() << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::triggerCallback( const SG::DataProxy *dp, 
                             const std::string& key ) {
 
#ifndef NDEBUG
  if (m_log.level() <= MSG::VERBOSE) {
    m_log << MSG::VERBOSE << "triggerCallback(DataProxy*)"<< endmsg;
  }
#endif

  std::map<const SG::DataProxy*, BFCN*>::const_iterator pitr = 
    m_proxyMap.find(dp);
  if (pitr == m_proxyMap.end()) {
    m_log << MSG::ERROR << "no callback associated with DataProxy "
          << m_names[dp] << endmsg;
    return StatusCode::FAILURE;
  }

  BFCN* fcn = pitr->second;

  return ( triggerCallback(fcn,key) );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::PrintStartSet() {
  startITR start_itr;
  std::string objname;
  
  if (m_startSet_Clock.begin() != m_startSet_Clock.end()) {
    m_log << endl << "ClockTime start set: " << endl;
    for (start_itr = m_startSet_Clock.begin(); start_itr!=m_startSet_Clock.end(); ++start_itr ) {
      objname = m_names[ (*start_itr)->proxy() ];
      m_log << "  " << objname << " (" << (*start_itr)->proxy() << ") " 
            << (*start_itr)->range()->start() << endl;    
    }
    m_log << endl;
  }

  if (m_startSet_RE.begin() != m_startSet_RE.end()) {
    m_log << "Run/Event start set: " << endl;
    for (start_itr = m_startSet_RE.begin(); start_itr!=m_startSet_RE.end();++start_itr ) {
      objname = m_names[ (*start_itr)->proxy() ];
      m_log << "  " << objname << " (" << (*start_itr)->proxy() << ") "
            << (*start_itr)->range()->start() << endl;    
    }
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::PrintStopSet() {
  stopITR  stop_itr;
  std::string objname;
  
  if (m_stopSet_Clock.begin() != m_stopSet_Clock.end()) {
    m_log << endl << "ClockTime stop set: " << endl;
    for( stop_itr=m_stopSet_Clock.begin(); stop_itr!=m_stopSet_Clock.end(); ++stop_itr ) {
      objname = m_names[ (*stop_itr)->proxy() ];
      m_log << "  " << objname << " (" << (*stop_itr)->proxy() << ") "
            << (*stop_itr)->range()->stop() << endl;    
    }
    m_log << endl;
  }
  
  if (m_stopSet_RE.begin() != m_stopSet_RE.end()) {
    m_log << "Run/Event stop set: " << endl;
    for( stop_itr=m_stopSet_RE.begin(); stop_itr!=m_stopSet_RE.end(); ++stop_itr ) {
      objname = m_names[ (*stop_itr)->proxy() ];
      m_log << "  " << objname << " (" << (*stop_itr)->proxy() << ") " 
            << (*stop_itr)->range()->stop() << endl;    
    }
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::PrintProxyMap(){
  m_log << endl;
  m_log << "------------------------------  IOVSvc Proxy Map  "
        << "------------------------------" << endl;

  std::set<const DataProxy*>::const_iterator itr;
  for (itr=m_proxies.begin(); itr!=m_proxies.end(); ++itr) {

    PrintProxyMap(*itr);
    
  }
  m_log << "----------------------------------------------------------"
        << "---------------------" << endl;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::PrintProxyMap(const SG::DataProxy* dp){

  m_log << "  " << dp << "  " << dp->clID() << "  "
        << m_names.find(dp)->second << endl;

  pair<pmITR,pmITR> pi = m_proxyMap.equal_range(dp);
  pmITR pitr;
  
  if (pi.first == pi.second) {
    m_log << "         ->  no callback associated" << endl;
  } else {
    for (pitr=pi.first; pitr!=pi.second; ++pitr) {
      BFCN* fcn = pitr->second;
      map<BFCN*,CallBackID>::const_iterator fitr = m_fcnMap.find(fcn);
      CallBackID cbid = fitr->second;
      m_log << "         ->  " << fcn << "  " << cbid.name() << endl; 
    }
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::regFcn(SG::DataProxy* dp, 
                   const CallBackID c, 
                   const IOVSvcCallBackFcn& fcn,
                   bool trigger) {

  std::string tname,fullname;
  StatusCode sc = p_CLIDSvc->getTypeNameOfID( dp->clID(), tname );
  if (sc.isFailure()) {
    m_log << MSG::ERROR << "Unable to get type name from ClassIDSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  fullname = tname + "[" + dp->name() + "]";

  // see if proxy already bound
  if (m_proxies.find( dp ) == m_proxies.end()) {
    m_log << MSG::ERROR << "Cannot register object " << c.name()
          << " with DataHandle " << fullname 
          << " -> Need to bind DataHandle first" 
          << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_names[dp] = fullname;
  }

  // check if this prox/function pair already registered
  
  std::pair<pmITR,pmITR> fitr = m_proxyMap.equal_range( dp );
  for (pmITR p=fitr.first; p!=fitr.second; ++p) {
    if ( m_fcnMap[p->second] == c ) {
      m_log << MSG::ERROR << "CallBack function " << c.name()
            << " already registered against " << fullname << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // this function could have already been registered against another
  // DataProxy, so see if we can find it.
  BFCN *obs;
  if (m_cbidMap.find(c) == m_cbidMap.end()) {
    //    obs = new BFCN (boost::bind(updFcn,const_cast<T*>(obj),_1,_2));
    obs = new BFCN(fcn);
    m_cbidMap[c] = obs;
    m_fcnMap[obs] = c;
  } else {
    obs = m_cbidMap[c];
  }

  m_proxyMap.insert(std::pair<const SG::DataProxy*,BFCN* >(dp,obs));
  m_bfcnMap.insert(std::pair<BFCN*, const SG::DataProxy*> (obs,dp));

  // attach pointer to map of CallBackIDs
  ObjMap::const_iterator oitr = m_objMap.find(c.ptr());
  if ( oitr != m_objMap.end()) {
    oitr->second->insert(c);
  } else {
    std::set<CallBackID> *cbs = new std::set<CallBackID>;
    cbs->insert( c );
    m_objMap[c.ptr()] = cbs;
  }

  // add it to the trigger tree.
  CBNode *cn = m_trigTree->findNode(obs);
  if ( cn == 0) {
    m_trigTree->addNode(obs,c,dp);
  } else {
    CBNode *cp = m_trigTree->findNode(dp);
    if (cp)
      m_trigTree->connectNode(cn,cp);
    else
      m_log << MSG::ERROR << "Cannot find callback node for parent DataProxy "
            << dp->name()
            << endmsg;
  }

#ifndef NDEBUG
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "register by " << c.name() << " bound to " << fullname 
          << endmsg;
  }
#endif

  if (trigger) {
    if (m_first) {
      m_log << MSG::INFO 
            << "Still in initialize phase, not tiggering callback for "
            << c.name() << " bound to " << fullname
            << endmsg;
    } else {
      return triggerCallback(obs, dp->name());
    }
  }
    
  return StatusCode::SUCCESS;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::regFcn(const CallBackID c1,
                   const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
                   bool trigger) {

  // Check if second function has been registered with same proxy
  BFCN *obs1 = m_cbidMap[c1];
  BFCN *obs2;
  std::set<const SG::DataProxy*> proxyset;
  if (m_cbidMap.find(c2) != m_cbidMap.end()) {
    obs2 = m_cbidMap[c2];

    std::pair<fnITR,fnITR> fi1 = m_bfcnMap.equal_range( obs1 );
    for (fnITR fitr1= fi1.first; fitr1!=fi1.second; ++fitr1) {
      const SG::DataProxy* prx1 = fitr1->second;

      std::pair<fnITR,fnITR> fi2 = m_bfcnMap.equal_range( obs2 );
      for (fnITR fitr2=fi2.first; fitr2!=fi2.second; ++fitr2) {
        const SG::DataProxy* prx2 = fitr2->second;

        if (prx1 == prx2) {
#ifndef NDEBUG
          if (m_log.level() <= MSG::DEBUG) {
            m_log << MSG::DEBUG << "Callback function " << c2.name()
                  << " cannot be registered since it has already been registered "
                  << "against " << m_names[prx1] << endmsg;
          }
#endif
        } else {
          proxyset.insert(prx1);    // don't care if it gets done many times
        }
      }
    }
  } else {
    obs2 = new BFCN( fcn2 );
    m_cbidMap[c2] = obs2;
    m_fcnMap[obs2] = c2;

    // get all proxies that fcn1 is registered against
    std::pair<fnITR,fnITR> fi1 = m_bfcnMap.equal_range( obs1 );
    for(fnITR fitr1=fi1.first; fitr1!=fi1.second; ++fitr1) {
      const SG::DataProxy *prx1 = fitr1->second;
      proxyset.insert(prx1);
    }
  }

  if (proxyset.size() == 0) {
#ifndef NDEBUG
    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "Callback function " << c2.name() 
            << " cannot be registered, since it has already been registered"
            << " against everything it can be." << endmsg;
    }
#endif
    return StatusCode::SUCCESS;
  }

  // attach pointer to map of CallBackIDs
  ObjMap::const_iterator oitr = m_objMap.find(c2.ptr());
  if ( oitr != m_objMap.end()) {
    oitr->second->insert(c2);
  } else {
    std::set<CallBackID> *cbs = new std::set<CallBackID>;
    cbs->insert( c2 );
    m_objMap[c2.ptr()] = cbs;
  }

  // Link fcn2 to all proxies known to fcn1
  std::set<const SG::DataProxy*>::iterator pitr;
  std::list<std::string> klist;
  for (pitr=proxyset.begin(); pitr!=proxyset.end(); ++pitr) {
    const SG::DataProxy* prx = *pitr;
    m_proxyMap.insert(std::pair<const SG::DataProxy*,BFCN* >(prx,obs2));
    m_bfcnMap.insert(std::pair<BFCN*,const SG::DataProxy*>(obs2,prx));

#ifndef NDEBUG
    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "register by " << c2.name() << " bound to "
            << m_names[prx] << endmsg;
    }
#endif
    klist.push_back( prx->name() );

  }

  // note that the ordering of the parameters in addNode is the reverse 
  // order of  regFcn
  CBNode *cn = m_trigTree->findNode(obs2);
  if ( cn == 0) {
    m_trigTree->addNode(obs2,c2,obs1);
  } else {
    CBNode *cp = m_trigTree->findNode(obs1);
    if (cp == 0) {
      m_log << MSG::ERROR << "regFcn: could not locate parent of " << cn->name()
            << ". This should never happen" << endmsg;
      return StatusCode::FAILURE;
    }
    m_trigTree->connectNode(cn,cp);
  }


  if (trigger) {
    if (m_first) {
      m_log << MSG::INFO 
            << "Still in initialize phase, not tiggering callback for "
            << c2.name() << " bound to " << *klist.begin()
            << endmsg;
    } else {
      return triggerCallback(obs2, *(klist.begin()) );
    }
  }

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::regFcn(const IAlgTool* ia,
                   const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
                   bool trigger) {

  ObjMap::const_iterator oitr = m_objMap.find( ia );

  if (oitr == m_objMap.end()) {
    // tool not registered at all
    m_log << MSG::ERROR << "No callback registered with AlgTool " << ia->name()
          << endmsg;
    return StatusCode::FAILURE;

  } else {
    std::set<CallBackID> *sc = oitr->second;
    
    if (sc->size() == 1) {
      // this is ok - only one callback registered with this tool
      CallBackID cb = *(sc->begin());
      
      return regFcn(cb, c2, fcn2, trigger);

    } else {
      // there is more than one callback registered to this tool
    
      m_log << MSG::ERROR << "More than one callback registered to AlgTool "
            << ia->name() << ". Found : " << sc->size()
            << endmsg;
      return StatusCode::FAILURE;
    }
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
IOVSvcTool::getTriggeredTools(const std::string& key, 
                              std::set<std::string>& tools) {

  bool match = false;
  for (pmITR pitr=m_proxyMap.begin(); pitr != m_proxyMap.end(); ++pitr) {
    if (key == pitr->first->name()) {
      tools.insert( m_fcnMap[pitr->second].objName() );
      match = true;
    }
  }

  return ( (match) ? StatusCode::SUCCESS : StatusCode::FAILURE );
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
IOVSvcTool::reinitialize(){
  // Set flag to reset all proxies 
  m_resetAllCallbacks = true;
  return (StatusCode::SUCCESS);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::scanStartSet(startSet &pSet, const std::string &type, 
                         std::set<const SG::DataProxy*, SortDPptr> &proxiesToReset) {

  std::string objname;
  
  if (pSet.begin()==pSet.end())  return;
  
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "--> scan for resets: start set: " << type << endl;
  }

  startITR start_itr( pSet.begin() );
  while ( start_itr != pSet.end() ) {
    
    if (m_resetAllCallbacks || (*start_itr)->range()->start() > m_curTime) {
      if (m_log.level() <= MSG::DEBUG) {
        m_log << "\t" << m_names[ (*start_itr)->proxy() ] << ": " 
              << (*start_itr)->range()->start()<<"   <- removed"<<endl;
      }
      proxiesToReset.insert( (*start_itr)->proxy() );

      (*start_itr)->setRemovedStart( true );
      pSet.erase(start_itr++);

    } else {
      break;
    }
  }

  if (m_log.level() <= MSG::DEBUG) {
    m_log << endmsg;
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
IOVSvcTool::scanStopSet(stopSet &pSet, const std::string &type,
                        std::set<const SG::DataProxy*, SortDPptr> &proxiesToReset) {

  std::string objname;

  if (pSet.begin()==pSet.end())  return;
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "--> scan for resets: stop set: " << type << endl;
  }

  stopITR  stop_itr(pSet.begin());
  while ( stop_itr != pSet.end() ) {
    
    if (m_resetAllCallbacks || (*stop_itr)->range()->stop() <= m_curTime) {
      if (m_log.level() <= MSG::DEBUG) {
        m_log << "   " << m_names[ (*stop_itr)->proxy() ] << ": " 
              << (*stop_itr)->range()->stop()<< "  -> removed"<<endl;
      }
      proxiesToReset.insert( (*stop_itr)->proxy() );
      
      (*stop_itr)->setRemovedStop( true );
      pSet.erase(stop_itr++);
      
    } else {
      break;
    }
  }
  if (m_log.level() <= MSG::DEBUG) {
    m_log << endmsg;
  }  

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool
IOVSvcTool::holdsProxy( const DataProxy* proxy ) const {

  return ! ( m_proxies.find( proxy ) == m_proxies.end() );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool
IOVSvcTool::holdsProxy( const CLID& clid, const std::string& key ) const {

  const DataProxy* proxy = p_cndSvc->proxy(clid,key);

  if (proxy == 0) {
    m_log << MSG::ERROR << "holdsProxy: could not retrieve proxy "
          << fullProxyName(clid,key) << " from store" << endmsg;
    return false;
  }

  return ( holdsProxy(proxy) );

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool 
IOVSvcTool::holdsCallback( const CallBackID& cb ) const { 

  return ! (m_cbidMap.find(cb) == m_cbidMap.end());

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool 
IOVSvcTool::holdsAlgTool( const IAlgTool* ia ) const {

  ObjMap::const_iterator oitr = m_objMap.find( ia );

  return !(oitr == m_objMap.end());

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVSvcTool::resetAllProxies() {

  set< const DataProxy* >::iterator itr = m_proxies.begin();
  for (; itr != m_proxies.end(); ++itr) {
    DataProxy *prx = const_cast<DataProxy*>( *itr );
#ifndef NDEBUG
    if (m_log.level() <= MSG::VERBOSE) {
      m_log << MSG::VERBOSE << "clearing proxy payload for " << m_names[prx]
            << endmsg;
    }
#endif
    
    p_cndSvc->clearProxyPayload(prx);
    
    m_trigTree->cascadeTrigger(true, *itr);

  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::string
IOVSvcTool::fullProxyName( const TransientAddress* tad ) const {

  return fullProxyName(tad->clID(), tad->name());

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::string
IOVSvcTool::fullProxyName( const DataProxy* dp ) const {
  return fullProxyName(dp->clID(), dp->name());
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::string
IOVSvcTool::fullProxyName( const CLID& clid, const std::string& key ) const {

  std::string fullname, tname;
  ostringstream ost;
  ost << clid;
  if (p_CLIDSvc->getTypeNameOfID( clid, tname ).isFailure()) {
    fullname = "[" + ost.str() + "/" + key + "]";
  } else {
    fullname = "[" + tname + ":" + ost.str() + "/" + key + "]";
  }

  return fullname;
}
