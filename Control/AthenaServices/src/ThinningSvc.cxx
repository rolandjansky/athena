///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinningSvc.cxx 
// Implementation file for class ThinningSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <set>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

// AthenaServices includes
#include "ThinningSvc.h"

using Athena::IThinningHdlr;
using Athena::ISlimmingHdlr;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ThinningSvc::ThinningSvc( const std::string& name, 
                          ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_slimmingStore   ( ),
  m_thinningStore   ( ),
  m_thinningOccurred(false),
  m_storeGate       ("StoreGateSvc", name)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  declareProperty( "StoreGate",
		   m_storeGate = StoreGateSvc_t( "StoreGateSvc", name ),
		   "Handle to the StoreGateSvc instance holding containers "
		   "which will be thinned in the course of the job" );

  declareProperty( "Streams",
		   m_thinnedOutStreamNames,
		   "The names of output stream(s) we want to apply thinning on."
		   " If empty, then thinning will be applied on all streams "
		   "found during the job configuration." );

  // defaults:
  {
    std::vector<std::string> defaultValue(0);
    m_thinnedOutStreamNames.set( defaultValue );
  }

  // for SvcHandle ? someday ?
  //declareInterface<IThinningSvc>(this);
}

// Destructor
///////////////
ThinningSvc::~ThinningSvc()
{ 
  cleanupStore();
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinningSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // Set to be listener for beginning of event
  IIncidentSvc* incSvc = 0;
  if ( !service("IncidentSvc",incSvc).isSuccess() ||
       0 == incSvc ) {
    ATH_MSG_ERROR ("Unable to get the IncidentSvc");
    return StatusCode::FAILURE;
  }
  incSvc->addListener( this, IncidentType::BeginEvent  );

  // retrieve pointer to @c StoreGateSvc
  if ( !m_storeGate.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Could not retrieve pointer to StoreGateSvc !!");
    return StatusCode::FAILURE;
  }

  
  // display configuration
  {
    std::ostringstream streams;
    if ( m_thinnedOutStreamNames.value().empty() ) {
      streams << "<none> ";
    } else {
      const std::set<std::string> streamset
	( m_thinnedOutStreamNames.value().begin(),
	  m_thinnedOutStreamNames.value().end() );
      std::copy( streamset.begin(), streamset.end(),
		 std::ostream_iterator<std::string>( streams, " " ) );
    }
    ATH_MSG_INFO ("Thinning streams: [ " << streams.str() << "]");
  }
  if ( m_thinnedOutStreamNames.value().empty() ) {
    ATH_MSG_WARNING
      ("No stream labeled as to be thinned ! Check your configuration !");
  }

  return StatusCode::SUCCESS;
}

StatusCode ThinningSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  cleanupStore();
  SlimmingStore_t().swap (m_slimmingStore);
  ThinningStore_t().swap (m_thinningStore);
  HandlerSet_t().swap (m_ownedHandlers);
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
ThinningSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IThinningSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IThinningSvc*>(this);

  } else if ( IProxyDict::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IProxyDict*>(this);

  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

SG::DataProxy* 
ThinningSvc::proxy( const void* const pTransient ) const
{ return m_storeGate->proxy( pTransient ); }

SG::DataProxy*  
ThinningSvc::proxy( const CLID& id, const std::string& key ) const
{ return m_storeGate->proxy( id, key ); }

SG::DataProxy*
ThinningSvc::proxy_exact (SG::sgkey_t sgkey) const
{ return m_storeGate->proxy_exact( sgkey ); }

std::vector<const SG::DataProxy*> 
ThinningSvc::proxies() const
{ return m_storeGate->proxies(); }

StatusCode ThinningSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  return m_storeGate->addToStore (id, proxy);
}


/**
 * @brief Record an object in the store.
 * @param obj The data object to store.
 * @param key The key as which it should be stored.
 * @param allowMods If false, the object will be recorded as const.
 * @param returnExisting If true, return proxy if this key already exists.
 *
 * Full-blown record.  @c obj should usually be something
 * deriving from @c SG::DataBucket.
 *
 * Returns the proxy for the recorded object; nullptr on failure.
 * If the requested CLID/key combination already exists in the store,
 * the behavior is controlled by @c returnExisting.  If true, then
 * the existing proxy is returned; otherwise, nullptr is returned.
 * In either case, @c obj is destroyed.
 */
SG::DataProxy*
ThinningSvc::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                           const std::string& key,
                           bool allowMods,
                           bool returnExisting)
{
  return m_storeGate->recordObject (obj, key, allowMods, returnExisting);
}


/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 */
StatusCode ThinningSvc::updatedObject (CLID id, const std::string& key)
{
  return m_storeGate->updatedObject (id, key);
}


