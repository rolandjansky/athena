/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaBaseComps/AthService.h"

#include "ByteStreamData/RawEvent.h"

// Forward declarations.
class ISvcLocator;
class IAthenaIPCTool;
class EventContextByteStream;
class ByteStreamInputSvc;
class IROBDataProviderSvc;

// Class EventSelectorByteStream.
class EventSelectorByteStream : public ::AthService,
		virtual public IEvtSelector,
		virtual public IEvtSelectorSeek,
		virtual public IEventShare,
		virtual public IIoComponent {
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

   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode createContext(Context*& it) const override;
   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode next(Context& it) const override;
   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode next(Context& it, int jump) const override;
   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode previous(Context& it) const override;
   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode previous(Context& it, int jump) const override;

   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode last(Context& it) const override;
   /// Implementation of the IEvtSelector interface methods.
   virtual StatusCode rewind(Context& it) const override;

   virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const override;
   virtual StatusCode releaseContext(Context*& it) const override;
   virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const override;

   /// Seek to a given event number.
   /// @param evtnum [IN]  The event number to which to seek.
   virtual StatusCode seek (Context& it, int evtnum) const override;

   /// Return the current event number.
   virtual int curEvent (const Context& it) const override;

   /// Always returns -1.
   virtual int size (Context& it) const override;

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

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit() override;

private: // internal member functions
   /// reinitialize the service when a @c fork() occured/was-issued
   StatusCode reinit();
   StatusCode openNewRun() const;
   void nextFile() const; 
   int findEvent(int evtNum) const; //>! Search for event number evtNum.
   StatusCode buildEventAttributeList() const;

private:
   // property
   std::string      m_eventSourceName;
   Gaudi::Property<int>  m_maxBadEvts;    //!< number of bad events allowed before quitting.
   mutable int              m_fileCount;     //!< number of files to process.

   mutable std::vector<int>     m_numEvt;
   mutable std::vector<int>     m_firstEvt;

   EventContextByteStream*  m_beginIter;
   EventContextByteStream*  m_endIter;
   ByteStreamInputSvc*      m_eventSource;
   Gaudi::Property<std::vector<std::string>> m_inputCollectionsProp;
   mutable std::vector<std::string>::const_iterator m_inputCollectionsIterator;
   void inputCollectionsHandler(Property&);
   ServiceHandle<IIncidentSvc> m_incidentSvc;
   ServiceHandle<StoreGateSvc> m_evtStore;

   long m_SkipEvents; // Number of events to skip at the beginning
   Gaudi::Property<std::vector<long>> m_skipEventSequenceProp;
   mutable std::vector<long> m_skipEventSequence;

   bool m_firstFileFired;
   bool m_beginFileFired;
   bool m_inputCollectionsFromIS;
   mutable long m_NumEvents; // Number of Events read so far.

   mutable ToolHandle<IAthenaIPCTool> m_eventStreamingTool;

   Gaudi::Property<bool>    m_procBadEvent;  //!< process bad events, which fail check_tree().

   /// HelperTools, vector of names of AlgTools that are executed by the EventSelector
   ToolHandleArray<IAthenaSelectorTool> m_helperTools;
   ToolHandle<IAthenaSelectorTool> m_counterTool;
         
   /**@name Athena standard EventSelector properties */

   /// Flags to indicate override of run/event/time
   ///  These are almost always false.
   Gaudi::Property<bool> m_overrideRunNumber;
   Gaudi::Property<bool> m_overrideEventNumber;
   Gaudi::Property<bool> m_overrideTimeStamp;
   Gaudi::Property<bool> m_filebased;

   Gaudi::CheckedProperty<int> m_runNo;
   Gaudi::CheckedProperty<int> m_firstEventNo;
   Gaudi::CheckedProperty<int> m_eventsPerRun;
   Gaudi::CheckedProperty<int> m_firstLBNo;
   Gaudi::CheckedProperty<int> m_eventsPerLB;
   Gaudi::CheckedProperty<int> m_initTimeStamp;
   Gaudi::Property<int> m_timeStampInterval;
};

#endif
