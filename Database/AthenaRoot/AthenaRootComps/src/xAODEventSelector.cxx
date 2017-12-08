///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODEventSelector.cxx 
// Implementation file for class xAODEventSelector
// Author: Johannes Elmsheuser, Will Buttinger
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdint.h>

// ROOT includes
#include "TROOT.h"
#include "RootUtils/TBranchElementClang.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TFile.h"
#include "TKey.h"
#include "TLeaf.h"

#include "GaudiKernel/Property.h"

// Framework includes
//#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"

// StoreGate includes

#include "SGTools/BuiltinsClids.h"   // to make sure we have their clids
#include "SGTools/CLASS_DEF.h"
#include "SGTools/StlMapClids.h"     // to make sure we have their clids
#include "SGTools/StlVectorClids.h"  // to make sure we have their clids
#include "SGTools/TransientAddress.h"
#include "StoreGate/StoreGate.h" 
#include "SGTools/CLIDRegistry.h"

// EventInfo includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"

// Package includes
#include "xAODEventSelector.h"
#include "xAODBranchAddress.h"
#include "RootGlobalsRestore.h"

// xAOD include(s):
#include "xAODRootAccess/Init.h"

//#include "xAODRootAccess/TEvent.h"
//#include "AsgTools/SgTEvent.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include <map>


#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/IOStats.h"

namespace Athena {

/** @class xAODEventContext 
 *  event selector context ... just holds reference back to the selector
 */
class xAODEventContext : public ::IEvtSelector::Context
{
public:
   /// standard c-tor with initialization
  xAODEventContext(const xAODEventSelector* sel) : m_evtsel(sel) {}
  /// standard d-tor
  virtual ~xAODEventContext() {}
  /// identifier method required by IEvtSelector::Context
  virtual void* identifier() const { return (void*)(m_evtsel); }