/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

std::size_t 
ThinningSvc::index_impl( const SG::DataProxy* cproxy,
                         std::size_t idx ) const
{
  if ( 0 == cproxy ) {
    // this is now demoted to ::DEBUG as it may very well happen (even in
    // sound cases) that e.g. an ElementLink has no proxy (e.g. it points to
    // another file and BackNavigation was disabled)
    ATH_MSG_DEBUG ("Received a null DataProxy. Probably would have required "
		   "BackNavigation (but irrelevant to thinning)");
    return idx;
  }

  SG::DataProxy *proxy = const_cast<SG::DataProxy*>(cproxy);

  const bool dbg = msgLvl(MSG::VERBOSE);
  if ( dbg ) {
    msg(MSG::VERBOSE)
      << "index_impl(" << proxy->name() 
      << ", clid=[" << proxy->clID() << "]"
      << ", idx="
      << idx << ") -> [";
  }

  if ( !m_thinningOccurred ) {
    if ( dbg ) {
      msg(MSG::VERBOSE)
	<< idx << "] (no-thinning-occured)"
	<< endreq;
    }
    return idx;
  }

  ThinningStore_t::const_iterator iEntry = m_thinningStore.find( proxy );
  if ( iEntry == m_thinningStore.end() ) {
    // no such object has been registered: not thinned
    // old index holds
    if ( dbg ) {
      msg(MSG::VERBOSE)
	<< idx << "] (obj-not-thinned)"
	<< endreq;
    }
    return idx;
  }

  const IndexMap_t& indexMap = iEntry->second.get<2>();
  IndexMap_t::const_iterator i = indexMap.find( idx );
  if ( i == indexMap.end() ) {
    // element has been thinned away...
    if ( dbg ) {
      msg(MSG::VERBOSE) << "<removed>" << "]" << endreq;
    }
    return IThinningSvc::RemovedIdx;
  }

  if ( dbg ) {
    if (i->second != IThinningSvc::RemovedIdx) {
      msg(MSG::VERBOSE) << i->second << "] (thinned)";
    } else {
      msg(MSG::VERBOSE) << "<removed>]";
    }
    msg(MSG::VERBOSE) << endreq;
  }
  return i->second;
}

/** @brief test if a container is thinned
 */
