/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTORATHENAPOOL_H
#define EVENTSELECTORATHENAPOOL_H

/** @file EventSelectorAthenaPool.h
 *  @brief This file contains the class definition for the EventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PersistentDataModel/Guid.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/ISecondaryEventSelector.h"
#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "AthenaBaseComps/AthService.h"

#include <map>
#include <atomic>

// Forward declarations
class IIncidentSvc;
class IOpaqueAddress;
class ISvcLocator;
class EventContextAthenaPool;
class PoolCollectionConverter;
class ActiveStoreSvc;
class StoreGateSvc;
namespace pool {
   class ICollectionCursor;
}

/** @class EventSelectorAthenaPool
 *  @brief This class is the EventSelector for event data.
 **/
class EventSelectorAthenaPool :
  public extends<::AthService, IEvtSelector, IEvtSelectorSeek, IEventShare, IIoComponent, IIncidentListener, ISecondaryEventSelector>
{

public: // Constructor and Destructor
   /// Standard Service Constructor
   EventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~EventSelectorAthenaPool();

   /// Required of all Gaudi Services
   virtual StatusCode initialize() override;
   virtual StatusCode start() override;
   virtual StatusCode stop() override;
   virtual StatusCode finalize() override;
   /// Does this object satisfy a given interface?  See Gaudi documentation for details.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   //-------------------------------------------------
   // IEventSelector
   /// @brief create context
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

   //-------------------------------------------------
   // IEventSelectorSeek
   /// Seek to a given event number.
   /// @param ctxt [IN/OUT] current event context.
   /// @param evtnum [IN]  The event number to which to seek.
   virtual StatusCode seek(Context& ctxt, int evtnum) const override;

   /// Return the current event number.
   /// @param ctxt [IN/OUT] current event context.
   virtual int curEvent (const Context& ctxt) const override;

   /// Return the size of the collection.
   /// @param ctxt [IN/OUT] current event context.
   virtual int size (Context& ctxt) const override;

   //-------------------------------------------------
   // IEventShare
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

   //-------------------------------------------------
   // IIoComponent
   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit() override;
   /// Callback method to finalize the internal state of the component for I/O purposes (e.g. before @c fork(2))
   virtual StatusCode io_finalize() override;

   //-------------------------------------------------
   // IIncidentListener
   /// Incident service handle listening for BeginProcessing and EndProcessing
   virtual void handle(const Incident& incident) override;

protected:
   //-------------------------------------------------
   // ISecondaryEventSelector
   /// Handle file transition at the next iteration
   virtual StatusCode nextHandleFileTransition(IEvtSelector::Context& ctxt) const override;
   /// Sync event count
   virtual void syncEventCount(int count) const override;
   /// Record AttributeList in StoreGate
   virtual StatusCode recordAttributeList() const override;
   /// Fill AttributeList with specific items from the selector and a suffix
   virtual StatusCode fillAttributeList(coral::AttributeList *attrList, const std::string &suffix, bool copySource) const override;
   // Disconnect DB if all events from the source FID were processed and the Selector moved to another file
   virtual bool disconnectIfFinished(const SG::SourceID &fid) const override;

private: // internal member functions
   /// Return pointer to active event SG
   StoreGateSvc* eventStore() const;
   /// Reinitialize the service when a @c fork() occured/was-issued
   StatusCode reinit();
   /// Return pointer to new PoolCollectionConverter
   PoolCollectionConverter* getCollectionCnv(bool throwIncidents = false) const;
   /// Search for event with number evtNum.
   int findEvent(int evtNum) const;
   /// Fires the EndInputFile incident (if there is an open file) at end of selector
   void fireEndFileIncidents(bool isLastFile) const;

private: // data
   EventContextAthenaPool*         m_endIter{};

   ServiceHandle<ActiveStoreSvc> m_activeStoreSvc{this, "ActiveStoreSvc", "ActiveStoreSvc", ""};

   mutable PoolCollectionConverter* m_poolCollectionConverter ATLAS_THREAD_SAFE {};
   mutable pool::ICollectionCursor* m_headerIterator ATLAS_THREAD_SAFE {};
   mutable Guid m_guid ATLAS_THREAD_SAFE {};
   mutable std::map<SG::SourceID, int> m_activeEventsPerSource ATLAS_THREAD_SAFE;

   ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc{this, "AthenaPoolCnvSvc", "AthenaPoolCnvSvc", ""};
   ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentSvc", "IncidentSvc", ""};

private: // properties
   /// IsSecondary, know if this is an instance of secondary event selector
   Gaudi::Property<bool> m_isSecondary{this, "IsSecondary", false, ""};
   /// ProcessMetadata, switch on firing of FileIncidents which will trigger processing of metadata: default = true.
   Gaudi::Property<bool> m_processMetadata{this, "ProcessMetadata", true, ""};
   /// CollectionType, type of the collection: default = "ImplicitROOT".
   Gaudi::Property<std::string> m_collectionType{this, "CollectionType", "ImplicitROOT", ""};
   /// CollectionTree, prefix of the collection TTree: default = "POOLContainer_".
   Gaudi::Property<std::string> m_collectionTree{this, "CollectionTree", "POOLContainer", ""};
   /// Connection, connection string.
   // TODO: check if really not used anywhere
   Gaudi::Property<std::string> m_connection{this, "Connection", "", ""};
   /// RefName, attribute name.
   Gaudi::Property<std::string> m_refName{this, "RefName", "", ""};
   /// AttributeList SG key
   Gaudi::Property<std::string> m_attrListKey{this, "AttributeListKey", "Input", ""};
   /// InputCollections, vector with names of the input collections.
   Gaudi::Property<std::vector<std::string>> m_inputCollectionsProp{this, "InputCollections", {}, ""};
   mutable std::vector<std::string>::const_iterator m_inputCollectionsIterator ATLAS_THREAD_SAFE;
   void inputCollectionsHandler(Gaudi::Details::PropertyBase&);
   /// Query, query string.
   Gaudi::Property<std::string> m_query{this, "Query", "", ""};

   /// KeepInputFilesOpen, boolean flag to keep files open after PoolCollection reaches end: default = false.
   /// Needed for PilUp to run without PoolFileCatalog. Relies on POOL to close files when reaching DB_AGE_LIMIT.
   Gaudi::Property<bool> m_keepInputFilesOpen{this, "KeepInputFilesOpen", false, ""};

   /// HelperTools, vector of names of AlgTools that are executed by the EventSelector
   ToolHandleArray<IAthenaSelectorTool> m_helperTools{this};
   ToolHandle<IAthenaSelectorTool> m_counterTool{this, "CounterTool", "", ""};
   ToolHandle<IAthenaIPCTool> m_eventStreamingTool{this, "SharedMemoryTool", "", ""};

   /// The following are included for compatibility with McEventSelector and are not really used.
   /// However runNo, oldRunNo and overrideRunNumberFromInput are used to reset run number for
   /// simulated events, needed to use condition
   Gaudi::CheckedProperty<int> m_runNo{this, "RunNumber", 0, ""};
   Gaudi::CheckedProperty<int> m_oldRunNo{this, "OldRunNumber", 0, ""};
   Gaudi::Property<bool> m_overrideRunNumber{this, "OverrideRunNumber", false, ""};
   Gaudi::Property<bool> m_overrideRunNumberFromInput{this, "OverrideRunNumberFromInput", false, ""};
   // TODO: check if not really used
   Gaudi::CheckedProperty<int> m_firstEventNo{this, "FirstEvent", 0, ""};
   // TODO: check if not really used
   Gaudi::CheckedProperty<int> m_eventsPerRun{this, "EventsPerRun", 1000000, ""};
   Gaudi::CheckedProperty<int> m_firstLBNo{this, "FirstLB", 0, ""};
   Gaudi::CheckedProperty<int> m_eventsPerLB{this, "EventsPerLB", 1000, ""};
   Gaudi::CheckedProperty<int> m_initTimeStamp{this, "InitialTimeStamp", 0, ""};
   Gaudi::Property<int> m_timeStampInterval{this, "TimeStampInterval", 0, ""};

   mutable std::atomic_long m_curCollection{};
   mutable std::vector<int> m_numEvt ATLAS_THREAD_SAFE;
   mutable std::vector<int> m_firstEvt ATLAS_THREAD_SAFE;

   /// SkipEvents, numbers of events to skip: default = 0.
   Gaudi::Property<int> m_skipEvents{this, "SkipEvents", 0, ""};
   Gaudi::Property<std::vector<long>> m_skipEventSequenceProp{this, "SkipEventSequence", {}, ""};
   mutable std::vector<long> m_skipEventSequence ATLAS_THREAD_SAFE;

   mutable std::atomic_int m_evtCount{}; // internal count of events
   mutable std::atomic_bool m_firedIncident{};

   typedef std::mutex CallMutex;
   mutable CallMutex m_callLock;

   /// @brief make the @c DoubleEventSelectorAthenaPool a friend so it can access the 
   ///        internal @c EventSelectorAthenaPool methods and members
   friend class DoubleEventSelectorAthenaPool;
};

#endif