  /// access to the container of files
  const std::vector<std::string>& files() const { return m_evtsel->m_inputCollectionsName.value(); }
  /// access to the current file
  TFile* file() const { return m_evtsel->m_tfile; }
  /// call to setFile on evtSel
  StatusCode setFile(const std::string& fname) { return const_cast<xAODEventSelector*>(m_evtsel)->setFile(fname); }
  /// access to the file iterator
  std::size_t fileIndex() const { return m_evtsel->m_collIdx; }
  /// access to the current event entry number
  int64_t entry() const { return m_evtsel->m_curEvt; }

private:
  /// reference to the hosting event selector instance
  const xAODEventSelector* m_evtsel;
};

xAODEventSelector::xAODEventSelector( const std::string& name,
                                                  ISvcLocator* svcLoc ) :
  AthService ( name,    svcLoc ),
  m_dataStore( "StoreGateSvc/StoreGateSvc", name ),
  m_imetaStore( "StoreGateSvc/InputMetaDataStore", name ),
  m_ometaStore( "StoreGateSvc/MetaDataStore", name ),
  m_clidsvc  ( "ClassIDSvc",   name ),
  m_dictsvc  ( "AthDictLoaderSvc", name ),
  m_incsvc   ( "IncidentSvc", name ),
  m_poolSvc  ( "PoolSvc" , name ),
  m_ppSvc    ( "ProxyProviderSvc" , name ),
  m_nbrEvts  ( 0 ),
  m_curEvt   ( 0 ),
  m_collIdx  ( 0 ),
  m_collEvts (   ),
  m_needReload (true),
  m_rootAddresses (),
  m_tevent(NULL),
  m_tfile(NULL),
  m_tevent_entries(0)
{
//Properties important to end user:
   declareProperty( "InputCollections", m_inputCollectionsName,"List of input (ROOT) file names" );
   declareProperty( "SkipEvents",m_skipEvts = 0,"Number of events to skip at the beginning" );
   declareProperty( "AccessMode", m_accessMode = -1, "-1 = use TEvent Default; 0 = BranchAccess; 1 = ClassAccess; 2 = AthenaAccess" );

   declareProperty( "FillEventInfo", m_fillEventInfo=false,"If True, will fill old EDM EventInfo with xAOD::EventInfo content, necessary for database reading (IOVDbSvc)");

   declareProperty( "PrintPerfStats", m_printPerfStats=false,"If True, at end of job will print the xAOD perf stats");

//Expert Properties:
  declareProperty( "EvtStore", m_dataStore,       "Store where to publish data");
  declareProperty( "ProxyProviderSvc" , m_ppSvc , "The ProxyProviderSvc that we should register ourself in and connect the EvtStore to");
  declareProperty( "InputMetaStore",m_imetaStore,  "Store where to publish (input) metadata");
  declareProperty( "MetaStore",m_ometaStore,       "Store where to publish (output) metadata");
  declareProperty( "TreeName",m_tupleName = "CollectionTree","Name of the TTree to load/read from input file(s)" );
  declareProperty( "MetaDataTreeName", m_metadataName = "MetaData","Name of the TTree to load/read metadata from input file(s)" );

  declareProperty( "ReadMetaDataWithPool", m_readMetadataWithPool=false, "If true, using POOL to read metadata, will ensure input file is registered with catalog");

#ifndef XAOD_ANALYSIS
  declareProperty( "BackNavigation", m_backNavigation=false, "Compability flag for RecExCommon");
  declareProperty( "CollectionType", m_collectionType="", "Compability flag for RecExCommon");
#endif

}

// Destructor
///////////////
xAODEventSelector::~xAODEventSelector()
{}

StatusCode xAODEventSelector::initialize()
{
  ATH_MSG_VERBOSE ("Enter xAODEventSelector initialization...");

  CHECK( m_clidsvc.retrieve() );
  CHECK( m_dictsvc.retrieve() );

  if ( m_tupleName.value().empty() ) {
    ATH_MSG_ERROR("You have to give a TTree name to read from the ROOT files !");
    return StatusCode::FAILURE;
  }

  const std::size_t nbrInputFiles = m_inputCollectionsName.value().size();
  if ( nbrInputFiles < 1 ) {
    ATH_MSG_ERROR("You need to give at least 1 input file !!" << endmsg
       << "(Got [" << nbrInputFiles << "] file instead !)");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Selector configured to read [" << nbrInputFiles << "] file(s)...");
  }
  
  ATH_MSG_DEBUG("Calling xAOD::Init...");
  xAOD::Init().ignore();
  //if using the AthROOTErrorHandlerSvc, need to initialize it once again to give back error handling control to svc
  if(serviceLocator()->existsService("AthROOTErrorHandlerSvc")) {
    ServiceHandle<IService> ehSvc("AthROOTErrorHandlerSvc",name());
    CHECK( ehSvc.retrieve() );
    CHECK( ehSvc->initialize() ); //gives back control to svc
    CHECK( ehSvc.release() );
  }
  switch(m_accessMode) {
    case -1: ATH_MSG_INFO("Using DEFAULT xAOD access mode (usually same as CLASS mode)"); break;
    case 0: ATH_MSG_INFO("Using BRANCH xAOD access mode"); break;
    case 1: ATH_MSG_INFO("Using CLASS xAOD access mode"); break;
    case 2: ATH_MSG_INFO("Using ATHENA xAOD access mode"); break;
  }
  if(m_accessMode != -1) {
   m_tevent = new xAOD::xAODTEvent(xAOD::TEvent::EAuxMode(m_accessMode)); //our special class inheriting from xAOD::TEvent
  } else {
   m_tevent = new xAOD::xAODTEvent(); //our special class inheriting from xAOD::TEvent
  }


  {
    // register this service for 'I/O' events
    ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
    if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return StatusCode::FAILURE;
    }
    if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
      return StatusCode::FAILURE;
    }
    // register input file's names with the I/O manager
    const std::vector<std::string>& incol = m_inputCollectionsName.value();
    bool allGood = true;
    for (std::size_t icol = 0, imax = incol.size(); icol < imax; icol++) {
      if (!iomgr->io_register(this, 
                              IIoComponentMgr::IoMode::READ, 
                              incol[icol]).isSuccess()) {
        ATH_MSG_FATAL("could not register [" << incol[icol] << "] for output !");
        allGood = false;
      } else {
        ATH_MSG_VERBOSE("io_register[" << this->name() << "](" << incol[icol] << ") [ok]");
      }
    }
    if (!allGood) {
      return StatusCode::FAILURE;
    }
  }

  if (!do_init_io().isSuccess()) {
    return StatusCode::FAILURE;
  }

  // retrieve event stores
  // this needs to happen *after* having initialized the i/o
  // as our branches (which need a valid m_ntuple pointer)
  // may be asked to be registered as we are a ProxyProvider.
  // retrieving the event store will poke the ProxyProviderSvc...
