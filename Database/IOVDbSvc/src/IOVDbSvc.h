/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IOVDbSvc.h
 * 
 * @brief  Athena service for Interval Of Validity database.
 * 
 * @author Antoine Perus <perus@lal.in2p3.fr>, Richard Hawkings
 * @date   Feb 2003, major update Jan 2009
 *
 * $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbSvc/src/IOVDbSvc.h,v 1.48 2009-02-10 14:09:58 hawkings Exp $
 */

#ifndef IOVDbSvc_IOVDbSvc_h
#define IOVDbSvc_IOVDbSvc_h

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IService.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IIoComponent.h"
#include "SGTools/DataProxy.h"
#include "EventInfoMgt/ITagInfoMgr.h"
#include "PoolSvc/IPoolSvc.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "IOVDbSvc/IIOVCondDbSvc.h"
#include "AthenaKernel/IOVTime.h"

#include "CoolKernel/IDatabase.h"
#include "CoolKernel/ValidityKey.h"

#include "IOVDbConn.h"


#include <string>
#include <vector>
#include <map>

class IOVSvc;
class IOVTime;
class IOVRange;
class StoreGateSvc; 
class IClassIDSvc;

class EventID;
class ITagInfoMgr;

class IOVDbFolder;

class IOVMetaDataContainer;
class CondAttrListCollection;

/**
 * @class   IOVDbSvc
 * 
 * @brief  Athena service for Interval Of Validity database.
 *         The IOVDbSvc may be in one of three time states which
 *         determines from where the IOV time is coming:
 *           initialization  -  IOV time must be set from the EventSelector
 *           begin run       -  IOV time should have been set with signalBeginRun
 *           event loop      -  IOV time is from EventInfo
 *           
 * 
 */
