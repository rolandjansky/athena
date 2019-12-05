/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DOUBLEEVENTSELECTORATHENAPOOL_H
#define DOUBLEEVENTSELECTORATHENAPOOL_H

/** @file DoubleEventSelectorAthenaPool.h
 *  @brief This file contains the class definition for the DoubleEventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren      -at- anl.gov>
 *  @author John Detek Chapman <chapman       -at- hep.phy.cam.ac.uk>
 *  @author Miha Muskinja      <miha.muskinja -at- cern.ch>
 **/

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PersistentDataModel/Guid.h"

#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaBaseComps/AthService.h"

#include <map>

// Forward declarations
namespace pool {
  class ICollectionCursor;
}
class IIncidentSvc;
class IOpaqueAddress;
class ISvcLocator;
class EventContextAthenaPool;
class PoolCollectionConverter;
class IAthenaIPCTool;
class ActiveStoreSvc;
class StoreGateSvc;
class IAthenaPoolCnvSvc;
class IIoComponentMgr;

/** @class DoubleEventSelectorAthenaPool
 *  @brief This class is the EventSelector for event data.
 **/
class DoubleEventSelectorAthenaPool :
  //public extends<AthService, IEvtSelector, IEvtSelectorSeek, IEventShare, IIoComponent, IIncidentListener> {
  public ::AthService,
  virtual public IEvtSelector,
  virtual public IEvtSelectorSeek,
  virtual public IEventShare,
  virtual public IIoComponent,
  virtual public IIncidentListener
{

public: // Constructor and Destructor
  /// Standard Service Constructor
  DoubleEventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~DoubleEventSelectorAthenaPool();

  /// Required of all Gaudi Services
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override;
  /// Does this object satisfy a given interface?  See Gaudi documentation for details.
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  virtual StatusCode createContext(IEvtSelector::Context*& ctxt) const override;

  /// @param ctxt [IN/OUT] current event context is interated to next event.
  virtual StatusCode next(IEvtSelector::Context& ctxt) const override;
  /// @param ctxt [IN/OUT] current event context is interated to next event.
  /// @param jump [IN] number of events to jump (currently not supported).
  virtual StatusCode next(IEvtSelector::Context& ctxt, int jump) const override;

  /// @param ctxt [IN/OUT] current event context is interated to previous event.
  virtual StatusCode previous(IEvtSelector::Context& ctxt) const override;
  /// @param ctxt [IN/OUT] current event context is interated to previous event.
  /// @param jump [IN] number of events to jump (currently not supported).
  virtual StatusCode previous(IEvtSelector::Context& ctxt, int jump) const override;

  /// @param ctxt [IN/OUT] current event context is interated to last event.
  virtual StatusCode last(IEvtSelector::Context& ctxt) const override;

  /// @param ctxt [IN/OUT] current event context is rewound to first event.
  virtual StatusCode rewind(IEvtSelector::Context& ctxt) const override;

  /// @param ctxt [IN] current event context.
  /// @param iop [OUT] pointer to IOpaqueAddress reference of selection context.
  virtual StatusCode createAddress(const IEvtSelector::Context& ctxt, IOpaqueAddress*& iop) const override;

  /// @param ctxt [IN] current event context is released.
  virtual StatusCode releaseContext(IEvtSelector::Context*& ctxt) const override;

  /// Set a selection criteria.
  /// @param criteria filter predicate (SQL-style WHERE clause)
  /// @param ctxt [IN] current event context.
  virtual StatusCode resetCriteria(const std::string& criteria, IEvtSelector::Context& ctxt) const override;

  /// Seek to a given event number.
  /// @param ctxt [IN/OUT] current event context.
  /// @param evtnum [IN]  The event number to which to seek.
  virtual StatusCode seek(Context& ctxt, int evtnum) const override;

  /// Return the current event number.
  /// @param ctxt [IN/OUT] current event context.
  virtual int curEvent (const Context& ctxt) const override;

  /// Make this a server.
  virtual StatusCode makeServer(int num) override;

  /// Make this a client.
  virtual StatusCode makeClient(int num) override;

  /// Request to share a given event number.
  /// @param evtnum [IN]  The event number to share.
  virtual StatusCode share(int evtnum) override;

  /// Read the next maxevt events.
  /// @param evtnum [IN]  The number of events to read.
  virtual StatusCode readEvent(int maxevt) override;

  /// Return the size of the collection.
  /// @param ctxt [IN/OUT] current event context.
  virtual int size (Context& ctxt) const override;

  /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
  virtual StatusCode io_reinit() override;
  /// Callback method to finalize the internal state of the component for I/O purposes (e.g. before @c fork(2))
  virtual StatusCode io_finalize() override;

  /// Incident service handle listening for BeginProcessing and EndProcessing
  virtual void handle(const Incident& incident) override;

private: // internal member functions
  /// Register input files' names with I/O Manager
  StatusCode registerInputFileNames(const std::vector<std::string>& incol, IIoComponentMgr* iomgr);
  /// Return pointer to active event SG
  StoreGateSvc* eventStore() const;
  /// Reinitialize the service when a @c fork() occured/was-issued
  StatusCode reinit();
  /// Reinitialize either the primary or the secondary input
  StatusCode reinitSingle(const std::vector<std::string>& inputCollections,
                          std::vector<int>& numEvt, std::vector<int>& firstEvt,
                          Guid& inputGuid,
                          std::vector<std::string>::const_iterator& inputCollectionsIterator,
                          long& curCollection, bool postInitHelperTools,
                          ToolHandle<IAthenaIPCTool>& eventStreamingTool,
                          PoolCollectionConverter*& poolCollectionConverter,
                          bool processMetadata,
                          bool& firedIncident,
                          pool::ICollectionCursor*& headerIterator);
  /// Return pointer to new PoolCollectionConverter for the Input
  PoolCollectionConverter* getCollectionCnv(std::vector<std::string>::const_iterator& inputCollectionsIterator,
                                            const std::vector<std::string>& inputCollections,
                                            long& curCollection, std::vector<int>& numEvt, std::vector<int>& firstEvt,
                                            bool processMetadata, bool throwIncidents = false) const;
  /// Record AttributeList in StoreGate
  StatusCode recordAllAttributeList() const;

  /// Search for event number evtNum.
  int findEvent(int evtNum, std::vector<int>& numEvt, std::vector<int>& firstEvt, const std::vector<std::string>& inputCollections) const;

  /// Fires the EndInputFile incident (if there is an open file) at end of selector
  void fireEndFileIncidents(bool processMetadata, const Guid& inputGuid, bool isLastFile, bool& firedIncident) const;

  // Disconnect DB if all events from the source FID were processed and the Selector moved to another file
  bool disconnectIfFinished( SG::SourceID fid ) const;

private: // data
  mutable EventContextAthenaPool*      m_beginIter{};
  EventContextAthenaPool*      m_endIter{};

  ServiceHandle<ActiveStoreSvc> m_activeStoreSvc;

  mutable PoolCollectionConverter* m_primaryPoolCollectionConverter{};
  mutable pool::ICollectionCursor* m_primaryHeaderIterator{};
  mutable Guid m_primaryGuid{};
  mutable PoolCollectionConverter* m_secondaryPoolCollectionConverter{};
  mutable pool::ICollectionCursor* m_secondaryHeaderIterator{};
  mutable Guid m_secondaryGuid{};
  mutable std::map<SG::SourceID, int> m_activeEventsPerSource;

  ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;
  ServiceHandle<IIncidentSvc> m_incidentSvc;

private: // properties
  /// ProcessMetadata, switch on firing of FileIncidents which will trigger processing of metadata: default = true.
  Gaudi::Property<bool> m_processPrimaryMetadata;
  Gaudi::Property<bool> m_processSecondaryMetadata;
  /// ShowSizeStat, show size statistics from POOL for all persistified objects: default = false.
  Gaudi::Property<bool> m_showSizeStat;
  /// CollectionType, type of the collection: default = "ImplicitROOT".
  Gaudi::Property<std::string> m_collectionType;
  /// CollectionTree, prefix of the collection TTree: default = "POOLContainer_".
  Gaudi::Property<std::string> m_collectionTree;
  /// Connection, connection string.
  Gaudi::Property<std::string> m_connection;
  /// RefName, attribute name.
  Gaudi::Property<std::string> m_refName;
  Gaudi::Property<std::string> m_derRefName;
  /// AttributeList SG key
  Gaudi::Property<std::string> m_attrListKey;
  Gaudi::Property<std::string> m_secondaryAttrListSuffix;
  /// InputCollections, vector with names of the input collections.
  Gaudi::Property<std::vector<std::string>> m_primaryInputCollectionsProp;
  mutable std::vector<std::string>::const_iterator m_primaryInputCollectionsIterator;
  Gaudi::Property<std::vector<std::string>> m_secondaryInputCollectionsProp;
  mutable std::vector<std::string>::const_iterator m_secondaryInputCollectionsIterator;
  void inputCollectionsHandler(Property&); // TODO does this need duplicating?
  /// Query, query string.
  Gaudi::Property<std::string> m_query;

  /// KeepInputFilesOpen, boolean flag to keep files open after PoolCollection reaches end: default = false.
  /// Needed for PilUp to run without PoolFileCatalog. Relies on POOL to close files when reaching DB_AGE_LIMIT.
  Gaudi::Property<bool> m_keepInputFilesOpen;

  /// HelperTools, vector of names of AlgTools that are executed by the EventSelector
  ToolHandleArray<IAthenaSelectorTool> m_helperTools;
  ToolHandle<IAthenaSelectorTool> m_counterTool;
  ToolHandle<IAthenaIPCTool> m_primaryEventStreamingTool;
  ToolHandle<IAthenaIPCTool> m_secondaryEventStreamingTool;

  /// The following are included for compatibility with McEventSelector and are not really used.
  /// However runNo, oldRunNo and overrideRunNumberFromInput are used to reset run number for
  /// simulated events, needed to use conditions
  Gaudi::CheckedProperty<int> m_runNo;
  Gaudi::CheckedProperty<int> m_oldRunNo;
  Gaudi::Property<bool> m_overrideRunNumberFromInput;
  Gaudi::CheckedProperty<int> m_firstEventNo;
  Gaudi::CheckedProperty<int> m_eventsPerRun;
  Gaudi::CheckedProperty<int> m_firstLBNo;
  Gaudi::CheckedProperty<int> m_eventsPerLB;
  Gaudi::CheckedProperty<int> m_initTimeStamp;
  Gaudi::Property<int> m_timeStampInterval;

  /// Flags to indicate override of run/event/time
  /// These are almost always false.
  Gaudi::Property<bool> m_overrideRunNumber;
  Gaudi::Property<bool> m_overrideEventNumber;
  Gaudi::Property<bool> m_overrideTimeStamp;

  mutable long m_curPrimaryCollection;
  mutable std::vector<int> m_numPrimaryEvt;
  mutable std::vector<int> m_firstPrimaryEvt;

  mutable long m_curSecondaryCollection;
  mutable std::vector<int> m_numSecondaryEvt;
  mutable std::vector<int> m_firstSecondaryEvt;

  /// SkipEvents, numbers of events to skip: default = 0.
  Gaudi::Property<int> m_skipEvents;
  Gaudi::Property<std::vector<long>> m_skipEventSequenceProp;
  mutable std::vector<long> m_skipEventSequence;

  mutable int m_evtCount; // internal count of events
  mutable bool m_firedPrimaryIncident{false};
  mutable bool m_firedSecondaryIncident{false};

  typedef std::mutex CallMutex;
  mutable CallMutex m_callLock;
};

#endif