/*
  if ( !m_dataStore.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Could not retrieve [" << m_dataStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // ditto for (input) meta data store
  if (!m_imetaStore.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve [" << m_imetaStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // ditto for (output) meta data store
  if (!m_ometaStore.retrieve().isSuccess()) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_ometaStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }
*/

  //ensure the xAODCnvSvc is listed in the EventPersistencySvc
  ServiceHandle<IProperty> epSvc("EventPersistencySvc",name());
  

  std::vector<std::string> propVal;
  CHECK( Gaudi::Parsers::parse( propVal , epSvc->getProperty("CnvServices").toString() ) );
  bool foundSvc(false); bool foundPoolSvc(false);
  for(auto s : propVal) {
    if(s=="Athena::xAODCnvSvc") { foundSvc=true; }
    if(s=="AthenaPoolCnvSvc") { foundPoolSvc=true; } //only need this if in hybrid mode
  }
  if(!foundSvc) propVal.push_back("Athena::xAODCnvSvc");
  if(!foundPoolSvc && m_readMetadataWithPool) propVal.push_back("AthenaPoolCnvSvc");

  if(!foundSvc || (!foundPoolSvc && m_readMetadataWithPool)) {
    CHECK( epSvc->setProperty("CnvServices", Gaudi::Utils::toString( propVal ) ));
  }


  //we should also add ourself as a proxy provider
  CHECK( m_ppSvc.retrieve() );

  //ensure the MetaDataSvc is added as a provider first, if we are in hybrid mode
  if(m_readMetadataWithPool) {
    std::vector<std::string> propVal;
    CHECK( Gaudi::Parsers::parse( propVal , dynamic_cast<IProperty*>(&*m_ppSvc)->getProperty("ProviderNames").toString() ) );
    bool foundSvc(false);
    for(auto s : propVal) {
      if(s=="MetaDataSvc") { foundSvc=true; break; }
    }
    if(!foundSvc) {
      propVal.push_back("MetaDataSvc");
      CHECK( dynamic_cast<IProperty*>(&*m_ppSvc)->setProperty("ProviderNames", Gaudi::Utils::toString( propVal ) ));
    }
  }

  //now we add ourself as a provider
  m_ppSvc->addProvider( this );
  //trigger a reload of proxies in the storegate, which will poke the proxyprovidersvc
  //not actually needed
  //CHECK( m_dataStore->loadEventProxies() );

  
  //finally ensure the storegate has our proxy set in it
  //FIXME: this doesnt seem to allow multi storegates on the fly ???
  //m_dataStore->setProxyProviderSvc( &*m_ppSvc );

  CHECK( m_ppSvc.release() );


  //load the first file .. this is so metadata can be read even if no events present
  //checked above that there's at least one file
  CHECK( setFile(m_inputCollectionsName.value()[0]) );

  if(m_printPerfStats) xAOD::PerfStats::instance().start();


  return StatusCode::SUCCESS;
}