bool 
ThinningSvc::is_thinned_impl(const SG::DataProxy* p) const
{
  // no thinning in that event occurred: return early
  if ( !m_thinningOccurred ) {
    return false;
  }

  if (0 == p) {
    // this is now demoted to ::DEBUG as it may very well happen (even in
    // sound cases) that e.g. an ElementLink has no proxy (e.g. it points to
    // another file and BackNavigation was disabled)
    ATH_MSG_DEBUG ("Received a null DataProxy. Probably would have required "
		   "BackNavigation (but irrelevant to thinning)");
    return false;
  }
  
  SG::DataProxy *proxy = const_cast<SG::DataProxy*>(p);
  return m_thinningStore.find( proxy ) != m_thinningStore.end();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void ThinningSvc::handle( const Incident& inc )
{
  ATH_MSG_DEBUG ("Entering handle(): "   << endreq
		 << "  Incidence type: " << inc.type()   << endreq
		 << "            from: " << inc.source());
  // Only clearing the store for BeginEvent incident
  if ( inc.type() == IncidentType::BeginEvent ) {
    cleanupStore();
    ATH_MSG_DEBUG ("[" << IncidentType::BeginEvent << "] handled");
    return;
  }

  return;
}

StatusCode 
ThinningSvc::commit()
{
  bool allOk = true;

  ATH_MSG_DEBUG 
    ("applying registered slimmers (" << m_slimmingStore.size() << ")...");
  for ( SlimmingStore_t::iterator 
	  iStore    = m_slimmingStore.begin(), 
	  iStoreEnd = m_slimmingStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    iStore->second->commit();
  }
  ATH_MSG_DEBUG
    ("applying registered slimmers (" << m_slimmingStore.size() << ")...[ok]");

  for ( ThinningStore_t::iterator 
	  iStore    = m_thinningStore.begin(), 
	  iStoreEnd = m_thinningStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    if ( !this->commit( iStore, Policy::Delete ).isSuccess() ) {
      allOk = false;
    }
  }
  return allOk ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode 
ThinningSvc::commit( ThinningStore_t::iterator iStore, 
		     ThinningSvc::Policy::Type deletePolicy )
{
  ATH_MSG_VERBOSE ("commit(iStore, "
		   << ((deletePolicy==Policy::Delete) ? "delete" : "no del.")
		   << ")");

  Filter_t&      keep    = iStore->second.get<0>();
  IThinningHdlr* handler = iStore->second.get<1>();
  IndexMap_t& indexMap   = iStore->second.get<2>();
  const bool isMapping = handler->isMapping();
  std::size_t thinnedIdx = 0;
  for ( Filter_t::const_iterator i = keep.begin(), iEnd = keep.end(); 
	i!= iEnd; 
	++i ) {
    std::size_t idx = i->first;
    bool do_keep = i->second;
    if ( !do_keep ) {
      indexMap[idx] = IThinningSvc::RemovedIdx;
      if ( deletePolicy == Policy::Delete ) { 
	handler->remove(idx); 
      }
    } else {
      // if the underlying container uses a mapping-like indexing
      // we don't want to screw its indexing: return the original index
      indexMap[idx] = isMapping ? idx : thinnedIdx++;
    }
  }
  if ( deletePolicy == Policy::Delete ) {
    // we removed all thinned-elements: now we can pack the vector 
    // (and hope for the best...)
    handler->commit();
  }
  return StatusCode::SUCCESS;
}

/** @brief Rollback the thinning actions:
 *   - restore the requested to-be-thinned elements
 */
StatusCode 
ThinningSvc::rollback()
{
  ATH_MSG_VERBOSE ("rollback()");

  bool allOk = true;

  // -- thinning first --------------------------------------------------------
  ATH_MSG_DEBUG ("rolling-back thinning (" <<m_thinningStore.size()<< ")...");
  for ( ThinningStore_t::iterator 
	  iStore    = m_thinningStore.begin(), 
	  iStoreEnd = m_thinningStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    IThinningHdlr* handler = iStore->second.get<1>();
    if ( handler ) {
      handler->rollback();
    } else {
      allOk = false;
    }
  }
  if ( !allOk ) {
    ATH_MSG_WARNING ("some problem occurred while rolling-back thinning !" 
		     << endreq
		     << " (trying nonetheless to revert slimming)...");
  } else {
    ATH_MSG_DEBUG 
      ("rolling-back thinning (" << m_thinningStore.size() << ")... [done]");
  }

  // -- then slimming ---------------------------------------------------------
  ATH_MSG_DEBUG("rolling-back slimmers (" << m_slimmingStore.size() << ")...");
  for ( SlimmingStore_t::iterator 
	  iStore    = m_slimmingStore.begin(), 
	  iStoreEnd = m_slimmingStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    ISlimmingHdlr* handler = iStore->second;
    if ( handler ) {
      handler->rollback();
    } else {
      allOk = false;
    }
  }
  if ( !allOk ) {
    ATH_MSG_WARNING ("some problem occurred while rolling-back slimming !" 
		     << endreq
		     << " (trying nonetheless to revert slimming)...");
  } else {
    ATH_MSG_DEBUG
      ("rolling-back slimmers (" << m_slimmingStore.size() << ")... [done]");
  }

  return allOk ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

IThinningHdlr* 
ThinningSvc::handler( SG::DataProxy* proxy )
{
  ThinningStore_t::iterator i = m_thinningStore.find( proxy );
  if ( i == m_thinningStore.end() ) {
    return 0;
  }
  return i->second.get<1>();
}

StatusCode
ThinningSvc::filter_impl( IThinningHdlr* handler, SG::DataProxy* proxy, 
                          const Filter_t& filter,
                          const IThinningSvc::Operator::Type op )
{
  if ( 0 == proxy ) {
    msg(MSG::WARNING)
      << "Received a null DataProxy ! "
      << "No thinning will occur for that 'object' !"
      << endreq;
    if (m_ownedHandlers.find(handler) == m_ownedHandlers.end())
      delete handler;
    return StatusCode::RECOVERABLE;
  }

  ThinningStore_t::iterator i = m_thinningStore.find( proxy );
  if ( i == m_thinningStore.end() ) {
    m_thinningStore[proxy] = boost::make_tuple( filter, 
                                                handler,
                                                ThinningSvc::IndexMap_t() );
    m_ownedHandlers.insert (handler);
    i = m_thinningStore.find( proxy );
  } else {
    if (m_ownedHandlers.find(handler) == m_ownedHandlers.end())
      delete handler;

    typedef Filter_t::const_iterator Iter_t;
    Filter_t& m = i->second.get<0>();
    if ( op == IThinningSvc::Operator::And ) {
      for ( Iter_t itr = filter.begin(), iEnd = filter.end();
	    itr != iEnd;
	    ++itr ) {
        Filter_t::iterator mitr = m.find (itr->first);
	if (mitr==m.end()) {
	  ATH_MSG_ERROR ("could not apply filter for proxy [" << proxy->name()
			 << "]"
			 << endreq
			 << "indices do not match !");
	  return StatusCode::FAILURE;
	}
        mitr->second = mitr->second && itr->second;
      }
    } else if ( op == IThinningSvc::Operator::Or ) {
      for ( Iter_t itr = filter.begin(), iEnd = filter.end();
	    itr != iEnd;
	    ++itr ) {
        Filter_t::iterator mitr = m.find (itr->first);
	if (mitr==m.end()) {
	  ATH_MSG_ERROR ("could not apply filter for proxy [" << proxy->name()
			 << "]"
			 << endreq
			 << "indices do not match !");
	  return StatusCode::FAILURE;
	}
        mitr->second = mitr->second || itr->second;
      }
    } else {
      msg(MSG::ERROR)
        << "Unknown operator being passed for merging of filters !!"
        << endreq;
      return StatusCode::FAILURE;
    }
  }

  m_thinningOccurred = true;
  return this->commit( i, Policy::DontDelete );
}

/** Helper method to clean-up thinning handlers and store
 */
void 
ThinningSvc::cleanupStore()
{
  // cleanup slimming store
  for ( SlimmingStore_t::iterator
	  iStore    = m_slimmingStore.begin(),
	  iStoreEnd = m_slimmingStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    delete iStore->second; iStore->second = 0;
  }
  m_slimmingStore.clear();

  // cleanup thinning store
  for ( ThinningStore_t::iterator 
	  iStore    = m_thinningStore.begin(),
	  iStoreEnd = m_thinningStore.end();
	iStore != iStoreEnd;
	++iStore ) {
    IThinningHdlr* hdlr = iStore->second.get<1>();
    delete hdlr; hdlr = 0;
  }
  m_thinningStore.clear();
  m_ownedHandlers.clear();
  m_thinningOccurred = false;
}

/** @brief register a slimming handler with the @c IThinningSvc
 *  Registering a @c ISlimmingHdlr allows to slim an object and keep the
 *  side effects of slimming it 'guarded' within one output stream.
 *  The original state (before slimming) of the object will be restored after
 *  to-disk serialization took place.
 *  Note: @c IThinningSvc will take ownership of the handler.
 */
StatusCode 
ThinningSvc::register_slimmer (ISlimmingHdlr *handler)
{
  if (0==handler) {
    msg (MSG::ERROR)
      << "received null pointer to ISlimmingHdlr !"
      << endreq;
    return StatusCode::FAILURE;
  }

  SlimmedObj_t obj = handler->object();
  SlimmingStore_t::iterator itr = m_slimmingStore.find (obj);
  if ( itr != m_slimmingStore.end() ) {
    msg (MSG::ERROR)
      << "A slimmer was already registered for object at [" << obj << "] of "
      << "type [" << System::typeinfoName (handler->type_id()) << "] !"
      << endreq
      << "Previous slimmer was registered by ["
      << itr->second->requester()->name() << "]"
      << endreq
      << "Current slimmer is being registered by ["
      << handler->requester()->name() << "]"
      << endreq
      << "sorry: multi-pass slimming isn't supported !"
      << endreq
      << " --- now deleting your slim-handler ---"
      << endreq;
    delete handler; handler = 0;
    return StatusCode::FAILURE;
  }

  // note that we re-use this notifier so as to not overly complicate the API.
  // time will tell if that was a good idea...
  m_thinningOccurred = true;

  m_slimmingStore[obj] = handler;
  return StatusCode::SUCCESS;
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
SG::sgkey_t ThinningSvc::stringToKey (const std::string& str, CLID clid)
{
  return m_storeGate->stringToKey (str, clid);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* ThinningSvc::keyToString (sgkey_t key) const
{
  return m_storeGate->keyToString (key);
}


/**
 * @brief Find the string and CLID corresponding to a given key.
 * @param key The key to look up.
 * @param clid[out] The found CLID.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* ThinningSvc::keyToString (sgkey_t key,
                                             CLID& clid) const
{
  return m_storeGate->keyToString (key, clid);
}


/**
 * @brief Remember an additional mapping from key to string/CLID.
 * @param key The key to enter.
 * @param str The string to enter.
 * @param clid The CLID associated with the string.
 * @return True if successful; false if the @c key already
 *         corresponds to a different string.
 *
 * This registers an additional mapping from a key to a string;
 * it can be found later through @c lookup() on the string.
 * Logs an error if @c key already corresponds to a different string.
 */
void ThinningSvc::registerKey (sgkey_t key,
                               const std::string& str,
                               CLID clid)
{
  m_storeGate->registerKey (key, str, clid);
}

