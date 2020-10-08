/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTORBYTESTREAM_H
#define EVENTSELECTORBYTESTREAM_H

/**  @class  EventSelectorByteStream
     @brief  concrete implementation of IEvtSelector for ByteStream

     An event selector service that steps through Event
     Filter packed raw events by use of an event source
     object and an iterator object called EventIteratorByteStream.
     The type of event source is specified at run time. It
     can be a single file, a set of files or the network.
//---------------------------------------------------------------------
*/

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIoComponent.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/ISecondaryEventSelector.h"
#include "AthenaBaseComps/AthService.h"

#include "ByteStreamData/RawEvent.h"

// Forward declarations.
class ISvcLocator;
class EventContextByteStream;
class ByteStreamInputSvc;
class IROBDataProviderSvc;

// Class EventSelectorByteStream.
class EventSelectorByteStream :
   public extends<::AthService, IEvtSelector, IEvtSelectorSeek, IEventShare, IIoComponent, ISecondaryEventSelector>
{
public:
   /// Standard Constructor.
   EventSelectorByteStream(const std::string& name, ISvcLocator* svcloc);
   /// Standard Destructor.
   virtual ~EventSelectorByteStream();

   /// Implementation of Service base class methods.
   virtual StatusCode initialize() override;
   virtual StatusCode start() override;
   virtual StatusCode stop() override;
   virtual StatusCode finalize() override;

   // Implementation of IInterface methods.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   //-------------------------------------------------
   // IEventSelector
   /// @brief create context
   virtual StatusCode createContext(Context*& it) const override;
   /// @param it [IN/OUT] current event context is interated to next event.
   virtual StatusCode next(Context& it) const override;
   /// @param it [IN/OUT] current event context is interated to next event.
   /// @param jump [IN] number of events to jump (currently not supported).
   virtual StatusCode next(Context& it, int jump) const override;
   /// @param it [IN/OUT] current event context is interated to previous event.
   virtual StatusCode previous(Context& it) const override;
   /// @param it [IN/OUT] current event context is interated to previous event.
   /// @param jump [IN] number of events to jump (currently not supported).
   virtual StatusCode previous(Context& it, int jump) const override;

   /// @param it [IN/OUT] current event context is interated to last event.
   virtual StatusCode last(Context& it) const override;
   /// @param it [IN/OUT] current event context is rewound to first event.
   virtual StatusCode rewind(Context& it) const override;

   /// @param it [IN] current event context.
   /// @param iop [OUT] pointer to IOpaqueAddress reference of selection context.
   virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const override;

   /// @param it [IN] current event context is released.
   virtual StatusCode releaseContext(Context*& it) const override;

   /// Set a selection criteria.
   /// @param criteria filter predicate (SQL-style WHERE clause)
   /// @param context [IN] current event context.
   virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const override;

   //-------------------------------------------------
   // IEventSelectorSeek
   /// Seek to a given event number.
   /// @param it [IN/OUT] current event context.
   /// @param evtnum [IN]  The event number to which to seek.
   virtual StatusCode seek (Context& it, int evtnum) const override;

   /// Return the current event number.
   /// @param it [IN/OUT] current event context.
   virtual int curEvent (const Context& it) const override;

   /// Always returns -1.
   /// @param it [IN/OUT] current event context.
   virtual int size (Context& it) const override;

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

protected:
   //-------------------------------------------------
   // ISecondaryEventSelector
   /// Handle file transition at the next iteration
   virtual StatusCode nextHandleFileTransition(IEvtSelector::Context& it) const override;
   /// Sync event count
   virtual void syncEventCount(int count) const override;
   /// Record AttributeList in StoreGate
   virtual StatusCode recordAttributeList() const override;
   /// Fill AttributeList with specific items from the selector and a suffix
   virtual StatusCode fillAttributeList(coral::AttributeList *attrList, const std::string &suffix, bool copySource) const override;
   // Disconnect DB if all events from the source FID were processed and the Selector moved to another file
   virtual bool disconnectIfFinished(const SG::SourceID &fid) const override;

private: // internal member functions
   /// Reinitialize the service when a @c fork() occured/was-issued
   StatusCode reinit();
   StatusCode openNewRun() const;
   void nextFile() const; 
   /// Search for event with number evtNum.
   int findEvent(int evtNum) const;
   StoreGateSvc* eventStore() const;

private: // properties
   /// IsSecondary, know if this is an instance of secondary event selector
   Gaudi::Property<bool> m_isSecondary{this, "IsSecondary", false, ""};

   Gaudi::Property<std::string> m_eventSourceName{this, "ByteStreamInputSvc", "", ""};
   Gaudi::Property<bool> m_procBadEvent{this, "ProcessBadEvent", false, ""}; //!< process bad events, which fail check_tree().
   Gaudi::Property<int>  m_maxBadEvts{this, "MaxBadEvents", -1, ""};         //!< number of bad events allowed before quitting.
   mutable int           m_fileCount{};  //!< number of files to process.

   mutable std::vector<int>     m_numEvt;
   mutable std::vector<int>     m_firstEvt;

   EventContextByteStream*  m_beginIter{};
   EventContextByteStream*  m_endIter{};
   ByteStreamInputSvc*      m_eventSource{};
   Gaudi::Property<std::vector<std::string>> m_inputCollectionsProp{this, "Input", {}, ""};
   mutable std::vector<std::string>::const_iterator m_inputCollectionsIterator;
   void inputCollectionsHandler(Gaudi::Details::PropertyBase&);
   ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentSvc", "IncidentSvc", ""};
   ServiceHandle<ActiveStoreSvc> m_activeStoreSvc;

   Gaudi::Property<long> m_skipEvents{this, "SkipEvents", 0, ""}; // Number of events to skip at the beginning
   Gaudi::Property<std::vector<long>> m_skipEventSequenceProp{this, "SkipEventSequence", {}, ""};
   mutable std::vector<long> m_skipEventSequence;

   bool m_firstFileFired{};
   bool m_beginFileFired{};
   bool m_inputCollectionsFromIS{};
   mutable long m_NumEvents{}; // Number of Events read so far.

   mutable ToolHandle<IAthenaIPCTool> m_eventStreamingTool{this, "SharedMemoryTool", "", ""};

   /// HelperTools, vector of names of AlgTools that are executed by the EventSelector
   ToolHandleArray<IAthenaSelectorTool> m_helperTools{this};
   ToolHandle<IAthenaSelectorTool>      m_counterTool{this, "CounterTool", "", ""};

   /// The following are included for compatibility with McEventSelector and are not really used.
   /// However runNo, oldRunNo and overrideRunNumberFromInput are used to reset run number for
   /// simulated events, needed to use condition
   Gaudi::Property<bool> m_overrideRunNumber{this, "OverrideRunNumber", false, ""};
   Gaudi::Property<bool> m_filebased{this, "FileBased", true, ""};

   Gaudi::CheckedProperty<int> m_runNo{this, "RunNumber", 0, ""};
   Gaudi::CheckedProperty<int> m_firstEventNo{this, "FirstEvent", 0, ""};
   Gaudi::CheckedProperty<int> m_eventsPerRun{this, "EventsPerRun", 1000000, ""};
   Gaudi::CheckedProperty<int> m_firstLBNo{this, "FirstLB", 0, ""};
   Gaudi::CheckedProperty<int> m_eventsPerLB{this, "EventsPerLB", 1000, ""};
   Gaudi::CheckedProperty<int> m_initTimeStamp{this, "InitialTimeStamp", 0, ""};
   Gaudi::Property<int> m_timeStampInterval{this, "TimeStampInterval", 0, ""};
};

#endif
