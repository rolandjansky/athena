/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbSvc.cxx
// Re-implementation of Athena IOVDbSvc
// Richard Hawkijngs, started 23/11/08
// based on earlier code by RD Schaffer, Antoine Perus and RH

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreClearedIncident.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Guards.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "AthenaKernel/IOVRange.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "AthenaKernel/IAddressProvider.h"
#include "FileCatalog/IFileCatalog.h"
#include "EventInfo/TagInfo.h"
#include "EventInfoUtils/EventIDFromStore.h"

#include "IOVDbParser.h"
#include "IOVDbFolder.h"
#include "IOVDbSvc.h"

#include <list>
#include <algorithm>

// helper function for getting jobopt properties
namespace {
  bool
  refersToConditionsFolder(const TagInfo::NameTagPair & thisPair){
    return thisPair.first.front() == '/';
  }

} // anonymous namespace


IOVDbSvc::~IOVDbSvc() {}

/// Identify interfaces to which this service is responsive
StatusCode
IOVDbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IIOVDbSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IIOVDbSvc*)this;
  } else if  ( IIOVCondDbSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IIOVCondDbSvc*)this;
  } else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

int IOVDbSvc::poolSvcContext()
{
   if( m_poolSvcContext < 0 ) {
      // Get context for POOL conditions files, and created an initial connection
      if (m_par_managePoolConnections) {
         m_poolSvcContext=m_h_poolSvc->getInputContext("Conditions", m_par_maxNumPoolFiles);
      } else {
         m_poolSvcContext=m_h_poolSvc->getInputContext("Conditions");
      }
      if( m_h_poolSvc->connect(pool::ITransaction::READ, m_poolSvcContext).isSuccess() ) {
         ATH_MSG_INFO( "Opened read transaction for POOL PersistencySvc");
      } else {
         // We only emit info for failure to connect (for the moment? RDS 01/2008)
         ATH_MSG_INFO( "Cannot connect to POOL PersistencySvc" );
      }
   }
   return m_poolSvcContext;
}

