/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef IOVDBSVC_IOVDBSVC_H
#define IOVDBSVC_IOVDBSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IService.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "SGTools/DataProxy.h"

#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "IOVDbSvc/IIOVCondDbSvc.h"
#include "AthenaKernel/IAddressProvider.h"

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
class IAddressCreator;   
class IClassIDSvc;
class IPoolSvc;

class EventID;
class ITagInfoMgr;
class IIncidentSvc;

class IOVDbFolder;

class IIOVDbMetaDataTool;
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
                 public virtual AthService
{
  // Forward declarations
  template <class TYPE> class SvcFactory;
  
public:
  
  IOVDbSvc( const std::string& name, ISvcLocator* svc );
  virtual ~IOVDbSvc();
  
  /// Service init
  virtual StatusCode initialize() override;

  /// Service finalize
  virtual StatusCode finalize() override;
  
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
                               IOpaqueAddress*& ioa) override;



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

  /// Register callback for TagInfo access
  virtual StatusCode registerTagInfoCallback() override;

  /// Callback method for TagInfo access
  StatusCode         processTagInfo(IOVSVC_CALLBACK_ARGS);

  // return list of SG keys being provided by IOVDbSvc
  virtual std::vector<std::string> getKeyList() override;

  // return information about one SG key
  // - folder, tag, IOVRange and whether data has been retrieved
  // (if not, range and tag may not be meaningful)
  // return false if this key is not known to IOVDbSvc
  virtual
  bool getKeyInfo(const std::string& key, std::string& foldername,
                  std::string& tag, IOVRange& range, bool& retrieved,
                  unsigned long long& bytesRead, float& readTime) override;


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
  void printMetaDataContainer(const IOVMetaDataContainer* cont);
  void dumpKeys();

  // job option parameters
  // default database connection
  std::string         m_par_defaultConnection;
  //  IOVDbSvc global tag
  std::string                    m_par_globalTag;
  // production database instance, used to cross-check global tag
  std::string m_par_dbinst;
  //  a list of folders to preload
  std::vector<std::string>            m_par_folders;
  //  a list of overriding tags definitions
  std::vector<std::string>            m_par_overrideTags;
  //  a list of folders to write to file meta data
  std::vector<std::string>           m_par_foldersToWrite;    
  //  a flag to trigger the connections management
  BooleanProperty                m_par_manageConnections;
  //  a flag to manage pool connections
  BooleanProperty                m_par_managePoolConnections;
  // flag to dump SG keys at end of first event
  bool m_par_dumpkeys;
  //  a global force run number
  IntegerProperty                m_par_forceRunNumber;
  //  a global force lumiblock number
  IntegerProperty                m_par_forceLumiblockNumber;
  // a global force timestamp
  IntegerProperty m_par_forceTimestamp;
  //  max number of pool files open allowed
  IntegerProperty                m_par_maxNumPoolFiles;
  // timestampslop - backdata timestamp IOVs by this many seconds
  FloatProperty                  m_par_timeStampSlop;
  // force larger numbers of runs to be cached
  IntegerProperty m_par_cacheRun;
  // force larger timeranges to be cached (seconds)
  IntegerProperty m_par_cacheTime;
  // cache alignment - divide cache into N slices and align queries on slice
  // should be useful to improve Frontier cache hit rate
  UnsignedIntegerProperty m_par_cacheAlign;
  // online mode flag to ignore missing channels outside cache range
  BooleanProperty m_par_onlineMode;
  // check to ensure global/HVS tags are locked (for production)
  BooleanProperty m_par_checklock;

  // internal parameters  
  // handles to other services and tools
  ServiceHandle<IIOVSvc>         m_h_IOVSvc;
  ServiceHandle<StoreGateSvc>    m_h_sgSvc;
  ServiceHandle<StoreGateSvc>    m_h_detStore;
  ServiceHandle<StoreGateSvc>    m_h_metaDataStore;
  ServiceHandle<IAddressCreator> m_h_persSvc;   
  ServiceHandle<IClassIDSvc>     m_h_clidSvc;
  ServiceHandle<IPoolSvc>        m_h_poolSvc;
  ToolHandle<IIOVDbMetaDataTool> m_h_metaDataTool;
  ServiceHandle<ITagInfoMgr>     m_h_tagInfoMgr;
  // message stream
  MsgStream* m_log;
        
  // Flag to signal when a pool payload has been requested. This
  // implies that a pool file has been open during an event, and will
  // allow one to close the pool file and the end of event.
  bool                           m_poolPayloadRequested;

  // Pool service context
  unsigned int                   m_poolSvcContext;

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
  IOVDbSvc_state                 m_state;

  // IOVTime to be set during initialation or begin run
  IOVTime                        m_iovTime;

  // Global tag from TagInfo or joboptions
  std::string                    m_globalTag;

  // IOV value to be subtraced for timeStampSlop
  cool::ValidityKey m_iovslop;

  // vector of managed connections
  typedef std::vector<IOVDbConn*> ConnVec;
  ConnVec m_connections;
  // map of SG keyname to folder objects
  typedef std::map<std::string,IOVDbFolder*> FolderMap;
  FolderMap m_foldermap;
  // gloal abort flag
  bool m_abort;

};

#endif
