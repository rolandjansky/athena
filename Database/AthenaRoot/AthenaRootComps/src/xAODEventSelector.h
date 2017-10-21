///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODEventSelector.h 
// Header file for class xAODEventSelector
// Author: Johannes Elmsheuser, Will Buttinger
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENA_XAODEVENTSELECTOR_H 
#define ATHENAROOTCOMPS_ATHENA_XAODEVENTSELECTOR_H 1

// framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IAddressProvider.h"

#include "PoolSvc/IPoolSvc.h"

#include "TFile.h"

#include "xAODTEvent.h"

#include <unordered_map>

// Forward declaration
class ISvcLocator;
class StoreGateSvc;
class TTree;
class IClassIDSvc;
class IDictLoaderSvc;
namespace Athena { class xAODEventContext; }

namespace Athena {

/** @brief Class implementing the GAUDI @c IEvtSelector interface using 
 *         ROOT @c TTree as a backend
 */
class xAODEventSelector : 
  virtual public IEvtSelector,
  virtual public IEvtSelectorSeek,
  virtual public IAddressProvider,
  virtual public IIoComponent, virtual public IIncidentListener,
          public ::AthService
{ 
  friend class Athena::xAODEventContext;



  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  xAODEventSelector( const std::string& name, ISvcLocator* svcLoc );

  /// Destructor: 
  virtual ~xAODEventSelector(); 

  // Athena hooks
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface ) override;
  
  virtual void handle(const Incident& incident) override;

  ///@{
  /// @c IEvtSelector interface
  virtual StatusCode createContext( Context*& refpCtxt ) const override;

  virtual StatusCode last( Context& refContext ) const override;
  virtual StatusCode next( Context& refCtxt ) const override;
  virtual StatusCode next( Context& refCtxt, int jump ) const override;
  virtual StatusCode previous( Context& refCtxt ) const override;
  virtual StatusCode previous( Context& refCtxt, int jump ) const override;
  virtual StatusCode rewind( Context& refCtxt ) const override;

  virtual StatusCode createAddress( const Context& refCtxt, 
                                    IOpaqueAddress*& ) const override;
  virtual StatusCode releaseContext( Context*& refCtxt ) const override;
  virtual StatusCode resetCriteria( const std::string& cr, 
                                    Context& ctx )const override;
  ///@}

  ///@{
  /// @c IEventSeek interface
  /**
   * @brief Seek to a given event number.
   * @param evtnum  The event number to which to seek.
   */
  virtual StatusCode seek (Context& refCtxt, int evtnum) const override;

  /**
   * @brief return the current event number.
   * @return The current event number.
   */
  virtual int curEvent (const Context& refCtxt) const override;
  ///@}
  
  /// Callback method to reinitialize the internal state of the component 
  /// for I/O purposes (e.g. upon @c fork(2))
  virtual StatusCode io_reinit() override;

  ///@{
  /// @c IAddressProvider interface
  ///get all addresses from Provider : Called before Begin Event
  virtual 
  StatusCode preLoadAddresses(StoreID::type storeID, tadList& list) override;
 
   /// get all new addresses from Provider for this Event.
  virtual 
  StatusCode loadAddresses(StoreID::type storeID, tadList& list) override;
 
  /// update a transient Address
  virtual 
  StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                           const EventContext& ctx) override;
  ///@}

  ///@c ICollectionSize  interface
  virtual int size (Context& refCtxt) const override;


 private: 

  /// helper method to create proxies
  StatusCode createRootBranchAddresses(StoreID::type storeID,
                                       tadList &tads);


  /// helper method to retrieve the correct tuple
  TFile* fetchNtupleFile(const std::string& fname) const;

  /// helper method to create proxies for the metadata store
  StatusCode createMetaDataRootBranchAddresses() const;

  /// helper method to init the i/o components
  StatusCode do_init_io();

  /// switch to given file, loading metadata and triggering a beginInputFile incident
  StatusCode setFile(const std::string& fname);

  /// helper method to get the collection index (into `m_inputCollectionsName`)
  /// for a given event index `evtidx`.
  /// returns -1 if not found.
  int find_coll_idx(int evtidx) const;

  /// non-const access to self (b/c ::next() is const)
  xAODEventSelector *self() const
  { return const_cast<xAODEventSelector*>(this); }

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Pointer to the @c StoreGateSvc event store
  ServiceHandle<StoreGateSvc> m_dataStore;
  /// Pointer to the @c StoreGateSvc input metadata store
  ServiceHandle<StoreGateSvc> m_imetaStore;
  /// Pointer to the @c StoreGateSvc output metadata store
  ServiceHandle<StoreGateSvc> m_ometaStore;
  /// Pointer to the @c IClassIDSvc
  ServiceHandle<IClassIDSvc> m_clidsvc;
  /// Pointer to the @c IDictLoaderSvc
  ServiceHandle<IDictLoaderSvc> m_dictsvc;
  /// Handle to the incident service
  ServiceHandle<IIncidentSvc> m_incsvc;
  /// Handle to the PoolSvc (used in Hybrid mode when user is reading metadata with pool)
  ServiceHandle<IPoolSvc> m_poolSvc;
  /// Handle to the proxy provider svc
  ServiceHandle<IProxyProviderSvc> m_ppSvc;

  /// List of input files containing @c TTree 
  StringArrayProperty m_inputCollectionsName;

  /// Name of @c TTree to load from collection of input files
  StringProperty m_tupleName;

  /// Name of @c TTree to load from metadata of input files
  StringProperty m_metadataName;

  /// Number of events to skip at the beginning 
  long m_skipEvts;

  /// Number of Events read so far.
  mutable long m_nbrEvts;
  
  /// current event index
  mutable long m_curEvt;

  /// current collection index (into `m_inputCollectionsName`)
  mutable long m_collIdx;

  struct CollMetaData {
    /// number of entries up to this collection
    long min_entries;
    /// number of entries after this collection
    long max_entries;
  };
  /// cache of the number of entries for each collection
  mutable std::vector<CollMetaData> m_collEvts;


  // flag to trigger reloading of root branch addresses
  mutable bool m_needReload;

  // the list of transient addresses we "manage" or know about
  // these addresses are the things we can retrieve from the TEvent
  // for the event data
  std::unordered_map<SG::TransientAddress*, bool> m_rootAddresses;


  /// current TEvent being read
  mutable xAOD::xAODTEvent* m_tevent;

  /// current TFile being read
  mutable TFile* m_tfile;

  mutable long m_tevent_entries; //used to cache getEntries result of tevent

  int m_accessMode; //tevent access mode

  bool m_fillEventInfo = false; //if true, will fill EventInfo from xAOD::EventInfo

  bool m_readMetadataWithPool = false; //interacts with PoolSvc to create collections, needed by MetaDataSvc

  bool m_printPerfStats = false; //should we monitor the touched input branches?

#ifndef XAOD_ANALYSIS
  //these are here just for compatibility with RecExCommon ... we were trying to use this selector in recexcommon jobs for a bit
  bool m_backNavigation=false;
  StringProperty m_collectionType;
#endif

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> namespace Athena

#endif //> ATHENAROOTCOMPS_ATHENA_XAODEVENTSELECTOR_H