StatusCode IOVDbSvc::initialize() {
  if (StatusCode::SUCCESS!=AthService::initialize()) return StatusCode::FAILURE;
  // subscribe to events
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  if (StatusCode::SUCCESS!=incSvc.retrieve()) {
    ATH_MSG_ERROR( "Unable to get the IncidentSvc" );
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incSvc->addListener( this, "BeginEvent", pri );
  incSvc->addListener( this, "StoreCleared", pri );

  // Register this service for 'I/O' events
  ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
  if (!iomgr.retrieve().isSuccess()) {
     ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
     return(StatusCode::FAILURE);
  }
  if (!iomgr->io_register(this).isSuccess()) {
     ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
     return(StatusCode::FAILURE);
  }
  // print warnings/info depending on state of job options
  if (!m_par_manageConnections)
    ATH_MSG_INFO( "COOL connection management disabled - connections kept open throughout job" );
  if (!m_par_managePoolConnections)
    ATH_MSG_INFO( "POOL file connection management disabled - files kept open throught job" );
  if (m_par_maxNumPoolFiles.value() > 0)
    ATH_MSG_INFO( "Only " << m_par_maxNumPoolFiles.value() <<  " POOL conditions files will be open at once" );
  if (m_par_forceRunNumber.value() > 0 || m_par_forceLumiblockNumber.value() > 0)
    ATH_MSG_WARNING( "Global run/LB number forced to be [" <<
      m_par_forceRunNumber.value() << "," << m_par_forceLumiblockNumber.value() <<  "]" );
  if (m_par_forceTimestamp.value() > 0) 
    ATH_MSG_WARNING( "Global timestamp forced to be " <<
      m_par_forceTimestamp.value() );
  if (m_par_cacheRun.value() > 0) 
    ATH_MSG_INFO( "Run-LB data will be cached in groups of " << 
      m_par_cacheRun.value() << " runs" );
  if (m_par_cacheTime.value() > 0)
    ATH_MSG_INFO( "Timestamp data will be cached in groups of " << m_par_cacheTime.value() << " seconds" );
  if (m_par_cacheAlign > 0) 
    ATH_MSG_INFO( "Cache alignment will be done in " << m_par_cacheAlign.value() << " slices" );
  if (m_par_onlineMode) 
    ATH_MSG_INFO(  "Online mode ignoring potential missing channels outside cache" );
  if (m_par_checklock)
    ATH_MSG_INFO( "Tags will be required to be locked");

  // make sure iovTime is undefined
  m_iovTime.reset();

  // extract information from EventSelector for run/LB/time overrides
  if (StatusCode::SUCCESS!=checkEventSel()) return StatusCode::FAILURE;

  // initialise default connection
  if (m_par_defaultConnection!="") {
    // default connection is readonly if no : in name (i.e. logical conn)
    bool readonly=(m_par_defaultConnection.value().find(':')==std::string::npos);
    m_connections.push_back(new IOVDbConn(m_par_defaultConnection,readonly,msg()));
  }

  // set time of timestampslop in nanoseconds
  m_iovslop=static_cast<cool::ValidityKey>(m_par_timeStampSlop*1.E9);

  // check for global tag in jobopt, which will override anything in input file
  if (m_par_globalTag!="") {
    m_globalTag=m_par_globalTag;
    ATH_MSG_INFO( "Global tag: " << m_par_globalTag.value() << " set from joboptions" );
  }

  // setup folders and process tag overrides
  if (StatusCode::SUCCESS!=setupFolders()) return StatusCode::FAILURE;

  // Set state to initialize
  m_state=IOVDbSvc::INITIALIZATION;
  ATH_MSG_INFO( "Initialised with " << m_connections.size() << 
    " connections and " << m_foldermap.size() << " folders" );
  if (m_outputToFile.value()) ATH_MSG_INFO("Db dump to file activated");
  ATH_MSG_INFO( "Service IOVDbSvc initialised successfully" );
  return StatusCode::SUCCESS;
}


StatusCode IOVDbSvc::io_reinit() {
   ATH_MSG_DEBUG("I/O reinitialization...");
   // PoolSvc clears all connections on IO_reinit - forget the stored contextId
   m_poolSvcContext = -1;
   return(StatusCode::SUCCESS);
}

StatusCode IOVDbSvc::io_finalize() {
   ATH_MSG_DEBUG("I/O finalization...");
   return(StatusCode::SUCCESS);
}

StatusCode IOVDbSvc::finalize() {
  // summarise and delete folders, adding total read from COOL
  unsigned long long nread=0;
  float readtime=0.;
  // accumulate a map of readtime by connection
  typedef std::map<IOVDbConn*,float> CTMap;
  CTMap ctmap;
  for (const auto & namePtrPair : m_foldermap) {
    IOVDbFolder* folder=namePtrPair.second;
    folder->summary();
    nread+=folder->bytesRead();
    const float& fread=folder->readTime();
    readtime+=fread;
    IOVDbConn* cptr=folder->conn();
    CTMap::iterator citr=ctmap.find(cptr);
    if (citr!=ctmap.end()) {
      (citr->second)+=fread;
    } else {
      ctmap.insert(CTMap::value_type(cptr,fread));
    }
    delete folder;
  }
  ATH_MSG_INFO(  " bytes in (( " << std::fixed << std::setw(9) << std::setprecision(2) <<
    readtime << " ))s" );

  // close and delete connections, printing time in each one
  for (auto & pThisConnection : m_connections) {
    float fread=0;
    CTMap::iterator citr=ctmap.find(pThisConnection);
    if (citr!=ctmap.end()) fread=citr->second;
    pThisConnection->setInactive();
    pThisConnection->summary(fread);
    delete pThisConnection;
  }
  // finally remove the msg svc
  //delete m_log;
  return AthService::finalize();
}

cool::IDatabasePtr IOVDbSvc::getDatabase(bool readOnly) {
  // get default database connection
  cool::IDatabasePtr dbconn;
  if (m_par_defaultConnection=="" || m_connections.size()==0) {
    ATH_MSG_INFO( "No default COOL database connection is available");
    dbconn.reset();
  } else {
    if (m_connections[0]->isReadOnly()!=readOnly) {
      ATH_MSG_INFO("Changing state of default connection to readonly=" << readOnly );
      m_connections[0]->setReadOnly(readOnly);
    }
    dbconn=m_connections[0]->getCoolDb();
  }
  return dbconn;
}

StatusCode IOVDbSvc::preLoadAddresses(StoreID::type storeID,tadList& tlist) {
  // Read information for folders and setup TADs
  if (storeID!=StoreID::DETECTOR_STORE) return StatusCode::SUCCESS;
  // Preloading of addresses should be done ONLY for detector store
  ATH_MSG_DEBUG( "preLoadAddress: storeID -> " << storeID );
  // check File Level Meta Data of input, see if any requested folders are available there
  SG::ConstIterator<IOVMetaDataContainer> cont;
  SG::ConstIterator<IOVMetaDataContainer> contEnd;
  if (StatusCode::SUCCESS==m_h_metaDataStore->retrieve(cont,contEnd)) {
    unsigned int ncontainers=0;
    unsigned int nused=0;
    for (;cont!=contEnd; ++cont) {
      ++ncontainers;
      const std::string& fname=cont->folderName();
      // check if this folder is in list requested by IOVDbSvc
      for (const auto & thisNamePtrPair : m_foldermap) {
        // take data from FLMD only if tag override is NOT set
        if (thisNamePtrPair.second->folderName()==fname && !(thisNamePtrPair.second->tagOverride())) {
          ATH_MSG_INFO( "Folder " << fname << " will be taken from file metadata" );
          thisNamePtrPair.second->useFileMetaData();
          thisNamePtrPair.second->setFolderDescription( cont->folderDescription() );
          ++nused;
          break;
        }
      }
    }
    ATH_MSG_INFO( "Found " << ncontainers <<  " metadata containers in input file, " << nused << " will be used");
  } else {
    ATH_MSG_DEBUG( "Could not retrieve IOVMetaDataContainer objects from MetaDataStore" );
  }

  // Remove folders which should only be read from file meta data, but
  // were not found in the MetaDataStore

  // Note: we cannot iterate and perform erase within the iteration
  // because the iterator becomes invalid. So first collect the keys
  // to erase in a first pass and then erase them.
  std::vector<std::string> keysToDelete;
   for (const auto & thisNamePtrPair : m_foldermap)  {
    if (thisNamePtrPair.second->fromMetaDataOnly() && !thisNamePtrPair.second->readMeta()) {
      ATH_MSG_INFO( "preLoadAddresses: Removing folder " << thisNamePtrPair.second->folderName() << 
        ". It should only be in the file meta data and was not found." );
      keysToDelete.push_back(thisNamePtrPair.first);
    }
  }
  
  for (auto & thisKey : keysToDelete) {
    FolderMap::iterator fitr=m_foldermap.find(thisKey);
    if (fitr != m_foldermap.end()) {
      fitr->second->conn()->decUsage();
      delete (fitr->second);
      m_foldermap.erase(fitr);
    } else {
      ATH_MSG_ERROR( "preLoadAddresses: Could not find folder " << thisKey << " for removal" );
    }
  }
  

  // loop over all folders, grouped by connection
  // do metadata folders on first connection (default connection)
  bool doMeta=true;
  // do not close COOL connection until next one has been opened, this enables
  // connection sharing in CORAL, so all COOL connections will use the same
  // CORAL one (althugh they will each be given a separate session)
  IOVDbConn* oldconn=0;
  for (const auto & pThisConnection : m_connections) {
    if (pThisConnection->nFolders()>0 || doMeta) {
      // loop over all folders using this connection
      for (const auto & thisNamePtrPair : m_foldermap) {
        IOVDbFolder* folder=thisNamePtrPair.second;
        if (folder->conn()==pThisConnection || (folder->conn()==0 && doMeta)) {
          std::unique_ptr<SG::TransientAddress> tad =
            folder->preLoadFolder(&(*m_h_detStore),m_par_cacheRun.value(),
                                  m_par_cacheTime.value());
          if (oldconn!=pThisConnection) {
            // close old connection if appropriate
            if (m_par_manageConnections && oldconn!=0) oldconn->setInactive();
            oldconn=pThisConnection;
          }
          if (tad==0) {
            ATH_MSG_ERROR( "preLoadFolder failed for folder " << folder->folderName() );
            return StatusCode::FAILURE;
          }
          // for write-metadata folder, request data preload
          if (folder->writeMeta()) {
            if (StatusCode::SUCCESS!=m_h_IOVSvc->preLoadDataTAD(tad.get(),folder->eventStore())) {
              ATH_MSG_ERROR( "Could not request IOVSvc to preload metadata for " << folder->folderName() );
              return StatusCode::FAILURE;
            }
          } else {
            // for other folders, just preload TAD (not data)
            if (StatusCode::SUCCESS!=m_h_IOVSvc->preLoadTAD(tad.get(), folder->eventStore())) {
              ATH_MSG_ERROR( "Could not request IOVSvc to preload metadata for " << folder->folderName() );
              return StatusCode::FAILURE;
            }
          }
          // Add TAD to Storegate
          tlist.push_back(tad.release());
          // check for IOV override
          folder->setIOVOverride(m_par_forceRunNumber.value(),
                                 m_par_forceLumiblockNumber.value(),m_par_forceTimestamp.value());
        }
      }
    }
    doMeta=false;
  }
  // close last connection
  if (oldconn!=0 and m_par_manageConnections) oldconn->setInactive();

  // some folder keys may have changed during preloadFolder due to use of
  // <key> specification in folder description string
  // build a new foldermap with the updated keys
  FolderMap newmap;
  for (const auto & thisNamePtrPair : m_foldermap) {
    newmap[thisNamePtrPair.second->key()]=thisNamePtrPair.second;
  }
  m_foldermap=newmap;
  // fill global and explicit folder tags into TagInfo
  if (StatusCode::SUCCESS!=fillTagInfo()) 
    ATH_MSG_ERROR("Could not fill TagInfo object from preLoadAddresses" );
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::loadAddresses(StoreID::type /*storeID*/, tadList& /*list*/ ) {
  // this method does nothing
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode IOVDbSvc::updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                                   const EventContext& /*ctx*/)
{
  // Provide TAD and associated range, actually reading the conditions data

  // Read information for folders and setup TADs
  if (storeID!=StoreID::DETECTOR_STORE) return StatusCode::FAILURE;
  Gaudi::Guards::AuditorGuard auditor(std::string("UpdateAddr::")+(tad->name().empty() ? "anonymous" : tad->name()),
                                      auditorSvc(), "preLoadProxy");

  // first check if this key is managed by IOVDbSvc
  // return FAILURE if not - this allows other AddressProviders to be 
  // asked for the TAD
  const std::string& key=tad->name();
  FolderMap::const_iterator fitr=m_foldermap.find(key);
  if (fitr==m_foldermap.end()) {
    ATH_MSG_VERBOSE( 
        "updateAddress cannot find description for TAD " << key );
    return StatusCode::FAILURE;
  }
  IOVDbFolder* folder=fitr->second;
  if (folder->clid()!=tad->clID()) {
    ATH_MSG_VERBOSE( "CLID for TAD " << key << " is " << tad->clID()
             << " but expecting " << folder->clid() );
    
    return StatusCode::FAILURE;
  }

  // IOVDbSvc will satisfy the request, using already found folder
  // now determine the current IOVTime
  if (m_state==IOVDbSvc::INITIALIZATION && !m_iovTime.isValid()) {
    ATH_MSG_DEBUG( "updateAddress: in initialisation phase and no iovTime defined" );
    return::StatusCode::SUCCESS;
  }
  if (m_state==IOVDbSvc::EVENT_LOOP) {
    // determine iovTime from eventID in the event context
    const EventIDBase* evid = EventIDFromStore( m_h_sgSvc );
    if( evid ) {
      m_iovTime.setRunEvent( evid->run_number(), evid->lumi_block()) ;
      // save both seconds and ns offset for timestamp
      uint64_t nsTime = evid->time_stamp() *1000000000LL;
      nsTime += evid->time_stamp_ns_offset();
      m_iovTime.setTimestamp(nsTime);
      ATH_MSG_DEBUG( "updateAddress - using iovTime from EventInfo: " << m_iovTime);
    } else {
      // failed to get event info - just return success
      ATH_MSG_DEBUG( "Could not get event - initialise phase");
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_DEBUG("updateAddress: using iovTime from init/beginRun: " << m_iovTime);
  }

  // check consistency of global tag and database instance, if set
  // catch most common user misconfigurations
  // this is only done here as need global tag to be set even if read from file
  if (!m_par_dbinst.empty() && !m_globalTag.empty() and (m_par_source!="CREST")) {
    const std::string tagstub=m_globalTag.substr(0,7);
    ATH_MSG_DEBUG( "Checking " << m_par_dbinst << " against " <<tagstub );
    if (((m_par_dbinst=="COMP200" || m_par_dbinst=="CONDBR2") && 
         (tagstub!="COMCOND" && tagstub!="CONDBR2")) ||
        (m_par_dbinst=="OFLP200" && (tagstub!="OFLCOND" && tagstub!="CMCCOND"))) {
      ATH_MSG_FATAL( "Likely incorrect conditions DB configuration! " 
             <<  "Attached to database instance " << m_par_dbinst <<
        " but global tag begins " << tagstub );
      ATH_MSG_FATAL( "See Atlas/CoolTroubles wiki for details," << 
        " or set IOVDbSvc.DBInstance=\"\" to disable check" );
      return StatusCode::FAILURE;
    }
  }


  // obtain the validity key for this folder (includes overrides)
  cool::ValidityKey vkey=folder->iovTime(m_iovTime);
  ATH_MSG_DEBUG("Validity key "<<vkey);
  if (!folder->readMeta() && !folder->cacheValid(vkey)) {
    // mark this folder as not-dropped so cache-read will succeed
    folder->setDropped(false);
    // reload cache for this folder (and all others sharing this DB connection)
    ATH_MSG_DEBUG( "Triggering cache load for folder " << folder->folderName());
    if (StatusCode::SUCCESS!=loadCaches(folder->conn())) {
      ATH_MSG_ERROR( "Cache load failed for folder " <<  folder->folderName() );
      return StatusCode::FAILURE;
    }
  }

  // data should now be in cache
  std::unique_ptr<IOpaqueAddress> address;
  IOVRange range;
  // setup address and range
  {
    Gaudi::Guards::AuditorGuard auditor(std::string("FldrSetup:")+(tad->name().empty() ? "anonymous" : tad->name()),
                                        auditorSvc(), "preLoadProxy");
    if (!folder->getAddress(vkey,&(*m_h_persSvc),poolSvcContext(),address,
                            range,m_poolPayloadRequested)) {
      ATH_MSG_ERROR( "getAddress failed for folder " << folder->folderName() );
      return StatusCode::FAILURE;
    }
  }
  // reduce minimum IOV of timestamp folders to avoid 'thrashing' 
  // due to events slightly out of order in HLT
  if (folder->timeStamp()) {
    cool::ValidityKey start=range.start().timestamp();
    if (start>m_iovslop) start-=m_iovslop;
    range=IOVRange(IOVTime(start),range.stop());
  }

  // Pass range onto IOVSvc
  if (StatusCode::SUCCESS!=m_h_IOVSvc->setRange(tad->clID(),tad->name(),
                                                range,folder->eventStore())) {
    ATH_MSG_ERROR( "setRange failed for folder " << folder->folderName() );
    return StatusCode::FAILURE;
  }
  tad->setAddress(address.release());
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVDbSvc::getRange( const CLID&        clid, 
                               const std::string& dbKey,
                               const IOVTime&     time,
                               IOVRange&          range,
                               std::string&       tag,
                               std::unique_ptr<IOpaqueAddress>&   address) {

  ATH_MSG_DEBUG( "getRange  clid: " << clid << " key: \""<< dbKey << "\"  t: " << time );
  const std::string& key=dbKey;
  FolderMap::const_iterator fitr=m_foldermap.find(key);
  if (fitr==m_foldermap.end()) {
    ATH_MSG_VERBOSE("getRange cannot find description for dbKey " << key );
    return StatusCode::FAILURE;
  }
  IOVDbFolder* folder=fitr->second;
  if (folder->clid()!=clid) {
    ATH_MSG_VERBOSE( "supplied CLID for " << key << " is " 
             << clid
             << " but expecting " << folder->clid() );
    
    return StatusCode::FAILURE;
  }


  /// FIXME?
  tag = folder->key();
  // check consistency of global tag and database instance, if set
  // catch most common user misconfigurations
  // this is only done here as need global tag to be set even if read from file
  if (!m_par_dbinst.empty() && !m_globalTag.empty() and m_par_source!="CREST") {
    const std::string tagstub=m_globalTag.substr(0,7);
    ATH_MSG_DEBUG( "Checking " << m_par_dbinst << " against " <<tagstub );
    if (((m_par_dbinst=="COMP200" || m_par_dbinst=="CONDBR2") && 
         (tagstub!="COMCOND" && tagstub!="CONDBR2")) ||
        (m_par_dbinst=="OFLP200" && (tagstub!="OFLCOND" && tagstub!="CMCCOND"))) {
      ATH_MSG_FATAL( "Likely incorrect conditions DB configuration! " 
             <<  "Attached to database instance " << m_par_dbinst <<
        " but global tag begins " << tagstub );
      ATH_MSG_FATAL( "See Atlas/CoolTroubles wiki for details," << 
        " or set IOVDbSvc.DBInstance=\"\" to disable check" );
      return StatusCode::FAILURE;
    }
  }


  // obtain the validity key for this folder (includes overrides)
  cool::ValidityKey vkey=folder->iovTime(time);
  if (!folder->readMeta() && !folder->cacheValid(vkey)) {
    // mark this folder as not-dropped so cache-read will succeed
    folder->setDropped(false);
    // reload cache for this folder (and all others sharing this DB connection)
    ATH_MSG_DEBUG( "Triggering cache load for folder " << folder->folderName() );
    if (StatusCode::SUCCESS!=loadCaches(folder->conn(),&time)) {
      ATH_MSG_ERROR( "Cache load failed for folder " <<  folder->folderName() );
      return StatusCode::FAILURE;
    }
  }

  // data should now be in cache
  address.reset();
  // setup address and range
  {
    Gaudi::Guards::AuditorGuard auditor(std::string("FldrSetup:")+(key.empty() ? "anonymous" : key),
                                        auditorSvc(), "preLoadProxy");
    if (!folder->getAddress(vkey,&(*m_h_persSvc),poolSvcContext(),address,
                            range,m_poolPayloadRequested)) {
      ATH_MSG_ERROR("getAddress failed for folder " <<folder->folderName() );
      return StatusCode::FAILURE;
    }
  }

  // Special handling for extensible folders:
  if (folder->extensible()) {
    // Set the end time to just past the current event or lumiblock.
    IOVTime extStop = range.stop();
    if (folder->timeStamp()) {
      extStop.setTimestamp (time.timestamp() + 1);
    }
    else {
      extStop.setRETime (time.re_time() + 1);
    }
    range = IOVRange (range.start(), extStop);
  }



  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode IOVDbSvc::setRange( const CLID&        /*clid*/,
                               const std::string& /*dbKey*/,
                               const IOVRange&    /*range*/,
                               const std::string& /*storeName*/ ) {
  // this method does nothing
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::signalBeginRun(const IOVTime& beginRunTime,
                                    const EventContext& ctx)
{
  // Begin run - set state and save time for later use
  m_state=IOVDbSvc::BEGIN_RUN;
  // Is this a different run compared to the previous call?
  bool newRun = m_iovTime.isValid() && (m_iovTime.run() != beginRunTime.run());
  m_iovTime=beginRunTime;
  // For a MC event, the run number we need to use to look up the conditions
  // may be different from that of the event itself.  Override the run
  // number with the conditions run number from the event context,
  // if it is defined.
  EventIDBase::number_type conditionsRun =
    Atlas::getExtendedEventContext(ctx).conditionsRun();
  if (conditionsRun != EventIDBase::UNDEFNUM) {
    m_iovTime.setRunEvent (conditionsRun, m_iovTime.event());
  }

  ATH_MSG_DEBUG( "signalBeginRun> begin run time " << m_iovTime);
  if (!m_par_onlineMode) {
    return StatusCode::SUCCESS;
  }

  // ONLINE mode: allow adding of new calibration constants between runs
  if (!newRun) {
    ATH_MSG_DEBUG( "Same run as previous signalBeginRun call. Skipping re-loading of folders..." );
    return StatusCode::SUCCESS;
  }

  // all other stuff is event based so happens after this.
  // this is before first event of each run
  ATH_MSG_DEBUG( "In online mode will recheck ... " );
  ATH_MSG_DEBUG( "First reload PoolCataloge ... " );
  
  pool::IFileCatalog* catalog=
    const_cast<pool::IFileCatalog*>(m_h_poolSvc->catalog());
  catalog->commit();
  catalog->start(); 

  for (const auto & pThisConnection : m_connections){
    // only access connections which are actually in use - avoids waking up
    // the default DB connection if it is not being used
    if (pThisConnection->nFolders()>0) {
      //request for database activates connection
      cool::IDatabasePtr dbconn=pThisConnection->getCoolDb();
      if (dbconn.get()==0) {
        ATH_MSG_FATAL( "Conditions database connection " <<  pThisConnection->name() << " cannot be opened - STOP" );
        return StatusCode::FAILURE;
      }
      for (const auto & thisNamePtrPair: m_foldermap) { 
        IOVDbFolder* folder=thisNamePtrPair.second;
        if (folder->conn()!=pThisConnection) continue; 
        folder->printCache();
        cool::ValidityKey vkey=folder->iovTime(m_iovTime);
        {
          Gaudi::Guards::AuditorGuard auditor(std::string("FldrCache:")+folder->folderName(), auditorSvc(), "preLoadProxy");
          if (!folder->loadCacheIfDbChanged(vkey, m_globalTag, dbconn, m_h_IOVSvc)) {
            ATH_MSG_ERROR( "Problem RELOADING: " << folder->folderName());
            return StatusCode::FAILURE;
          }
        }
        folder->printCache();
      }
    }
    if (m_par_manageConnections) pThisConnection->setInactive();
  }
  return StatusCode::SUCCESS;
}

void IOVDbSvc::signalEndProxyPreload() {
  // this method does nothing
}

void IOVDbSvc::handle( const Incident& inc) {
  // Handle incidents:
  // BeginEvent to set IOVDbSvc state to EVENT_LOOP
  // StoreCleared to close any open POOL files
  ATH_MSG_VERBOSE( "entering handle(), incident type " << inc.type()
           << " from " << inc.source() );
  if (inc.type()=="BeginEvent") {
    m_state=IOVDbSvc::EVENT_LOOP;
  } else {
    const StoreClearedIncident* sinc= 
      dynamic_cast<const StoreClearedIncident*>(&inc);
    if ((inc.type()=="StoreCleared" && sinc!=0 && sinc->store()==&*m_h_sgSvc)) {
      if (inc.type()=="StoreCleared") {
        m_state=IOVDbSvc::FINALIZE_ALG;
      }
      if (m_par_managePoolConnections && m_poolPayloadRequested) {
        // reset POOL connection to close all open conditions POOL files
        m_par_managePoolConnections.set(false);
        m_poolPayloadRequested=false;
        if( m_poolSvcContext ) {
           if (StatusCode::SUCCESS==m_h_poolSvc->disconnect(m_poolSvcContext)) {
              ATH_MSG_DEBUG( "Successfully closed input POOL connections");
           } else {
              ATH_MSG_WARNING( "Unable to close input POOL connections" );
           }
           // reopen transaction
           if (StatusCode::SUCCESS==m_h_poolSvc->connect(pool::ITransaction::READ,
                                                         m_poolSvcContext)) {
              ATH_MSG_DEBUG("Reopend read transaction for POOL conditions input files" );
           } else {
              ATH_MSG_WARNING("Cannot reopen read transaction for POOL conditions input files");
           }
        }
      }
    }
  }
}

StatusCode IOVDbSvc::processTagInfo() {
  // Processing of taginfo
  // Set GlobalTag and any folder-specific overrides if given
  const TagInfo* tagInfo=0;
  if (StatusCode::SUCCESS!=m_h_detStore->retrieve(tagInfo)) {
    ATH_MSG_ERROR( "No TagInfo in DetectorStore" );
    return StatusCode::FAILURE;
  }
  // dump out contents of TagInfo
  ATH_MSG_DEBUG( "Tags from input TagInfo:");
  if (msg().level()>=MSG::DEBUG) tagInfo->printTags(msg());
  
  // check IOVDbSvc GlobalTag, if not already set
  if (m_globalTag=="") {
    tagInfo->findTag("IOVDbGlobalTag",m_globalTag);
    if (m_globalTag!="") ATH_MSG_INFO( "Global tag: " << m_globalTag<< " set from input file" );
  }

  // now check for tag overrides for specific folders
  const auto & nameTagPairs= tagInfo->getInputTags();
  for (const auto & thisNameTagPair: nameTagPairs) {
    // assume tags relating to conditions folders start with /
    if (not refersToConditionsFolder(thisNameTagPair)) continue;
    // check for folder(s) with this name in (key, ptr) pair
    for (const auto & thisKeyPtrPair: m_foldermap) {
      IOVDbFolder* folder=thisKeyPtrPair.second;
      const std::string& ifname=folder->folderName();
      if (ifname!=thisNameTagPair.first) continue; 
      // use an override from TagInfo only if there is not an explicit jo tag,
      // and folder meta-data is not used, and there is no <noover/> spec,
      // and no global tag set in job options
      const auto & theTag{thisNameTagPair.second};
      if (folder->joTag()=="" && !folder->readMeta() && !folder->noOverride() && m_par_globalTag=="") {
        folder->setTagOverride(theTag,false);
        ATH_MSG_INFO( "TagInfo override for tag " << theTag << " in folder " << ifname );
      } else if (folder->joTag()!=theTag) {
        const std::string tagTypeString=(folder->joTag().empty()) ? "hierarchical" : "jobOption";
        ATH_MSG_INFO( "Ignoring inputfile TagInfo request for tag " << theTag << " in folder " << ifname<<" in favour of "<<tagTypeString);
      }
    }
  }
  return StatusCode::SUCCESS;
}

std::vector<std::string> 
IOVDbSvc::getKeyList() {
  // return a list of all the StoreGate keys being managed by IOVDbSvc
  std::vector<std::string> keys;
  keys.reserve(m_foldermap.size());
  std::for_each(m_foldermap.begin(),m_foldermap.end(), [&keys](const auto &i){keys.emplace_back(i.first);});
  return keys;
}

bool IOVDbSvc::getKeyInfo(const std::string& key, IIOVDbSvc::KeyInfo& info) {
  // return information about given SG key
  // first attempt to find the folder object for this key
  FolderMap::const_iterator itr = m_foldermap.find(key);
  if (itr!=m_foldermap.end()) {
    const IOVDbFolder* f=itr->second;
    info.folderName = f->folderName();
    info.tag = f->resolvedTag();
    info.range = f->currentRange();
    info.retrieved = f->retrieved();
    info.bytesRead = f->bytesRead();
    info.readTime = f->readTime();
    info.extensible = f->extensible();
    return true;
  } else {
    info.retrieved = false;
    return false;
  }
}

bool IOVDbSvc::dropObject(const std::string& key, const bool resetCache) {
  // find the folder corresponding to this object
  FolderMap::const_iterator itr=m_foldermap.find(key);
  if (itr!=m_foldermap.end()) {
    IOVDbFolder* folder=itr->second;
    CLID clid=folder->clid();
    SG::DataProxy* proxy=m_h_detStore->proxy(clid,key);
    if (proxy!=0) {
      m_h_detStore->clearProxyPayload(proxy);
      ATH_MSG_DEBUG("Dropped payload for key " << key );
      folder->setDropped(true);
      if (resetCache) {
        folder->resetCache();
        ATH_MSG_DEBUG( "Cache reset done for folder " << folder->folderName() );
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


/************************/
// private methods of IOVDbSvc

StatusCode IOVDbSvc::checkEventSel() {
  // check if EventSelector is being used to override run numbers
  // if so, we can set IOV time already to allow conditons retrieval
  // in the initialise phase, needed for setting up simulation

  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc",name());
  ATH_CHECK( joSvc.retrieve() );

  if (!joSvc->has("EventSelector.OverrideRunNumber")) {
    // do not return FAILURE if the EventSelector cannot be found, or it has
    // no override property, can e.g. happen in online running
    ATH_MSG_DEBUG( "No EventSelector.OverrideRunNumber property found" );
    return StatusCode::SUCCESS;
  }

  BooleanProperty bprop("OverrideRunNumber",false);
  ATH_CHECK( bprop.fromString(joSvc->get("EventSelector.OverrideRunNumber")) );
  if (bprop.value()) {
    // if flag is set, extract Run,LB and time
    ATH_MSG_INFO(  "Setting run/LB/time from EventSelector override in initialize" );
    uint32_t run,lumib;
    uint64_t time;
    bool allGood=true;
    if (m_par_forceRunNumber.value()!=0 ||
        m_par_forceLumiblockNumber.value()!=0)
      ATH_MSG_WARNING( "forceRunNumber property also set" );
    IntegerProperty iprop1("RunNumber",0);
    if (iprop1.fromString(joSvc->get("EventSelector.RunNumber","INVALID"))) {
      run=iprop1.value();
    } else {
      ATH_MSG_ERROR( "Unable to get RunNumber from EventSelector");
      allGood=false;
    }
    IntegerProperty iprop2("FirstLB",0);
    if (iprop2.fromString(joSvc->get("EventSelector.FirstLB","INVALID"))) {
      lumib=iprop2.value();
    } else {
      ATH_MSG_ERROR( "Unable to get FirstLB from EventSelector");
      allGood=false;
    }
    IntegerProperty iprop3("InitialTimeStamp",0);
    if (iprop3.fromString(joSvc->get("EventSelector.InitialTimeStamp","INVALID"))) {
      time=iprop3.value();
    } else {
      ATH_MSG_ERROR("Unable to get InitialTimeStamp from EventSelector" );
      allGood=false;
    }
    if (allGood) {
      m_iovTime.setRunEvent(run,lumib);
      uint64_t nsTime=time*1000000000LL;
      m_iovTime.setTimestamp(nsTime);
      ATH_MSG_INFO( "run/LB/time set to [" << run << "," << lumib << " : " << nsTime << "]" );
    } else {
      ATH_MSG_ERROR( "run/LB/Time NOT changed" );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::setupFolders() {
  // read the Folders joboptions and setup the folder list
  // no wildcards are allowed

  //1. Loop through folders
  std::list<IOVDbParser> allFolderdata;
  for (const auto & thisFolder : m_par_folders.value()) {
    ATH_MSG_DEBUG( "Setup folder " << thisFolder );
    IOVDbParser folderdata(thisFolder,msg());
    if (!folderdata.isValid()) return StatusCode::FAILURE;
    allFolderdata.push_back(folderdata);
  }

  //2. Loop through overwrites:
  // syntax for entries is <prefix>folderpath</prefix> <tag>value</tag>
  // folderpath matches from left of folderName
  // but if partial match, next character must be / so override for /Fred/Ji
  // matches /Fred/Ji/A and /Fred/Ji but not /Fred/Jim

  for (const auto & thisOverrideTag : m_par_overrideTags) {
    IOVDbParser keys(thisOverrideTag,msg());
    std::string prefix;
    if (!keys.getKey("prefix","",prefix)) { // || !keys.getKey("tag","",tag)) {
      ATH_MSG_ERROR( "Problem in overrideTag specification " <<thisOverrideTag );
      return StatusCode::FAILURE;
    }

    for (auto& folderdata : allFolderdata) {
      const std::string& ifname=folderdata.folderName();
      if (ifname.substr(0,prefix.size())==prefix && 
          (ifname.size()==prefix.size() || ifname.substr(prefix.size(),1)=="/")) {
        //Match! 
        folderdata.applyOverrides(keys,msg());
      }// end if
    }// end loop over allFolderdata
  }// end loop over overrides

  //3. Remove any duplicates:
  std::list<IOVDbParser>::iterator it1=allFolderdata.begin(); 
  std::list<IOVDbParser>::iterator it_e=allFolderdata.end(); 
  for (;it1!=it_e;++it1) {
    const IOVDbParser& folder1=*it1;
    std::list<IOVDbParser>::iterator it2=it1;
    ++it2;
    while(it2!=it_e) {
      const IOVDbParser& folder2=*it2;
      if (folder1==folder2) {
        it2=allFolderdata.erase(it2); //FIXME: Smarter distinction/reporting about same folder but different keys.
        ATH_MSG_DEBUG( "Removing duplicate folder " << folder1.folderName());
      } else {
        ++it2;
        //Catch suspicous cases:
        if (folder1.folderName()==folder2.folderName()) {
          ATH_MSG_WARNING( "Folder name appears twice: " << folder1.folderName() );
          ATH_MSG_WARNING( folder1 << " vs " << folder2 );
        }
      }
    }//end inner loop
  }//end outer loop

  //4.Set up folder map with cleaned folder list

  for (const auto& folderdata : allFolderdata) {
    // find the connection specification first - db or dbConnection
    // default is to use the 'default' connection
    IOVDbConn* conn=0;
    std::string connstr;
    if (folderdata.getKey("db","",connstr) || 
        folderdata.getKey("dbConnection","",connstr)) {
      // an explicit database name is specified
      // check if it is already present in the existing connections
      for (const auto & pThisConnection : m_connections) {
        if (pThisConnection->name()==connstr) {
          // found existing connection - use that
          conn=pThisConnection;
          break;
        }
      }
      if (conn==0) {
        // create new read-onlyconnection
        conn=new IOVDbConn(connstr,true,msg());
        m_connections.push_back(conn);
      }
    } else {
      // no connection specified - use default if available
      if (m_par_defaultConnection!="") {
        conn=m_connections[0];
      } else {
        ATH_MSG_FATAL( "Folder request " << folderdata.folderName() << 
          " gives no DB connection information and no default set" );
        return StatusCode::FAILURE;
      }
    }
    // create the new folder, but only if a folder for this SG key has not
    // already been requested
    IOVDbFolder* folder=new IOVDbFolder(conn,folderdata,msg(),&(*m_h_clidSvc), &(*m_h_metaDataTool),
                                        m_par_checklock, m_outputToFile.value(), m_par_source);
    const std::string& key=folder->key();
    if (m_foldermap.find(key)==m_foldermap.end()) {  //This check is too weak. For POOL-based folders, the SG key is in the folder description (not known at this point).
      m_foldermap[key]=folder;
      conn->incUsage();
    } else {
      ATH_MSG_ERROR( "Duplicate request for folder " << 
        folder->folderName() << 
        " associated to already requested Storegate key " << key );
      // clean up this duplicate request
      delete folder;
    }
  }// end loop over folders
  // check for folders to be written to metadata
  for (const auto & folderToWrite : m_par_foldersToWrite) {
    // match wildcard * at end of string only (i.e. /A/* matches /A/B, /A/C/D)
    std::string match=folderToWrite;
    std::string::size_type idx=folderToWrite.find("*");
    if (idx!=std::string::npos) {
      match=folderToWrite.substr(0,idx);
    }
    for (const auto & thisFolder : m_foldermap) {
      IOVDbFolder* fptr=thisFolder.second;
      if ((fptr->folderName()).substr(0,match.size())==match) {
        fptr->setWriteMeta();
        ATH_MSG_INFO( "Folder " << fptr->folderName() << " will be written to file metadata" );
      }
    }//end loop over FolderMap
  }//end loop over  m_par_foldersToWrite
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::fillTagInfo() {
  if (m_par_globalTag!="") {
    ATH_MSG_DEBUG( "Adding GlobalTag " << m_par_globalTag << " into TagInfo" );
    if (StatusCode::SUCCESS!=m_h_tagInfoMgr->addTag("IOVDbGlobalTag",m_par_globalTag))
      return StatusCode::FAILURE;
  }
  // add all explicit tags specified in folders
  // can be from Folders or tagOverrides properties
  for (const auto & thisFolder : m_foldermap) {
    const IOVDbFolder* folder=thisFolder.second;
    if (folder->joTag()!="") {
      ATH_MSG_DEBUG( "Adding folder " << folder->folderName() <<" tag " << folder->joTag() << " into TagInfo" );
      if (StatusCode::SUCCESS!=m_h_tagInfoMgr->addTag(folder->folderName(),folder->joTag()))
        return StatusCode::FAILURE;
    }
    // check to see if any input TagInfo folder overrides should be removed
    // this anticipates the decisions which will be made in processTagInfo
    // Here we do not have access to the TagInfo object, but can put remove
    // requests in for all folders if the global tag is set, or if there is
    // an explict joboption tag, nooverride spec, or data comes from metadata
    if (m_par_globalTag!="" || folder->joTag()!="" || folder->noOverride() ||
        folder->readMeta()) {
      if (StatusCode::SUCCESS!=
          m_h_tagInfoMgr->removeTagFromInput(folder->folderName())) {
        ATH_MSG_WARNING( "Could not add TagInfo remove request for "
               << folder->folderName() );
      } else {
        ATH_MSG_INFO( "Added taginfo remove for " << 
          folder->folderName() );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IOVDbSvc::loadCaches(IOVDbConn* conn, const IOVTime* time) {
  // load the caches for all folders using the given connection
  // so connection use is optimised

  Gaudi::Guards::AuditorGuard auditor(std::string("loadCachesOverhead:")+conn->name(), auditorSvc(), "preLoadProxy");

  ATH_MSG_DEBUG( "loadCaches: Begin for connection " << conn->name());
  // if global abort already set, load nothing
  if (m_abort) return StatusCode::FAILURE;
  bool access=false;
  StatusCode sc=StatusCode::SUCCESS;
  for (const auto & thisNamePtrPair : m_foldermap) {
    IOVDbFolder* folder=thisNamePtrPair.second;
    if (folder->conn()!=conn) continue;
    cool::ValidityKey vkey=folder->iovTime(time==nullptr ? m_iovTime : *time);
    // protect against out of range times (timestamp -1 happened in FDR2)
    if (vkey>cool::ValidityKeyMax) {
      ATH_MSG_WARNING( "Requested validity key " << vkey << " is out of range, reset to 0" );
      vkey=0;
    }
    if (!folder->cacheValid(vkey) && !folder->dropped()) {
      access=true;
      {
        Gaudi::Guards::AuditorGuard auditor(std::string("FldrCache:")+folder->folderName(), auditorSvc(), "preLoadProxy");
        if (!folder->loadCache(vkey,m_par_cacheAlign,m_globalTag,m_par_onlineMode)) {
          ATH_MSG_ERROR( "Cache load (prefetch) failed for folder " << folder->folderName() );
          // remember the failure, but also load other folders on this connection
          // while it is open
          sc=StatusCode::FAILURE;
        }
      }
    }
  }
  // disconnect from database if we connected
  if (access && m_par_manageConnections) conn->setInactive();
  // if connection aborted, set overall abort so we do not waste time trying
  // to read data from other schema
  if (conn->aborted()) {
    ATH_MSG_FATAL( "Connection " << conn->name() << " was aborted, set global abort" );
    m_abort=true;
    ATH_MSG_FATAL( "loadCache: impossible to load cache!" ); 
    throw std::exception();
  }
  return sc;
}
