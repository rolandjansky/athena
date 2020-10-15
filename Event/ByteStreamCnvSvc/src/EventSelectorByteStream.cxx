/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
#include "EventSelectorByteStream.h"

#include <vector>
#include <algorithm>

#include "EventContextByteStream.h"
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvc/ByteStreamExceptions.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "EventInfo/EventInfo.h"

// EventInfoAttributeList includes
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "EventInfo/TriggerInfo.h"
#include "PersistentDataModel/DataHeader.h"
#include "eformat/StreamTag.h"


// Constructor.
EventSelectorByteStream::EventSelectorByteStream(
    const std::string& name,
    ISvcLocator* svcloc)
  : base_class(name, svcloc)
  , m_activeStoreSvc("ActiveStoreSvc", name) {
  declareProperty("HelperTools", m_helperTools);

  // RunNumber, OldRunNumber and OverrideRunNumberFromInput are used
  // to override the run number coming in on the input stream
  m_runNo.verifier().setLower(0);
  // The following properties are only for compatibility with
  // McEventSelector and are not really used anywhere
  // TODO(berghaus): validate if those are even used
  m_eventsPerRun.verifier().setLower(0);
  m_firstEventNo.verifier().setLower(0);
  m_firstLBNo.verifier().setLower(0);
  m_eventsPerLB.verifier().setLower(0);
  m_initTimeStamp.verifier().setLower(0);

  m_inputCollectionsProp.declareUpdateHandler(
      &EventSelectorByteStream::inputCollectionsHandler,
      this);
}


/******************************************************************************/
void EventSelectorByteStream::inputCollectionsHandler(Gaudi::Details::PropertyBase&) {
  if (this->FSMState() != Gaudi::StateMachine::OFFLINE) {
    this->reinit().ignore();
  }
}


/******************************************************************************/
EventSelectorByteStream::~EventSelectorByteStream() {
}


/******************************************************************************/
StoreGateSvc*
EventSelectorByteStream::eventStore() const {
  if (m_activeStoreSvc == 0) {
    if (!m_activeStoreSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Cannot get ActiveStoreSvc");
      throw GaudiException(
          "Cannot get ActiveStoreSvc", name(), StatusCode::FAILURE);
    }
  }

  return(m_activeStoreSvc->activeStore());
}