class IOVDbSvc : public virtual IIOVCondDbSvc,
                 public virtual IIOVDbSvc,
                 public virtual IAddressProvider,
                 public virtual IIncidentListener,
                 public virtual AthService,
                 public virtual IIoComponent
{
  // Forward declarations
  template <class TYPE> class SvcFactory;
  
public:
  
  /// Forward base class ctor
  using AthService::AthService;
  virtual ~IOVDbSvc();
  
  /// Service init
  virtual StatusCode initialize() override;
  StatusCode io_reinit() override final;

  /// Service finalize
  virtual StatusCode finalize() override;
  StatusCode io_finalize() override final;

  /// Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface )  override;
  static const InterfaceID& interfaceID();
  
  // IIOVDbSvc interface   

  /// Access to COOL database for a given folder
  virtual cool::IDatabasePtr getDatabase( bool readOnly ) override;

  typedef IAddressProvider::tadList tadList;
  typedef IAddressProvider::tadListIterator tadListIterator;

  /// \name Address use
  //@{
  /// Get all addresses that the provider wants to preload in SG maps
  virtual StatusCode preLoadAddresses( StoreID::type storeID,
                                       tadList& list ) override;
    
  /// Get all new addresses from Provider for this Event.
  virtual StatusCode loadAddresses( StoreID::type storeID,
                                    tadList& list ) override;
      
  /// Update a transient Address
  virtual StatusCode updateAddress( StoreID::type storeID,
                                    SG::TransientAddress* tad,
                                    const EventContext& ctx ) override;
  //@}
  

  /// \name IOV range access
  //@{
  /// Get range for a particular data object
  ///  identified by its clid and key and a requested IOVTime
  virtual StatusCode getRange( const CLID& clid, 
                               const std::string& dbKey, 
                               const IOVTime& time,
                               IOVRange& range,
                               std::string& tag,
                               std::unique_ptr<IOpaqueAddress>& ioa) override;



  /// Set range for a particular data object
  virtual StatusCode setRange( const CLID& clid,
                               const std::string& dbKey,
                               const IOVRange& range,
                               const std::string& tag ) override;

  /// Set time for begin run
  virtual StatusCode signalBeginRun(const IOVTime& beginRunTime,
                                    const EventContext& ctx) override;

  /// Signal that callback has been fired
  virtual void       signalEndProxyPreload() override;
  //@}
  
  /// Incident service handle for EndEvent
  virtual void handle( const Incident& incident ) override;

  /// Process TagInfo
  virtual StatusCode processTagInfo() override;

  // return list of SG keys being provided by IOVDbSvc
  virtual std::vector<std::string> getKeyList() override;

  /// Return information about SG key
  /// return false if this key is not known to IOVDbSvc
  virtual
  bool getKeyInfo(const std::string& key, IIOVDbSvc::KeyInfo& info) override;

  // drop an IOVDbSvc-managed object from Storegate, indicating we will
  // not read it again and can free up memory 
  // If resetCache=True, also drop the corresponding folder cache
  // so any subsequent reads will access the database again
  // returns False if key not known to IOVDbSvc
  virtual
  bool dropObject(const std::string& key,const bool resetCache=false) override;


private:
  // internal methods
  StatusCode checkEventSel();
  StatusCode setupFolders();
  StatusCode fillTagInfo();
  StatusCode loadCaches(IOVDbConn* conn, const IOVTime* time=nullptr);

  // job option parameters
  // default database connection
  Gaudi::Property<std::string>    m_par_defaultConnection{this,"dbConnection","sqlite://;schema=cooldummy.db;dbname=OFLP200","default DB connection string"};
  //  IOVDbSvc global tag
  Gaudi::Property<std::string>    m_par_globalTag{this,"GlobalTag","","Global database tag"};
  // production database instance, used to cross-check global tag
  Gaudi::Property<std::string>    m_par_dbinst{this,"DBInstance","","Database instance (like OFLP200)"};
  //  a list of folders to preload
  Gaudi::Property<std::vector<std::string> >  m_par_folders{this,"Folders",{},"List of database folders to preload","OrderedSet<std::string>"};
  //  a list of overriding tags definitions
  Gaudi::Property<std::vector<std::string> >  m_par_overrideTags{this,"overrideTags",{},"List of foolder-tag overrides","OrderedSet<std::string>"};
  //  a list of folders to write to file meta data
  Gaudi::Property<std::vector<std::string> >  m_par_foldersToWrite{this,"FoldersToMetaData",{},"list of folders to write to file meta data","OrderedSet<std::string>"};    
  //  a flag to trigger the connections management
  BooleanProperty                m_par_manageConnections{this,"ManageConnections",true,"flag to trigger the connections management"};
  //  a flag to manage pool connections
  BooleanProperty                m_par_managePoolConnections{this,"ManagePoolConnections",true,"flag to manage pool connections"};
  //  a global force run number
  IntegerProperty                m_par_forceRunNumber{this,"forceRunNumber",0,"globally force run-number (default: no force)"};
  //  a global force lumiblock number
  IntegerProperty                m_par_forceLumiblockNumber{this,"forceLumiblockNumber",0,"globally force LB-number (default: no force)"};
  // a global force timestamp
  IntegerProperty m_par_forceTimestamp{this,"forceTimeStamp",0,"globally force time-stamp (default: no force)"};
  //  max number of pool files open allowed
  IntegerProperty                m_par_maxNumPoolFiles{this,"MaxPoolFilesOpen",5,"max number of pool files open allowed (0 means no limit)"};
  // timestampslop - backdata timestamp IOVs by this many seconds
  FloatProperty                  m_par_timeStampSlop{this,"TimeStampSlop",0.0,"backdata timestamp IOVs by this many seconds"};
  // force larger numbers of runs to be cached
  IntegerProperty m_par_cacheRun{this,"CacheRun",0,"force larger numbers of runs to be cached"};
  // force larger timeranges to be cached (seconds)
  IntegerProperty m_par_cacheTime{this,"CacheTime",0,"force larger timeranges to be cached (seconds)"};
  // cache alignment - divide cache into N slices and align queries on slice
  // should be useful to improve Frontier cache hit rate
  UnsignedIntegerProperty m_par_cacheAlign{this,"CacheAlign",0,"cache alignment - divide cache into N slices and align queries on slice"}; 
  // online mode flag to ignore missing channels outside cache range
  BooleanProperty m_par_onlineMode{this,"OnlineMode",false,"online mode flag to ignore missing channels outside cache range"};
  // check to ensure global/HVS tags are locked (for production)
  BooleanProperty m_par_checklock{this,"CheckLock",false,"check to ensure global/HVS tags are locked (for production)"};
  // Source of data as a string; default is "COOL_DATABASE"
  StringProperty m_par_source{this,"Source","COOL_DATABASE","source of data as a string (default COOL_DATABASE)"};
  // Format of data; default is empty string (default for a given source)
  StringProperty m_par_format{this,"Format",{},"Format of data; default is empty string (default for a given source)"};
  // Can output to file for debugging purposes
  BooleanProperty m_outputToFile{this,"OutputToFile",false,"output to file for debugging purposes"};
  // internal parameters  
  // handles to other services and tools
  ServiceHandle<IIOVSvc>         m_h_IOVSvc{this,"IOVSvc","IOVSvc"};
  ServiceHandle<StoreGateSvc>    m_h_sgSvc{this,"StoreGateSvc","StoreGateSvc"};
  ServiceHandle<StoreGateSvc>    m_h_detStore{this,"DetectorStore","DetectorStore"};
  ServiceHandle<StoreGateSvc>    m_h_metaDataStore{this,"MetaDataStore","StoreGateSvc/MetaDataStore"};
  ServiceHandle<IAddressCreator> m_h_persSvc{this,"EventPersistencySvc","EventPersistencySvc"};
  ServiceHandle<IClassIDSvc>     m_h_clidSvc{this,"ClassIDSvc","ClassIDSvc"};
  ServiceHandle<IPoolSvc>        m_h_poolSvc{this,"PoolSvc","PoolSvc"};
  PublicToolHandle<IIOVDbMetaDataTool> m_h_metaDataTool{this,"IOVDbMetaDataTool","IOVDbMetaDataTool"};
  ServiceHandle<ITagInfoMgr>     m_h_tagInfoMgr{this,"TagInfoMgr","TagInfoMgr"};
        
  // Flag to signal when a pool payload has been requested. This
  // implies that a pool file has been open during an event, and will
  // allow one to close the pool file and the end of event.
  bool                           m_poolPayloadRequested{false};

  // Pool service context
  int                   poolSvcContext();
  int                   m_poolSvcContext {-1};

  // Flag to keep track of which state we are in to know
  // how to obtain the IOV time, i.e. during:
  //   initialization  -  IOV time is from the  EventSelector
  //   begin run       -  IOV time should have been set with signalBeginRun
  //   event loop      -  IOV time is from EventInfo
  //   finalize alg    -  IOV time is the last event's time from EventInfo
  enum IOVDbSvc_state {
    INITIALIZATION = 0,
    BEGIN_RUN,
    EVENT_LOOP,
    FINALIZE_ALG
  };
  IOVDbSvc_state                 m_state{INITIALIZATION};

  // IOVTime to be set during initialation or begin run
  IOVTime                        m_iovTime{};

  // Global tag from TagInfo or joboptions
  std::string                    m_globalTag{};

  // IOV value to be subtraced for timeStampSlop
  cool::ValidityKey m_iovslop{};

  // vector of managed connections
  typedef std::vector<IOVDbConn*> ConnVec;
  ConnVec m_connections;
  // map of SG keyname to folder objects
  typedef std::map<std::string,IOVDbFolder*> FolderMap;
  FolderMap m_foldermap;
  // gloal abort flag
  bool m_abort{false};
 
};

#endif