StatusCode xAODEventSelector::finalize()
{
  ATH_MSG_VERBOSE ("Finalize...");
  // FIXME: this should be tweaked/updated if/when a selection function
  //        or filtering predicate is applied (one day?)
  ATH_MSG_INFO ("Total events read: " << (m_nbrEvts - m_skipEvts));

  if(m_printPerfStats) {
    xAOD::PerfStats::instance().stop();
    xAOD::IOStats::instance().stats().Print();
  }
  

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
xAODEventSelector::queryInterface( const InterfaceID& riid, 
                                         void** ppvInterface )
{
  if ( IEvtSelector::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEvtSelector*>(this);
  } else if ( IEvtSelectorSeek::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEvtSelectorSeek*>(this);
  } else if ( IIoComponent::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IIoComponent*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode
xAODEventSelector::next( IEvtSelector::Context& ctx ) const
{
  // std::cout << "::next(fidx=" << m_collIdx << ", eidx=" << m_curEvt << ")"
  //           << std::endl;
  ATH_MSG_DEBUG ("next() : iEvt " << m_curEvt);

  // get evt context
  xAODEventContext* rctx = dynamic_cast<xAODEventContext*>(&ctx);
  if ( 0 == rctx ) {
    ATH_MSG_ERROR ("Could not dyn-cast to xAODEventContext !!");
    throw GaudiException("xAODEventSelector::next() - Unable to get xAODEventContext","xAODEventSelector",StatusCode::FAILURE);
  }
  

  TFile *file = rctx->file();
  if (!file) { //must be starting another file ...
    auto& fnames = rctx->files();
    //std::size_t fidx = rctx->fileIndex();
    if( rctx->setFile("").isFailure() ) {
      throw GaudiException("xAODEventSelector::next() - Fatal error when trying to setFile('')","xAODEventSelector",StatusCode::FAILURE);
    }

    while( m_tevent_entries == 0 ) { //iterate through files until we have one with entries
      if (m_collIdx < int(rctx->files().size())) {
         const std::string& fname = fnames[m_collIdx];
         if( rctx->setFile( fname ).isFailure() ) {
	   throw GaudiException("xAODEventSelector::next() - Fatal error when trying to setFile('" + fname + "')","xAODEventSelector",StatusCode::FAILURE);
	 }
         ATH_MSG_DEBUG("TEvent entries = " << m_tevent_entries);
      } else {
         // end of collections
	return StatusCode::FAILURE; //this is a valid failure ... athena will interpret as 'finished looping'
      }
      if( m_tevent_entries==0) m_collIdx++;
      
      
   } //end of while loop

  }


  ATH_MSG_DEBUG("m_curEvt=" << m_curEvt);
  
   //Infer the local entry (entry of current file) from the global entry

  int64_t global_entry = rctx->entry(); //the actual event counter
  int64_t entry = global_entry;
  if (m_collEvts[m_collIdx].min_entries < 0) {
    // need to trigger collmetadata...
    const_cast<xAODEventSelector*>(this)->find_coll_idx(entry);
  }
  // rctx::entry is the *global* entry number.
  // we need the local one...
  entry = global_entry - m_collEvts[m_collIdx].min_entries; //actual entry in the current file



  if ( m_tevent_entries > entry ) {

    // Load the event:
    if( m_tevent->getEntry( entry ) < 0 ) {
         ATH_MSG_ERROR( "Failed to load entry " << static_cast< int >( entry ) );
	 throw GaudiException("xAODEventSelector::next() - xAOD::TEvent::getEntry returned less than 0 bytes","xAODEventSelector",StatusCode::FAILURE);
    }

    ++m_nbrEvts;
    m_curEvt = global_entry + 1;

    // std::cout << "--event-info--" << std::endl;
    // event info

    // EventInfo is required, otherwise Athena will crash
    const xAOD::EventInfo* xaodEventInfo = 0;
    if(m_fillEventInfo) {
      if(m_tevent->retrieve( xaodEventInfo , "EventInfo").isFailure()) {
	ATH_MSG_ERROR("Could not find xAOD::EventInfo");
	throw GaudiException("xAODEventSelector::next() - Could not find xAOD::EventInfo","xAODEventSelector",StatusCode::FAILURE);
      }
    }
    EventType* evtType = new EventType;
    //const std::size_t runNbr = (xaodEventInfo) ? xaodEventInfo->runNumber() : 0;
    EventInfo* evtInfo = new EventInfo(
				       (xaodEventInfo) ? new EventID(xaodEventInfo->runNumber(), xaodEventInfo->eventNumber(), xaodEventInfo->timeStamp(), xaodEventInfo->timeStampNSOffset(), xaodEventInfo->lumiBlock(), xaodEventInfo->bcid()) : new EventID(0, m_curEvt-1,0 ), evtType);
    if ( !m_dataStore->record( evtInfo, "EventInfo" ).isSuccess() ) {
      ATH_MSG_ERROR ("Could not record EventInfo !");
      delete evtInfo; evtInfo = 0;
      throw GaudiException("xAODEventSelector::next() - Could not record EventInfo","xAODEventSelector",StatusCode::FAILURE);
    }

    return StatusCode::SUCCESS;

  } else {
    // file is depleted
    auto& fnames = rctx->files();
    std::size_t fidx = rctx->fileIndex();
    m_incsvc->fireIncident(FileIncident(name(), "EndInputFile", fnames[fidx]));

    // prepare for next file, if any...
    // std::cout << "=========================================================="
    //           << std::endl;
    // std::cerr << "::switch to next file...\n";

    // iterate over our "cached" transient addresses, 
    // marking them as garbage and dropping the RootBranchAddress (as a side effect of
    // ::setAddress(NULL).
    // this way, the next time we hit ::createRootBranchAddress or ::updateAddress
    // all internal states are kosher.
    for (auto& iaddr : self()->m_rootAddresses) {
      iaddr.second = false; // mark as invalid
      SG::TransientAddress* taddr = iaddr.first;
      taddr->setAddress(NULL);
    }
    const bool forceRemove = false;
    CHECK( m_dataStore->clearStore(forceRemove) ); //must clear the storegate so that any tampering user did in EndInputFile incident is cleared
    m_needReload = true;

    m_collIdx += 1;
    CHECK( rctx->setFile("") );
    return next(*rctx);
  }
  
  // NOT REACHED
  // std::cout << "***end of collections***" << std::endl;
  // end of collections
  //return StatusCode::FAILURE;
}

StatusCode xAODEventSelector::next( Context& ctx, int jump ) const
{
  ATH_MSG_DEBUG ("next(" << jump << ") : iEvt " << m_curEvt);

  if (self()->seek(ctx, m_curEvt + jump).isSuccess()) {
    return StatusCode::FAILURE;
  }
  return next(ctx);
}

StatusCode
xAODEventSelector::previous( IEvtSelector::Context& ctx ) const 
{
  return next( ctx, -1 );
}

StatusCode 
xAODEventSelector::previous( Context& ctx, int jump ) const 
{
  return next( ctx, -jump );
}

StatusCode
xAODEventSelector::last( Context& /*ctxt*/ ) const
{
  ATH_MSG_ERROR ("............. Last Event Not Implemented .............");
  return StatusCode::FAILURE;
}


StatusCode 
xAODEventSelector::rewind( Context& ctxt ) const 
{
  return self()->seek(ctxt, 0);
}

StatusCode
xAODEventSelector::createContext( Context*& refCtx ) const
{
  xAODEventContext *ctx = new xAODEventContext(this);
  refCtx = ctx;
  return StatusCode::SUCCESS;
}

StatusCode
xAODEventSelector::createAddress( const Context& /*refCtx*/, 
                                        IOpaqueAddress*& /*addr*/ ) const 
{
  //std::cerr << "::TTES::createAddress()...\n";
  return StatusCode::SUCCESS;
}

StatusCode
xAODEventSelector::releaseContext( Context*& refCtxt ) const
{
  if(refCtxt==0) return StatusCode::SUCCESS; //added to avoid warning from MetaDataSvc, which passes an empty context
  xAODEventContext *ctx  = dynamic_cast<xAODEventContext*>(refCtxt);
  if ( ctx )   {
    delete ctx; ctx = 0;
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode
xAODEventSelector::resetCriteria( const std::string&, Context& ) const 
{
  ATH_MSG_ERROR ("............. resetCriteria Not Implemented .............");
  return StatusCode::FAILURE;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/**
 * @brief Seek to a given event number.
 * @param evtnum  The event number to which to seek.
 */
StatusCode
xAODEventSelector::seek (Context& refCtxt, int evtnum) const
{
  // std::cout << "::seek - evtnum=" << evtnum 
  //           << " curevt=" << m_curEvt 
  //           << " curcol=" << m_collIdx
  //           << std::endl;
  long coll_idx = find_coll_idx(evtnum);
  // std::cout << "::seek - evtnum=" << evtnum 
  //           << " curevt=" << m_curEvt 
  //           << " curcol=" << m_collIdx
  //           << " colidx=" << coll_idx
  //           << std::endl;
  if (coll_idx == -1 && evtnum < m_curEvt) {
    coll_idx = m_collIdx;
  }

  if (coll_idx == -1) {
    ATH_MSG_INFO("seek: reached end of input.");
    return StatusCode::RECOVERABLE;
  }

  if (coll_idx != m_collIdx) {
    // tell everyone we switched files...
    xAODEventContext* rctx = dynamic_cast<xAODEventContext*>(&refCtxt);
    rctx->setFile("");
  }

  m_collIdx = coll_idx;
  m_curEvt = evtnum;

  return StatusCode::SUCCESS;
}

/**
 * @brief return the current event number.
 * @return The current event number.
 */
int 
xAODEventSelector::curEvent (const Context& /*refCtxt*/) const
{
  return m_curEvt;
}

/// Callback method to reinitialize the internal state of the component 
/// for I/O purposes (e.g. upon @c fork(2))
StatusCode 
xAODEventSelector::io_reinit()
{
  ATH_MSG_VERBOSE("I/O reinitialization...");

  ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
  if (!iomgr.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
    return StatusCode::FAILURE;
  }
  if (!iomgr->io_hasitem(this)) {
    ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
    return StatusCode::FAILURE;
  }
  std::vector<std::string> inputCollections = m_inputCollectionsName.value();

  for (std::size_t 
         i = 0, 
         imax = m_inputCollectionsName.value().size(); 
       i < imax; 
       ++i) {
    std::string &fname = inputCollections[i];
    // std::cout << "--retrieve new name for [" << fname << "]...\n";
    if (!iomgr->io_contains(this, fname)) {
      ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
      return StatusCode::FAILURE;
    }
    if (!iomgr->io_retrieve(this, fname).isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
      return StatusCode::FAILURE;
    }
    // std::cout << "--> [" << fname << "]\n" << std::flush;
  }
  // all good... copy over.
  m_inputCollectionsName = inputCollections;

  // remove our EventInfo if any...
  // {
  //   const bool force_remove = true;
  //   if (!m_dataStore->clearStore(force_remove).isSuccess()) {
  //     ATH_MSG_ERROR("could not clear event store!");
  //     return StatusCode::FAILURE;
  //   } else {
  //     ATH_MSG_INFO("sgdump: \n" << m_dataStore->dump()); 
  //   }
  // }

  // std::cout << "--> do_init_io...\n" << std::flush;
  if (!do_init_io().isSuccess()) {
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("I/O reinitialization... [done]");
  return StatusCode::SUCCESS;
}


///@{
/// @c IAddressProvider interface
///get all addresses from Provider : Called before Begin Event
StatusCode 
xAODEventSelector::preLoadAddresses(StoreID::type /*storeID*/, 
                                          tadList& /*tads*/)
{
  // std::cerr << "TTES::preLoadAddresses(" << int(storeID)
  // 	    << "," << tads.size()
  // 	    << ")...\n";
  return StatusCode::SUCCESS;
}
 
/// get all new addresses from Provider for this Event.
StatusCode 
xAODEventSelector::loadAddresses(StoreID::type storeID, tadList& tads)
{
  if (m_needReload || m_rootAddresses.empty()) {
    //CHECK(createMetaDataRootBranchAddresses()); 
    return createRootBranchAddresses(storeID, tads);
  }

  return StatusCode::SUCCESS;
}
 
/// update a transient Address
StatusCode 
xAODEventSelector::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad,
                                 const EventContext& /*ctx*/)
{
  // check if this tad is known to us.
  if (tad) {
    auto itr = m_rootAddresses.find(tad);
    if ( itr != m_rootAddresses.end() && itr->second ) {
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("updateAddress: address [" << tad->clID() << "#"
		  << tad->name() << ") NOT known to us.");
    return StatusCode::FAILURE;
  }  

  // do nothing. 
  return StatusCode::SUCCESS;
}
///@}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 




StatusCode 
xAODEventSelector::createRootBranchAddresses(StoreID::type storeID,
                                                   tadList &tads)
{
  if (storeID != StoreID::EVENT_STORE) {
    ATH_MSG_INFO("-- not the event-store --");
    return(StatusCode::SUCCESS);
  }

  if (0 == m_tfile) {
    ATH_MSG_ERROR("null pointer to n-tuple !");
    return StatusCode::FAILURE;
  }


  ATH_MSG_DEBUG("Reading xAOD::EventFormat");

  //  m_tevent->dump();

  const void* value_ptr = m_tevent; //passed as 'parameter' to the address object

  std::set<std::string> missingAux;

  for( auto itr = m_tevent->inputEventFormat()->begin(); itr!=m_tevent->inputEventFormat()->end();++itr) {
    //ATH_MSG_DEBUG("EFE:" << itr->first << " branchName = " << itr->second.branchName() << " className=" << itr->second.className());
      CLID id = 0;
      if( m_clidsvc->getIDOfTypeInfoName(itr->second.className(), id).isFailure() &&
	  m_clidsvc->getIDOfTypeName(itr->second.className(), id).isFailure()) {
	//if this is an AuxStore (infer if key ends in Aux.), its possible we schema-evolved away from the version in the input file, but that this evolution is actually 'ok' in some cases. So don't print an error if the CLID is missing for an Aux, but we will print a warning at the end for these aux stores
	if(itr->second.branchName().compare(itr->second.branchName().length()-4,4,"Aux.")==0) {
	  missingAux.insert( itr->second.className() );continue;
	} else {
	  //vectors can be missing their std:: prefix, so add that and retry before failing
	  TString className =  itr->second.className();
	  ///className.ReplaceAll("vector","std::vector");
	  //ALT solution to this is do what RootNtupleEventSelector does: uses TClass:GetClass
	  //and GetTypeInfo() method to get the proper type info
	  TClass *cls = TClass::GetClass(className);
	  if(cls) {
	    const std::type_info *ti = cls->GetTypeInfo();
	    if(ti) className = System::typeinfoName(*ti);
	  }
	  
	  if( m_clidsvc->getIDOfTypeInfoName(className.Data(), id).isFailure() &&
	      m_clidsvc->getIDOfTypeName(className.Data(), id).isFailure()) {
	    ATH_MSG_WARNING("No CLID for class " << itr->second.className() << " , cannot read " << itr->second.branchName());
	    continue;
	  }
	}
      }

      const std::string br_name = itr->second.branchName();

      Athena::xAODBranchAddress* addr = new Athena::xAODBranchAddress
        (POOL_ROOTTREE_StorageType, id, 
         m_tupleName.value(), 
         br_name, 
         (unsigned long)(value_ptr),
         (unsigned long)(0)); //IMPORTANT THIS IS 0: signals to xAODBranchAddress to read event-level info (see setTEventAddress)
      // recycle old rootaddress, if any.
      SG::TransientAddress* taddr = NULL;
      // FIXME: should we only iterate over m_rootAddresses which have been marked
      //        as invalid ? (ie: iaddr->second == false)
      //        probably not worth it... (but depends on the "occupancy")
      for (auto 
             iaddr = m_rootAddresses.begin(),
             iaddre= m_rootAddresses.end();
           iaddr != iaddre;
           ++iaddr) {
        SG::TransientAddress *old = iaddr->first;
        if (old->clID() == id &&
            old->name() == br_name) {
          // found a "cached" transient address which corresponds to this clid+key
          // bind it to our new RootBranchAddress...
          old->setAddress(addr);
          taddr = old;
          iaddr->second = true; // mark as valid
          break;
        }
      }
      if (taddr == NULL) {
        taddr = new SG::TransientAddress(id, br_name, addr);
        taddr->setProvider(this, storeID);
        taddr->clearAddress(false);
        // only add the *new* TransientAddress to the input list as the *old* ones
        // are already tracked by the datastore (via the sticky proxies)
        tads.push_back(taddr);
        // note: we can store this taddr *b/c* we don't clearAddress it
        // ie: b/c we just called clearAddress(false) so it will be recycled
        // over the events.
        m_rootAddresses.insert(std::make_pair(taddr, true));
      }
      // }
  }

  if(missingAux.size()) {
    std::string allAux; for(auto& s : missingAux) allAux += s + ", ";
    ATH_MSG_WARNING("The following AuxStore types are not directly accessible (missing CLID, possibly from schema evolution): " << allAux);
  }
  

  m_needReload = false;
  // remember that we need to fire a BeginInputFile incident.
  // we can't fire it just now as some client may need the tree and its
  // content loaded in the evtstore when their ::handle method is
  // called.
  // so we do it later.
  //MOVED TO handle method - which is fired on BeginEvent after StoreGateSvc
  //m_fireBIF = true;

  ATH_MSG_DEBUG("In xAODEventSelector::createRootBranchAddresses end ...");

  return StatusCode::SUCCESS;
}

StatusCode 
xAODEventSelector::createMetaDataRootBranchAddresses() const
{

  ATH_MSG_DEBUG("In xAODEventSelector::createMetaDataRootBranchAddresses start ...");
  
  if ( 0 == m_tfile ) {
    ATH_MSG_ERROR ("Could not get m_tfile !!");
    throw "xAODEventSelector: Unable to get m_tfile";
  }

  //FIXME JE
  ATH_MSG_DEBUG("m_metadataName.value().c_str() = " << m_metadataName.value().c_str() );
  TTree* tree = dynamic_cast<TTree*>(m_tfile->Get(m_metadataName.value().c_str()));
  ATH_MSG_DEBUG("m_tfile = " << m_tfile );
  ATH_MSG_DEBUG("tree = " << tree );
  if (!tree) std::abort();
  TObjArray *leaves = tree->GetListOfLeaves();
  if (!leaves) {
    ATH_MSG_INFO("no leaves!!");
    return StatusCode::SUCCESS;
  }
  
  // loop over leaves
  for (Int_t i = 0; i < leaves->GetEntries(); ++i) {
    TLeaf *leaf = (TLeaf *)leaves->At(i);
    TBranch *branch = leaf->GetBranch();
    if (branch) {
      
      CLID id = 0;
      const void* value_ptr = m_tevent;
      const std::string type_name = leaf->GetTypeName();
      const std::string br_name = branch->GetName();
      const std::string sg_key = br_name;//m_tupleName.value()+"/"+br_name;
      TClass *cls = TClass::GetClass(type_name.c_str());
      const std::type_info *ti = 0;

      // Skip the EventFormat branch. That must not be disturbed by the
      // generic metadata handling.
      if( br_name == "EventFormat" ) continue;

      if (cls) {
        ti = cls->GetTypeInfo();
        // first, try to load a dict for that class...
        if (ti) {
          m_dictsvc->load_type(*ti);
        }
        if (!ti) {
          ATH_MSG_DEBUG("could not find a type-info for [" << 
                          type_name << "]");
          continue;
        }

	// Find the clid for the typeInfo
	id = CLIDRegistry::typeinfoToCLID(*ti);

	if (id == 0) {
	  ATH_MSG_DEBUG("** could not find a CLID for type-name ["
			<< type_name << "]");
	  continue;
	}

	// Skip if type_name does contain xAOD, ie. is not an xAOD container
	const std::string toCheck = "xAOD::";
	if (type_name.find(toCheck) == std::string::npos) {
	  ATH_MSG_DEBUG("** Skip type-name = " << type_name << ", br_name = " << br_name );
	  continue;
	}

	ATH_MSG_DEBUG("id = " << id << ", m_metadataName.value() = " << m_metadataName.value() << ", br_name = " << br_name << ", value_ptr = " << value_ptr);
	Athena::xAODBranchAddress* addr = new Athena::xAODBranchAddress
	  (POOL_ROOTTREE_StorageType, id, 
	   m_metadataName.value(), 
	   br_name, 
	   (unsigned long)(value_ptr),
	   (unsigned long)(1)); //IMPORTANT THIS IS 1: signals to BranchAddress to read metadata
	if (!m_imetaStore->recordAddress(br_name, addr, true).isSuccess()) {
	  ATH_MSG_ERROR("could not record address at [" << br_name << "] in store ["
			<< m_imetaStore->name() << "]");
	  delete addr; addr = 0;
	}
	// SG::TransientAddress* taddr = new SG::TransientAddress
	//   (id, sg_key, addr);
	// taddr->setProvider(this);
	// taddr->clearAddress(true);
	// tads.push_back(taddr);
      }
    }
  }
  
  ATH_MSG_DEBUG("In xAODEventSelector::createMetaDataRootBranchAddresses end ...");
  
  return StatusCode::SUCCESS;
}


TFile*
xAODEventSelector::fetchNtupleFile(const std::string& fname) const
{
  TFile* file = NULL;
  if(fname.empty()) return file; //if blank, return 0
  RootGlobalsRestore rgr;
  //see if file already open
  file = (TFile*)gROOT->GetListOfFiles()->FindObject(fname.c_str());
  if (!file) {
    //open the file
    file = TFile::Open(fname.c_str(), "READ");
    if (file) file->SetName(fname.c_str());
  }
  //check file is ok before returning
  if (!file || file->IsZombie()) {
    ATH_MSG_ERROR("could not open next file in input collection ["
                  << fname << "]");
    if (file) {
      file->Close();
    }
    return 0;
  }
  return file;
}

//move onto given file
StatusCode xAODEventSelector::setFile(const std::string& fname) {

  TFile* newFile = fetchNtupleFile(fname);
  if(!newFile && !fname.empty()) {
      ATH_MSG_FATAL( "xAODEventSelector: Unable to fetch Ntuple: " << fname);
      return StatusCode::FAILURE; //failed to load file
  }

  if(m_tfile && m_tfile != newFile) {
    const std::string currFile = m_tfile->GetName();
    //disconnect pool if necessary
    if(m_readMetadataWithPool) m_poolSvc->disconnectDb("PFN:"+currFile).ignore();
    //close existing file
    m_tfile->Close();
    //we should also cleanup after pool, in case it has left open files dangling
  }
  m_tfile = newFile;
  m_tevent_entries = 0; //will set in a moment

  if(!m_tfile) return StatusCode::SUCCESS; //must have been setting to blank file

  if(m_tevent->readFrom( m_tfile ).isFailure()) {
      ATH_MSG_FATAL( "xAODEventSelector: TEvent cannot read " << fname);
      return StatusCode::FAILURE;
  }

  m_tevent_entries = m_tevent->getEntries();

  //need to load metadata for file
   if (!m_imetaStore->clearStore().isSuccess()) {
      ATH_MSG_INFO("could not clear store [" << m_imetaStore.typeAndName() << "]");
      return StatusCode::FAILURE;
   }

   
  if(m_readMetadataWithPool) {
      //ensure input file collection created
      ATH_MSG_DEBUG("Creating poolsvc collection for " << fname);
      m_poolSvc->createCollection( "ImplicitCollection" , "PFN:"+fname , fname ); //FIXME: this throws exceptions which get in the way of debugging with gdb :-(

      //metadata will be read by MetaDataSvc, triggered by the BeginInputFile call
   } else {
      if (!createMetaDataRootBranchAddresses().isSuccess()) {
         ATH_MSG_ERROR("Could not load metadata");
      }
   }

  //must trigger a beginInputFile event here
  m_incsvc->fireIncident(FileIncident(name(), "BeginInputFile", m_tfile->GetName())); 

  return StatusCode::SUCCESS;

}


StatusCode
xAODEventSelector::do_init_io()
{
  // std::cout << "::fetchNtuple..." << std::endl;

  // initialize some helper structures and data
  {
    CollMetaData zero; 
    zero.min_entries = -1;
    zero.max_entries = -1;
    m_collEvts.resize(m_inputCollectionsName.value().size(), zero);
    m_collIdx = 0;
  }



  //CHECK( setFile( m_inputCollectionsName.value()[m_collIdx] ) );


  // std::cout << "::clear-root-addresses..." << std::endl;
  // reset the list of branches
  //m_rootAddresses.clear();
  m_needReload = true;

  // skip events we are asked to skip
  m_curEvt  = m_skipEvts;
  m_nbrEvts = 0;

  // std::cout << "::fetchNtuple...[done]" << std::endl;
  return StatusCode::SUCCESS;
}

/// helper method to get the collection index (into `m_inputCollectionsName`)
/// for a given event index `evtidx`.
/// returns -1 if not found.
int 
xAODEventSelector::find_coll_idx(int evtidx) const
{
  // std::cout << "--find_coll_idx(" << evtidx << ")..." << std::endl
  //           << "--collsize: " << m_collEvts.size() << std::endl;
  for (std::size_t i = 0, imax = m_collEvts.size();
       i < imax;
       ++i) {
    // std::cout << "--[" << i << "]...\n";
    CollMetaData &itr = m_collEvts[i];
    if (itr.min_entries == -1) {
      TFile *file = fetchNtupleFile(m_inputCollectionsName.value()[i]);
      if (file) {
        long offset = 0;
        if (i > 0) {
          CollMetaData &jtr = m_collEvts[i-1];
          offset += jtr.max_entries;
        }
        itr.min_entries = offset;
        TTree* tree = dynamic_cast<TTree*>(file->Get(m_tupleName.value().c_str()));
        itr.max_entries = offset + ( (tree) ? tree->GetEntriesFast() : 0);
      } else {
	throw "xAODEventSelector: Unable to fetch ntuple";
      }
    }
    // std::cout << "--[" << i << "] => [" << itr.min_entries << ", "
    //           << itr.max_entries << ") evtidx=[" << evtidx << "]"
    //           << std::endl;
    if (itr.min_entries <= evtidx && evtidx < itr.max_entries) {
      return i;
    }
  }
  return -1;
}


int xAODEventSelector::size (Context& /*refCtxt*/) const {
   //use find_coll_idx to trigger a population of the m_collEvts ... dummy call with -1 to trigger all colls loaded
   find_coll_idx(-1);
   return m_collEvts.back().max_entries;
}

//not used for anything
void xAODEventSelector::handle(const Incident& /*incident*/) {

}

} //> namespace Athena