//________________________________________________________________________________
StatusCode EventSelectorByteStream::initialize() {
   if (m_isSecondary.value()) {
      ATH_MSG_DEBUG("Initializing secondary event selector " << name());
   } else {
      ATH_MSG_DEBUG("Initializing " << name());
   }

   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Check for input setting
   if (m_filebased && m_inputCollectionsProp.value().empty()) {
     ATH_MSG_FATAL("Unable to retrieve valid input list");
     return(StatusCode::FAILURE);
   }
   m_skipEventSequence = m_skipEventSequenceProp.value();
   std::sort(m_skipEventSequence.begin(), m_skipEventSequence.end());

   // Check ByteStreamCnvSvc
   IService* svc;
   if (!serviceLocator()->getService(m_eventSourceName.value(), svc).isSuccess()) {
      ATH_MSG_FATAL("Cannot get ByteStreamInputSvc");
      return(StatusCode::FAILURE);
   }
   m_eventSource = dynamic_cast<ByteStreamInputSvc*>(svc);
   if (m_eventSource == 0) {
      ATH_MSG_FATAL("Cannot cast ByteStreamInputSvc");
      return(StatusCode::FAILURE);
   }

   // Get CounterTool (if configured)
   if (!m_counterTool.empty()) {
      if (!m_counterTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get CounterTool.");
         return(StatusCode::FAILURE);
      }
   }
   // Get HelperTools
   if (!m_helperTools.empty()) {
      if (!m_helperTools.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get " << m_helperTools);
         return(StatusCode::FAILURE);
      }
   }
   // Get SharedMemoryTool (if configured)
   if (!m_eventStreamingTool.empty() && !m_eventStreamingTool.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaSharedMemoryTool");
      return(StatusCode::FAILURE);
   }

   // Register this service for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot retrieve IoComponentMgr.");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Cannot register myself with the IoComponentMgr.");
      return(StatusCode::FAILURE);
   }

   // Register the input files with the iomgr
   bool allGood = true;
   const std::vector<std::string>& incol = m_inputCollectionsProp.value();
   for (std::size_t icol = 0, imax = incol.size(); icol != imax; ++icol) {
      if (!iomgr->io_register(this, IIoComponentMgr::IoMode::READ, incol[icol]).isSuccess()) {
         ATH_MSG_FATAL("could not register [" << incol[icol] << "] for output !");
         allGood = false;
      } else {
         ATH_MSG_VERBOSE("io_register[" << this->name() << "](" << incol[icol] << ") [ok]");
      }
   }
   if (!allGood) {
      return(StatusCode::FAILURE);
   }

   // Must happen before trying to open a file
   StatusCode risc = this->reinit();

   return risc;
}
//__________________________________________________________________________
StatusCode EventSelectorByteStream::reinit() {
   ATH_MSG_INFO("reinitialization...");
   // reset markers
   if (m_inputCollectionsProp.value().size()>0) {
      m_numEvt.resize(m_inputCollectionsProp.value().size(), -1);
      m_firstEvt.resize(m_inputCollectionsProp.value().size(), -1);
   }
   else {
      m_numEvt.resize(1);
      m_firstEvt.resize(1);
   }

   // Initialize InputCollectionsIterator
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_NumEvents = 0;
   bool retError = false;
   if (!m_helperTools.empty()) {
      for (std::vector<ToolHandle<IAthenaSelectorTool> >::iterator iter = m_helperTools.begin(),
           last = m_helperTools.end(); iter != last; iter++) {
         if (!(*iter)->postInitialize().isSuccess()) {
            ATH_MSG_FATAL("Failed to postInitialize() " << (*iter)->name());
            retError = true;
         }
      }
   }
   if (retError) {
      ATH_MSG_FATAL("Failed to postInitialize() helperTools");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::start() {
   ATH_MSG_DEBUG("Calling EventSelectorByteStream::start()");
   // If file based input then fire appropriate incidents
   if (m_filebased) {
      if (!m_firstFileFired) {
         FileIncident firstInputFileIncident(name(), "FirstInputFile", "BSF:" + *m_inputCollectionsIterator);
         m_incidentSvc->fireIncident(firstInputFileIncident);
         m_firstFileFired = true;
      }

      // try to open a file
      if (this->openNewRun().isFailure()) { 
         ATH_MSG_FATAL("Unable to open any file in initialize"); 
         return(StatusCode::FAILURE);
      }
      // should be in openNewRun, but see comment there
      m_beginFileFired = true;
   }

   // Create the begin and end iterator's for this selector.
   m_beginIter =  new EventContextByteStream(this);
   // Increment to get the new event in.
   m_endIter   =  new EventContextByteStream(0);

   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::stop() {
   ATH_MSG_DEBUG("Calling EventSelectorByteStream::stop()");
   // Handle open files
   if (m_filebased) {
      // Close the file
      if (m_eventSource->ready()) { 
         m_eventSource->closeBlockIterator(false);
         FileIncident endInputFileIncident(name(), "EndInputFile", "stop");
         m_incidentSvc->fireIncident(endInputFileIncident);
      }
   }
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
StatusCode EventSelectorByteStream::finalize() {
   if (!m_counterTool.empty()) {
      if (!m_counterTool->preFinalize().isSuccess()) {
         ATH_MSG_WARNING("Failed to preFinalize() CounterTool");
      }
   }
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::iterator iter = m_helperTools.begin(),
        last = m_helperTools.end(); iter != last; iter++) {
      if (!(*iter)->preFinalize().isSuccess()) {
         ATH_MSG_WARNING("Failed to preFinalize() " << (*iter)->name());
      }
   }
   delete m_beginIter; m_beginIter = 0;
   delete m_endIter; m_endIter = 0;
   // Release AthenaSharedMemoryTool
   if (!m_eventStreamingTool.empty() && !m_eventStreamingTool.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaSharedMemoryTool");
   }
   // Release CounterTool
   if (!m_counterTool.empty()) {
      if (!m_counterTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release CounterTool.");
      }
   }
   // Release HelperTools
   if (!m_helperTools.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_helperTools);
   }
   if (m_eventSource) m_eventSource->release();
   // Finalize the Service base class.
   return(AthService::finalize());
}

void EventSelectorByteStream::nextFile() const {
   FileIncident endInputFileIncident(name(), "EndInputFile", "BSF:" + *m_inputCollectionsIterator);
   m_incidentSvc->fireIncident(endInputFileIncident);
   ++m_inputCollectionsIterator;
   ++m_fileCount;
}

StatusCode EventSelectorByteStream::openNewRun() const {
   // Should be protected upstream, but this is further protection
   if (!m_filebased) {
      ATH_MSG_ERROR("cannot open new run for non-filebased inputs");
      return(StatusCode::FAILURE);
   }
   // Check for end of file list
   if (m_inputCollectionsIterator == m_inputCollectionsProp.value().end()) {
      ATH_MSG_INFO("End of input file list reached");
      return(StatusCode::FAILURE);
   }
   std::string blockname = *m_inputCollectionsIterator;
   // try to open a file, if failure go to next FIXME: PVG: silient failure?
   //long nev = m_eventSource->getBlockIterator(blockname);
   auto nevguid = m_eventSource->getBlockIterator(blockname);
   long nev = nevguid.first;
   if (nev == -1) {
      ATH_MSG_FATAL("Unable to access file " << *m_inputCollectionsIterator << ", stopping here");
      throw ByteStreamExceptions::fileAccessError();
   }
   // Fire the incident
   if (!m_beginFileFired) {
     FileIncident beginInputFileIncident(name(), "BeginInputFile", "BSF:" + *m_inputCollectionsIterator,nevguid.second);
     m_incidentSvc->fireIncident(beginInputFileIncident);
     //m_beginFileFired = true;   // Should go here, but can't because IEvtSelector next is const
   }

   // check if file is empty
   if (nev == 0) {
      ATH_MSG_WARNING("no events in file " << blockname << " try next");
      if (m_eventSource->ready()) m_eventSource->closeBlockIterator(true);
      this->nextFile();
      return openNewRun();
   // check if skipping all events in that file (minus events already skipped)
   } else if (m_skipEvents.value() - m_NumEvents > nev) {
      ATH_MSG_WARNING("skipping more events " << m_skipEvents.value() - m_NumEvents << "(" << nev <<") than in file " << *m_inputCollectionsIterator << ", try next");
      m_NumEvents += nev;
      m_numEvt[m_fileCount] = nev;
      if (m_eventSource->ready()) m_eventSource->closeBlockIterator(true);
      this->nextFile();
      return openNewRun();
   }

   ATH_MSG_DEBUG("Opened block/file " << blockname); 
   m_firstEvt[m_fileCount] = m_NumEvents;
   m_numEvt[m_fileCount] = nev;

   return(StatusCode::SUCCESS);
}

StatusCode EventSelectorByteStream::createContext(IEvtSelector::Context*& it) const {
   it = new EventContextByteStream(this);
   return(StatusCode::SUCCESS);
}

StatusCode EventSelectorByteStream::next(IEvtSelector::Context& it) const {
   static int n_bad_events = 0;   // cross loop counter of bad events
   // Check if this is an athenaMP client process
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      void* source = 0;
      unsigned int status = 0;
      if (!m_eventStreamingTool->getLockedEvent(&source, status).isSuccess()) {
         ATH_MSG_FATAL("Cannot get NextEvent from AthenaSharedMemoryTool");
         return(StatusCode::FAILURE);
      }
      m_eventSource->setEvent(static_cast<char*>(source), status);
      return(StatusCode::SUCCESS);
   }
   // Call all selector tool preNext before starting loop
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
                   last = m_helperTools.end(); iter != last; iter++) {
      if (!(*iter)->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() " << (*iter)->name());
      }
   }
   if (!m_counterTool.empty()) {
      if (!m_counterTool->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() CounterTool.");
      }
   }
   // Find an event to return
   for (;;) {
      bool badEvent{};
      StatusCode sc = nextHandleFileTransition(it);
      if (sc.isRecoverable()) {
         badEvent = true;
      } else if (sc.isFailure()) {
         return StatusCode::FAILURE;
      }

      // increment that an event was found
      ++m_NumEvents;

      // check bad event flag and handle as configured 
      if (badEvent) {
         ++n_bad_events;
         ATH_MSG_INFO("Bad event encountered, current count at " << n_bad_events);
         bool toomany = (m_maxBadEvts >= 0 && n_bad_events > m_maxBadEvts);
         if (toomany) {ATH_MSG_FATAL("too many bad events ");}
         if (!m_procBadEvent || toomany) {
            // End of file
            it = *m_endIter;
            return(StatusCode::FAILURE);
         }
         ATH_MSG_WARNING("Continue with bad event");
      }
      // Build a DH for use by other components
      StatusCode rec_sg = m_eventSource->generateDataHeader();
      if (rec_sg != StatusCode::SUCCESS) {
         ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
      }

      // Check whether properties or tools reject this event
      if ( m_NumEvents > m_skipEvents.value() && 
           (m_skipEventSequence.empty() || m_NumEvents != m_skipEventSequence.front()) ) {
         StatusCode status(StatusCode::SUCCESS);
         // Build event info attribute list
         if (recordAttributeList().isFailure()) ATH_MSG_WARNING("Unable to build event info att list");
         for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
		         last = m_helperTools.end(); iter != last; iter++) {
            StatusCode toolStatus = (*iter)->postNext();
            if (toolStatus.isRecoverable()) {
               ATH_MSG_INFO("Request skipping event from: " << (*iter)->name());
               status = StatusCode::RECOVERABLE;
            } else if (toolStatus.isFailure()) {
               ATH_MSG_WARNING("Failed to postNext() " << (*iter)->name());
               status = StatusCode::FAILURE;
            }
         }
         if (status.isRecoverable()) {
            ATH_MSG_INFO("skipping event " << m_NumEvents);
         } else if (status.isFailure()) {
            ATH_MSG_WARNING("Failed to postNext() HelperTool.");
         } else {
               if (!m_counterTool.empty()) {
                  if (!m_counterTool->postNext().isSuccess()) {
                     ATH_MSG_WARNING("Failed to postNext() CounterTool.");
               }
            }
            break;
         }

         // Validate the event
         try {
            m_eventSource->validateEvent();
         }
         catch (const ByteStreamExceptions::badFragmentData&) { 
            ATH_MSG_ERROR("badFragment data encountered");

            ++n_bad_events;
            ATH_MSG_INFO("Bad event encountered, current count at " << n_bad_events);

            bool toomany = (m_maxBadEvts >= 0 && n_bad_events > m_maxBadEvts);
	         if (toomany) {ATH_MSG_FATAL("too many bad events ");}
            if (!m_procBadEvent || toomany) {
               // End of file
	            it = *m_endIter;
	         return(StatusCode::FAILURE);
   	      }
            ATH_MSG_WARNING("Continue with bad event");
         }
      } else {
         if (!m_skipEventSequence.empty() && m_NumEvents == m_skipEventSequence.front()) {
            m_skipEventSequence.erase(m_skipEventSequence.begin());
         }
         ATH_MSG_DEBUG("Skipping event " << m_NumEvents - 1);
      }
   } // for loop
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isServer()) { // For SharedReader Server, put event into SHM
      const RawEvent* pre = 0;
      pre = m_eventSource->currentEvent();
      StatusCode sc = m_eventStreamingTool->putEvent(m_NumEvents - 1, pre->start(), pre->fragment_size_word() * sizeof(uint32_t), m_eventSource->currentEventStatus());
      while (sc.isRecoverable()) {
         usleep(1000);
         sc = m_eventStreamingTool->putEvent(m_NumEvents - 1, pre->start(), pre->fragment_size_word() * sizeof(uint32_t), m_eventSource->currentEventStatus());
      }
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Cannot put Event " << m_NumEvents - 1 << " to AthenaSharedMemoryTool");
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::next(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      if ( m_NumEvents+jump != m_skipEvents.value()) {
         // Save initial event count
         unsigned int cntr = m_NumEvents;
         // In case NumEvents increments multiple times in a single next call
         while (m_NumEvents+1 <= cntr + jump) {
            if (!next(ctxt).isSuccess()) {
               return(StatusCode::FAILURE);
            }
         }
      }
      else ATH_MSG_DEBUG("Jump covered by skip event " << m_skipEvents.value());
      return(StatusCode::SUCCESS);
   }
   else { 
      ATH_MSG_WARNING("Called jump next with non-multiple jump");
   }
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::nextHandleFileTransition(IEvtSelector::Context& it) const
{
   const RawEvent* pre{}; 
   bool badEvent{};
   // if event source not ready from init, try next file
   if (m_filebased && !m_eventSource->ready()) {
      // next file
      this->nextFile();
      if (this->openNewRun().isFailure()) {
         ATH_MSG_DEBUG("Event source found no more valid files left in input list");
         m_NumEvents = -1;
         return StatusCode::FAILURE; 
      }
   }
   try { 
      pre = m_eventSource->nextEvent(); 
   } 
   catch (const ByteStreamExceptions::readError&) { 
      ATH_MSG_FATAL("Caught ByteStreamExceptions::readError"); 
      return StatusCode::FAILURE; 
   } 
   catch (const ByteStreamExceptions::badFragment&) { 
      ATH_MSG_ERROR("badFragment encountered");
      badEvent = true;
   }
   catch (const ByteStreamExceptions::badFragmentData&) { 
      ATH_MSG_ERROR("badFragment data encountered");
      badEvent = true;
   }
   // Check whether a RawEvent has actually been provided
   if (pre == nullptr) {
      it = *m_endIter;
      return StatusCode::FAILURE;
   }

   // If not secondary just return the status code based on if the event is bas
   if (!m_isSecondary.value()) {
      // check bad event flag and handle as configured 
      return badEvent ? StatusCode::RECOVERABLE : StatusCode::SUCCESS;
   } 

   // Build a DH for use by other components
   StatusCode rec_sg = m_eventSource->generateDataHeader();
   if (rec_sg != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
   }

   return StatusCode::SUCCESS;
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::previous(IEvtSelector::Context& /*ctxt*/) const {
    ATH_MSG_DEBUG(" ... previous");
    const RawEvent* pre = 0; 
    bool badEvent(false);
    // if event source not ready from init, try next file
    if (m_eventSource->ready()) {
       try { 
          pre = m_eventSource->previousEvent(); 
       } 
       catch (const ByteStreamExceptions::readError&) { 
          ATH_MSG_FATAL("Caught ByteStreamExceptions::readError"); 
          return StatusCode::FAILURE; 
       } 
       catch (const ByteStreamExceptions::badFragment&) { 
          ATH_MSG_ERROR("badFragment encountered");
          badEvent = true;
       }
       catch (const ByteStreamExceptions::badFragmentData&) { 
          ATH_MSG_ERROR("badFragment data encountered");
          badEvent = true;
       }
       // Check whether a RawEvent has actually been provided
       if (pre == 0) {
          ATH_MSG_ERROR("No event built");
 	 //it = *m_endIter;
 	 return(StatusCode::FAILURE);
       }
    }
    else {
       ATH_MSG_FATAL("Attempt to read previous data on invalid reader");
       return(StatusCode::FAILURE);
    }
    // increment that an event was found
    //++m_NumEvents;
 
    // check bad event flag and handle as configured 
    if (badEvent) {
       ATH_MSG_ERROR("Called previous for bad event");
       if (!m_procBadEvent) {
          // End of file
          //it = *m_endIter;
          return(StatusCode::FAILURE);
       }
       ATH_MSG_WARNING("Continue with bad event");
    }

    // Build a DH for use by other components
    StatusCode rec_sg = m_eventSource->generateDataHeader();
      if (rec_sg != StatusCode::SUCCESS) {
         ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
    }

    return StatusCode::SUCCESS;
}
//________________________________________________________________________________
StatusCode EventSelectorByteStream::previous(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      for (int i = 0; i < jump; i++) {
         if (!previous(ctxt).isSuccess()) {
            return(StatusCode::FAILURE);
         }
      }
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorByteStream::last(IEvtSelector::Context& it)const {
   if (it.identifier() == m_endIter->identifier()) {
      ATH_MSG_DEBUG("last(): Last event in InputStream.");
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorByteStream::rewind(IEvtSelector::Context& /*it*/) const {
   ATH_MSG_ERROR("rewind() not implemented");
   return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::resetCriteria(const std::string& /*criteria*/, IEvtSelector::Context& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
StatusCode EventSelectorByteStream::seek(Context& it, int evtNum) const {
   // Check that input is seekable
   if (!m_filebased) {
      ATH_MSG_ERROR("Input not seekable, choose different input svc");
      return StatusCode::FAILURE;
   }
   // find the file index with that event
   long fileNum = findEvent(evtNum);
   if (fileNum == -1 && evtNum >= m_firstEvt[m_fileCount] && evtNum < m_NumEvents) {
      fileNum = m_fileCount;
   }
   // if unable to locate file, exit
   if (fileNum == -1) {
      ATH_MSG_INFO("seek: Reached end of Input.");
      return(StatusCode::RECOVERABLE);
   }
   // check if it is the current file
   if (fileNum != m_fileCount) { // event in different file
      // Close input file if open
      if (m_eventSource->ready()) m_eventSource->closeBlockIterator(true);
      ATH_MSG_DEBUG("Seek to item: \"" << m_inputCollectionsProp.value()[fileNum] << "\" from the explicit file list.");
      std::string fileName = m_inputCollectionsProp.value()[fileNum];
      m_fileCount = fileNum;
      // Open the correct file
      auto nevguid = m_eventSource->getBlockIterator(fileName);
      long nev = nevguid.first;
      if (nev == -1) {
         ATH_MSG_FATAL("Unable to open file with seeked event " << evtNum << " file " << fileName);
         return StatusCode::FAILURE;
      }
      int delta = evtNum - m_firstEvt[m_fileCount];
      if (delta > 0) {
        if (next(*m_beginIter,delta).isFailure()) return StatusCode::FAILURE;
      }
      else return this->seek(it, evtNum);
   } 
   else { // event in current file
      int delta = (evtNum - m_firstEvt[m_fileCount] + 1) - m_eventSource->positionInBlock();
      ATH_MSG_DEBUG("Seeking event " << evtNum << " in current file wiht delta " << delta);
      if ( delta == 0 ) { // current event
         // nothing to do
      } 
      else if ( delta > 0 ) { // forward
         if ( this->next(*m_beginIter, delta).isFailure() ) return StatusCode::FAILURE;
      } 
      else if ( delta < 0 ) { // backward
         if ( this->previous(*m_beginIter, -1*delta).isFailure() ) return(StatusCode::FAILURE);
      } 
   }
   return(StatusCode::SUCCESS);
}

StatusCode EventSelectorByteStream::recordAttributeList() const
{
   std::string listName("EventInfoAtts");

   if (eventStore()->contains<AthenaAttributeList>(listName)) {
      const AthenaAttributeList* oldAttrList = nullptr;
      if (!eventStore()->retrieve(oldAttrList, listName).isSuccess()) {
         ATH_MSG_ERROR("Cannot retrieve old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
      if (!eventStore()->removeDataAndProxy(oldAttrList).isSuccess()) {
         ATH_MSG_ERROR("Cannot remove old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
   }

   // build spec and the new attr list
   coral::AttributeListSpecification* spec = new coral::AttributeListSpecification(); // the newly created attribute list owns the spec
   auto attrList = std::make_unique<AthenaAttributeList>(*spec);

   // fill the attr list
   ATH_CHECK(fillAttributeList(attrList.get(), "", false));

   // put result in event store
   if (eventStore()->record(std::move(attrList), listName).isFailure()) {
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode EventSelectorByteStream::fillAttributeList(coral::AttributeList *attrList, const std::string &suffix, bool /* copySource */) const
{
   attrList->extend("RunNumber"   + suffix, "unsigned int");
   attrList->extend("EventNumber" + suffix, "unsigned long long");
   attrList->extend("LumiBlockN"  + suffix, "unsigned int");
   attrList->extend("BunchId"     + suffix, "unsigned int");
   attrList->extend("EventTime"        + suffix, "unsigned int");
   attrList->extend("EventTimeNanoSec" + suffix, "unsigned int");

   // fill attribute list
   const RawEvent* event = m_eventSource->currentEvent();

   (*attrList)["RunNumber" + suffix].data<unsigned int>() = event->run_no();
   if (event->version() < 0x03010000) {
      (*attrList)["EventNumber" + suffix].data<unsigned long long>() = event->lvl1_id();
   } else {
      (*attrList)["EventNumber" + suffix].data<unsigned long long>() = event->global_id();
   }
   (*attrList)["LumiBlockN" + suffix].data<unsigned int>() = event->lumi_block();
   (*attrList)["BunchId" + suffix].data<unsigned int>() = event->bc_id();

   unsigned int bc_time_sec = event->bc_time_seconds();
   unsigned int bc_time_ns  = event->bc_time_nanoseconds();
   // bc_time_ns should be lt 1e9.
   if (bc_time_ns > 1000000000) {
      // round it off to 1e9
      ATH_MSG_WARNING(" bc_time nanosecond number larger than 1e9, it is " << bc_time_ns << ", reset it to 1 sec");
      bc_time_ns = 1000000000;
   }
   (*attrList)["EventTime" + suffix].data<unsigned int>() = bc_time_sec;
   (*attrList)["EventTimeNanoSec" + suffix].data<unsigned int>() = bc_time_ns;

   const OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer;

   event->status(buffer);
   attrList->extend("TriggerStatus" + suffix, "unsigned int");
   (*attrList)["TriggerStatus" + suffix].data<unsigned int>() = *buffer;

   attrList->extend("ExtendedL1ID"  + suffix, "unsigned int");
   attrList->extend("L1TriggerType" + suffix, "unsigned int");
   (*attrList)["ExtendedL1ID" + suffix].data<unsigned int>() = event->lvl1_id();
   (*attrList)["L1TriggerType" + suffix].data<unsigned int>() = event->lvl1_trigger_type();

   // Grab L1 words
   event->lvl1_trigger_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nlvl1_trigger_info(); ++iT1) {
      std::stringstream name;
      name << "L1TriggerInfo_" << iT1;
      attrList->extend(name.str() + suffix, "unsigned int");
      (*attrList)[name.str() + suffix].data<unsigned int>() = *buffer;
      ++buffer;
   } 

   // Grab L2 words
   event->lvl2_trigger_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nlvl2_trigger_info(); ++iT1) {
      if (*buffer != 0) {
         std::stringstream name;
         name << "L2TriggerInfo_" << iT1;
         attrList->extend(name.str() + suffix, "unsigned int");
         (*attrList)[name.str() + suffix].data<unsigned int>() = *buffer;
      }
      ++buffer;
   } 

   // Grab EF words
   event->event_filter_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nevent_filter_info(); ++iT1) {
      if (*buffer != 0) {
         std::stringstream name;
         name << "EFTriggerInfo_" << iT1;
         attrList->extend(name.str() + suffix, "unsigned int");
         (*attrList)[name.str() + suffix].data<unsigned int>() = *buffer;
      }
      ++buffer;
   } 

   // Grab stream tags
   event->stream_tag(buffer);
   std::vector<eformat::helper::StreamTag> onl_streamTags;
   eformat::helper::decode(event->nstream_tag(), buffer, onl_streamTags);
   for (std::vector<eformat::helper::StreamTag>::const_iterator itS = onl_streamTags.begin(),
      itSE = onl_streamTags.end(); itS != itSE; ++itS) {
      attrList->extend(itS->name + suffix, "string");
      (*attrList)[itS->name + suffix].data<std::string>() = itS->type;
   }

   return StatusCode::SUCCESS;
}

//__________________________________________________________________________
int EventSelectorByteStream::findEvent(int evtNum) const {
   // Loop over file event counts
   //ATH_MSG_INFO("try to find evnum = " << evtNum << " in " << m_numEvt.size() << " files");
   for (size_t i = 0; i < m_inputCollectionsProp.value().size(); i++) {
      if (m_inputCollectionsProp.value().size() != m_numEvt.size()) {
         ATH_MSG_ERROR("vector size incompatibility");
         break;
      }
      // if file not opened yet, check it
      if (m_numEvt[i] == -1) {
         std::string fileName = m_inputCollectionsProp.value()[i];
         auto nevguid = m_eventSource->getBlockIterator(fileName);
         long nev = nevguid.first;
         // if failure on file open, exit
         if (nev==-1) {
            break;
         }
         // set initial event counter for that file
         if (i > 0) {
            m_firstEvt[i] = m_firstEvt[i - 1] + m_numEvt[i - 1];
         } else {
            m_firstEvt[i] = 0;
         }
         // log number of events in that file
         m_numEvt[i] = nev;
      }
      // if sought event is in this file, then return the index of that file
      if (evtNum >= m_firstEvt[i] && evtNum < m_firstEvt[i] + m_numEvt[i]) {
         ATH_MSG_INFO("found " << evtNum << " in file " << i);
         return(i);
      }
   }
   ATH_MSG_INFO("did not find ev " << evtNum);
   // return file not found marker
   return(-1);
}

//__________________________________________________________________________
int EventSelectorByteStream::curEvent (const Context& /*it*/) const {
   // event counter in IEvtSelectorSeek interface
   return int(m_NumEvents);
}

//__________________________________________________________________________
int EventSelectorByteStream::size (Context& /*it*/) const {
  return -1;
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::makeServer(int /*num*/) {
   if (m_eventStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   return(m_eventStreamingTool->makeServer(1));
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::makeClient(int /*num*/) {
   if (m_eventStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   return(m_eventStreamingTool->makeClient(0));
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::share(int evtNum) {
   if (m_eventStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   if (m_eventStreamingTool->isClient()) {
      StatusCode sc = m_eventStreamingTool->lockEvent(evtNum);
      while (sc.isRecoverable()) {
         usleep(1000);
         sc = m_eventStreamingTool->lockEvent(evtNum);
      }
      return(sc);
   }
   return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::readEvent(int maxevt) {
   if (m_eventStreamingTool.empty()) {
      ATH_MSG_ERROR("No AthenaSharedMemoryTool configured for readEvent()");
      return(StatusCode::FAILURE);
   }
   ATH_MSG_VERBOSE("Called read Event " << maxevt);
   for (int i = 0; i < maxevt || maxevt == -1; ++i) {
      const RawEvent* pre = 0;
      if (this->next(*m_beginIter).isSuccess()) {
         pre = m_eventSource->currentEvent();
      } else {
         if (m_NumEvents == -1) {
            ATH_MSG_VERBOSE("Called read Event and read last event from input: " << i);
            break;
         }
         ATH_MSG_ERROR("Unable to retrieve next event for " << i << "/" << maxevt);
         return(StatusCode::FAILURE);
      }
      if (m_eventStreamingTool->isServer()) {
         StatusCode sc = m_eventStreamingTool->putEvent(m_NumEvents - 1, pre->start(), pre->fragment_size_word() * sizeof(uint32_t), m_eventSource->currentEventStatus());
         while (sc.isRecoverable()) {
            usleep(1000);
            sc = m_eventStreamingTool->putEvent(m_NumEvents - 1, pre->start(), pre->fragment_size_word() * sizeof(uint32_t), m_eventSource->currentEventStatus());
         }
         if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Cannot put Event " << m_NumEvents - 1 << " to AthenaSharedMemoryTool");
            return(StatusCode::FAILURE);
         }
      }
   }
   // End of file, wait for last event to be taken
   StatusCode sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   while (sc.isRecoverable()) {
      usleep(1000);
      sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   }
   if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Cannot put last Event marker to AthenaSharedMemoryTool");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::createAddress(const IEvtSelector::Context& /*it*/,
                IOpaqueAddress*& iop) const {
   SG::DataProxy* proxy = eventStore()->proxy(ClassID_traits<DataHeader>::ID(),"ByteStreamDataHeader");
   if (proxy !=0) {
     iop = proxy->address();
     return(StatusCode::SUCCESS);
   } else {
     iop = 0;
     return(StatusCode::FAILURE);
   }
}

//________________________________________________________________________________
StatusCode
EventSelectorByteStream::releaseContext(IEvtSelector::Context*& /*it*/) const {
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (riid == IEvtSelector::interfaceID()) {
      *ppvInterface = dynamic_cast<IEvtSelector*>(this);
   } else if (riid == IIoComponent::interfaceID()) {
      *ppvInterface = dynamic_cast<IIoComponent*>(this);
   } else if (riid == IProperty::interfaceID()) {
      *ppvInterface = dynamic_cast<IProperty*>(this);
   } else if (riid == IEvtSelectorSeek::interfaceID()) {
      *ppvInterface = dynamic_cast<IEvtSelectorSeek*>(this);
   } else if (riid == IEventShare::interfaceID()) {
      *ppvInterface = dynamic_cast<IEventShare*>(this);
   } else if (riid == ISecondaryEventSelector::interfaceID()) {
      *ppvInterface = dynamic_cast<ISecondaryEventSelector*>(this);
   } else {
      return(Service::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorByteStream::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
      ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_hasitem(this)) {
      ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
      return(StatusCode::FAILURE);
   }
   if (m_inputCollectionsFromIS) {
      /*   MN: don't copy the FullFileName again - rely on FileSchedulingTool
           to modify the EventSelector Input property directly
      IProperty* propertyServer = dynamic_cast<IProperty*>(m_eventSource);
      std::vector<std::string> vect;
      StringArrayProperty inputFileList("FullFileName", vect);
      StatusCode sc = propertyServer->getProperty(&inputFileList);
      if(sc.isFailure()) {
         ATH_MSG_FATAL("Unable to retrieve ByteStreamInputSvc");
         return(StatusCode::FAILURE);
      }
      m_inputCollectionsProp = inputFileList;
      */
   }
   std::vector<std::string> inputCollections = m_inputCollectionsProp.value();
   for (std::size_t i = 0, imax = inputCollections.size(); i != imax; ++i) {
      ATH_MSG_INFO("I/O reinitialization, file = "  << inputCollections[i]);
      std::string &fname = inputCollections[i];
      if (!iomgr->io_contains(this, fname)) {
         ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
      if (!iomgr->io_retrieve(this, fname).isSuccess()) {
         ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
   }
   // all good... copy over.
   m_beginFileFired = false;
   m_inputCollectionsProp = inputCollections;
   
   return(this->reinit());
}

//__________________________________________________________________________
void EventSelectorByteStream::syncEventCount(int count) const
{
   m_NumEvents = count;
}

//__________________________________________________________________________
bool EventSelectorByteStream::disconnectIfFinished(const SG::SourceID &/* fid */) const
{
   return true;
}
