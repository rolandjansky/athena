/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	EventSelectorByteStream.cxx
//====================================================================
//
// Include files.
#include "EventSelectorByteStream.h"
#include "EventContextByteStream.h"
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvc/ByteStreamExceptions.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGate.h"

// EventInfoAttributeList includes
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "EventInfo/TriggerInfo.h"
#include "PersistentDataModel/DataHeader.h"
#include "eformat/StreamTag.h"

#include <vector>
#include <algorithm>

// Constructor.
EventSelectorByteStream::EventSelectorByteStream(const std::string& name, ISvcLocator* svcloc) : ::AthService(name, svcloc),
        m_fileCount(0),
	m_beginIter(0),
	m_endIter(0),
	m_eventSource(0),
        m_incidentSvc("IncidentSvc", name),
        m_evtStore( "StoreGateSvc", name ),
        m_firstFileFired(false),
        m_beginFileFired(false),
	m_inputCollectionsFromIS(false),
	m_NumEvents(0),
 	m_eventStreamingTool("", this),
        m_helperTools(this),
        m_counterTool("", this) {
   declareProperty("ByteStreamInputSvc",  m_eventSourceName);
   declareProperty("Input",               m_inputCollectionsProp);
   declareProperty("MaxBadEvents",        m_maxBadEvts = -1);
   declareProperty("ProcessBadEvent",     m_procBadEvent = false);
   declareProperty("SkipEvents",          m_SkipEvents = 0);
   declareProperty("SkipEventSequence",   m_skipEventSequenceProp);

   declareProperty("HelperTools",         m_helperTools);
   declareProperty("CounterTool",         m_counterTool);
   declareProperty("SharedMemoryTool",    m_eventStreamingTool);

   // RunNumber, OldRunNumber and OverrideRunNumberFromInput are used
   // to override the run number coming in on the input stream
   declareProperty("RunNumber",           m_runNo = 0);
   m_runNo.verifier().setLower(0);
   // The following properties are only for compatibility with
   // McEventSelector and are not really used anywhere
   declareProperty("EventsPerRun",        m_eventsPerRun = 1000000);
   m_eventsPerRun.verifier().setLower(0);
   declareProperty("FirstEvent",          m_firstEventNo = 0);
   m_firstEventNo.verifier().setLower(0);
   declareProperty("FirstLB",             m_firstLBNo = 0);
   m_firstLBNo.verifier().setLower(0);
   declareProperty("EventsPerLB",         m_eventsPerLB = 1000);
   m_eventsPerLB.verifier().setLower(0);
   declareProperty("InitialTimeStamp",    m_initTimeStamp = 0);
   m_initTimeStamp.verifier().setLower(0);
   declareProperty("TimeStampInterval",   m_timeStampInterval = 0);
   declareProperty("OverrideRunNumber",   m_overrideRunNumber = false);
   declareProperty("OverrideEventNumber", m_overrideEventNumber = false);
   declareProperty("OverrideTimeStamp",   m_overrideTimeStamp = false);
   declareProperty("FileBased", m_filebased = true);

   m_inputCollectionsProp.declareUpdateHandler(&EventSelectorByteStream::inputCollectionsHandler, this);
}
//________________________________________________________________________________
void EventSelectorByteStream::inputCollectionsHandler(Property&) {
   if (this->FSMState() != Gaudi::StateMachine::OFFLINE) {
      this->reinit().ignore();
   }
}
//________________________________________________________________________________
EventSelectorByteStream::~EventSelectorByteStream() {
}
//________________________________________________________________________________
StatusCode EventSelectorByteStream::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Check for input setting
   if (m_filebased && m_inputCollectionsProp.value().size() == 0) {
      ATH_MSG_WARNING("InputCollections not properly set, checking EventStorageInputSvc properties");
      ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc", name());
      bool retrieve(false);
      if (!joSvc.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get JobOptionsSvc.");
      } else {
         // Check if FullFileName is set in the InputSvc
         typedef std::vector<const Property*> Properties_t;
         const Properties_t* esProps = joSvc->getProperties("ByteStreamInputSvc");
         std::vector<const Property*>::const_iterator ii = esProps->begin();
         if (esProps != 0) {
            while (ii != esProps->end()) {
               if ((*ii)->name() == "FullFileName") {
                  StringArrayProperty temp;
                  if ((*ii)->load(temp)) {
                     retrieve = true;
                     m_inputCollectionsProp.assign(temp);
                     m_inputCollectionsFromIS = true;
                     ATH_MSG_INFO("Retrieved InputCollections from InputSvc");
                  }
               }
               if ((*ii)->name() == "EventStore") {
                  StringProperty temp2;
                  if ((*ii)->load(temp2)) {
                     m_evtStore = ServiceHandle<StoreGateSvc>(temp2.value(),this->name());
                     ATH_MSG_INFO("Retrieved StoreGateSvc name of " << temp2);
                  }
               }
               ++ii;
            }
         } else {
            ATH_MSG_WARNING("Did not find InputSvc jobOptions properties");
         }
      }
      if (!retrieve) {
         ATH_MSG_FATAL("Unable to retrieve valid input list");
         return(StatusCode::FAILURE);
      }
   }
   m_skipEventSequence = m_skipEventSequenceProp.value();
   std::sort(m_skipEventSequence.begin(), m_skipEventSequence.end());

   // Check ByteStreamCnvSvc
   IService* svc;
   if (!serviceLocator()->getService(m_eventSourceName, svc).isSuccess()) {
      ATH_MSG_FATAL("Cannot get ByteStreamInputSvc");
      return(StatusCode::FAILURE);
   }
   m_eventSource = dynamic_cast<ByteStreamInputSvc*>(svc);
   if (m_eventSource == 0) {
      ATH_MSG_FATAL("Cannot cast ByteStreamInputSvc");
      return(StatusCode::FAILURE);
   }
   m_eventSource->addRef();
   if (!m_evtStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get StoreGateSvc");
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

   // For backward compatibility, check InputSvc properties for bad events
   ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc", name());
   if (!joSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get JobOptionsSvc.");
      return(StatusCode::FAILURE);
   }
   typedef std::vector<const Property*> Properties_t;
   const Properties_t* esProps = joSvc->getProperties("ByteStreamInputSvc");
   if (esProps != 0) {
      std::vector<const Property*>::const_iterator ii = esProps->begin();
      while (ii != esProps->end()) {
         IntegerProperty temp;
         if ((*ii)->name() == "MaxBadEvents") {     // find it
            if ((*ii)->load(temp)) {                // load it
               if (temp.value() != -1) {            // check if it is set
                  m_maxBadEvts = temp.value();
                  ATH_MSG_INFO("Retrieved MaxBadEvents=" << m_maxBadEvts << " from ByteStreamInputSvc");
               }
            }
         }
         BooleanProperty temp2;
         if ((*ii)->name() == "ProcessBadEvents") {     // find it
            if ((*ii)->load(temp)) {                // load it
               if (temp.value()) {            // check if it is set
                  m_procBadEvent = temp.value();
                  ATH_MSG_INFO("Retrieved ProcessBadEvents=" << m_procBadEvent << " from ByteStreamInputSvc");
               }
            }
         }
         ++ii;
      }
   } else {
      ATH_MSG_WARNING("Did not find ByteStreamInputSvc jobOptions properties");
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
      // Fire LastInputFile incident
      FileIncident lastInputFileIncident(name(), "LastInputFile", "stop");
      m_incidentSvc->fireIncident(lastInputFileIncident);
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
   long nev = m_eventSource->getBlockIterator(blockname);
   if (nev == -1) {
      ATH_MSG_FATAL("Unable to access file " << *m_inputCollectionsIterator << ", stopping here");
      throw ByteStreamExceptions::fileAccessError();
   }
   // Fire the incident
   if (!m_beginFileFired) {
     FileIncident beginInputFileIncident(name(), "BeginInputFile", "BSF:" + *m_inputCollectionsIterator);
     m_incidentSvc->fireIncident(beginInputFileIncident);
     //m_beginFileFired = true;   // Should go here, but can't because IEvtSelector next is const
   }

   // check if file is empty
   if (nev == 0) {
      ATH_MSG_WARNING("no events in file " << blockname << " try next");
      if (m_eventSource->ready()) m_eventSource->closeBlockIterator(true);
      this->nextFile();
      return openNewRun();
   // check if skipping all events in that file
   } else if (m_SkipEvents > nev) {
      ATH_MSG_WARNING("skipping more events " << m_SkipEvents << " than in file " << *m_inputCollectionsIterator << ", try next");
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
      const RawEvent* pre = 0; 
      bool badEvent(false);
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
      if (pre == 0) {
         //ATH_MSG_ERROR("No event built");
	 it = *m_endIter;
	 return(StatusCode::FAILURE);
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
      if ( m_NumEvents > m_SkipEvents && 
           (m_skipEventSequence.empty() || m_NumEvents != m_skipEventSequence.front()) ) {
         StatusCode status(StatusCode::SUCCESS);
         // Build event info attribute list
         if (buildEventAttributeList().isFailure()) ATH_MSG_WARNING("Unable to build event info att list");
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
	    m_incidentSvc->fireIncident(Incident(name(), "SkipEvent"));
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
	 m_incidentSvc->fireIncident(Incident(name(), "SkipEvent"));
      }
   } // for loop
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::next(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      if ( m_NumEvents+jump != m_SkipEvents) {
         // Save initial event count
         unsigned int cntr = m_NumEvents;
         // In case NumEvents increments multiple times in a single next call
         while (m_NumEvents+1 <= cntr + jump) {
            if (!next(ctxt).isSuccess()) {
               return(StatusCode::FAILURE);
            }
         }
      }
      else ATH_MSG_DEBUG("Jump covered by skip event " << m_SkipEvents);
      return(StatusCode::SUCCESS);
   }
   else { 
      ATH_MSG_WARNING("Called jump next with non-multiple jump");
   }
   return(StatusCode::SUCCESS);
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
      long nev = m_eventSource->getBlockIterator(fileName);
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

StatusCode EventSelectorByteStream::buildEventAttributeList() const
{
   //ServiceHandle<StoreGateSvc> m_evtStore("StoreGateSvc/StoreGateSvc",this->name());
   //StatusCode sc = m_evtStore.retrieve();
   //if (sc.isFailure()) return StatusCode::FAILURE;
   std::string listName("EventInfoAtts");
   //bool found  = m_evtStore->contains<AthenaAttributeList>(listName);
   //if (found) {
   if (m_evtStore->contains<AthenaAttributeList>(listName)) {
      const DataHandle<AthenaAttributeList> oldAttrList;
      if (!m_evtStore->retrieve(oldAttrList, listName).isSuccess()) {
         ATH_MSG_ERROR("Cannot retrieve old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
      if (!m_evtStore->removeDataAndProxy(oldAttrList.cptr()).isSuccess()) {
         ATH_MSG_ERROR("Cannot remove old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
      //ATH_MSG_ERROR("Event store not cleared properly after previous event");
      //return StatusCode::FAILURE;
   }
   // build spec
   // fill att list spec
   coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
   AthenaAttributeList* attlist = new AthenaAttributeList(*spec);
   attlist->extend("RunNumber"  ,"long");
   attlist->extend("EventNumber","long");
   attlist->extend("Lumiblock"  ,"int");
   attlist->extend("BunchID"    ,"int");
   attlist->extend("CrossingTimeSec", "unsigned int");
   attlist->extend("CrossingTimeNSec", "unsigned int");

   // fill attribute list
   const RawEvent* event = m_eventSource->currentEvent();

   (*attlist)["RunNumber"].data<long>() = event->run_no();
   if (event->version() < 0x03010000) {
     (*attlist)["EventNumber"].data<long>() = event->lvl1_id();
   } else {
     (*attlist)["EventNumber"].data<long>() = event->global_id();
   }
   (*attlist)["Lumiblock"].data<int>() = event->lumi_block();
   (*attlist)["BunchID"].data<int>() = event->bc_id();

   unsigned int bc_time_sec = event->bc_time_seconds();
   unsigned int bc_time_ns  = event->bc_time_nanoseconds();
   // bc_time_ns should be lt 1e9.
   if (bc_time_ns > 1000000000) {
     // round it off to 1e9
     ATH_MSG_WARNING(" bc_time nanosecond number larger than 1e9, it is " << bc_time_ns << ", reset it to 1 sec");
     bc_time_ns = 1000000000;
   }
   (*attlist)["CrossingTimeSec"].data<unsigned int>() = bc_time_sec;
   (*attlist)["CrossingTimeNSec"].data<unsigned int>() = bc_time_ns;

   const OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer;

   event->status(buffer);
   attlist->extend("TriggerStatus","unsigned int");
   (*attlist)["TriggerStatus"].data<unsigned int>() = *buffer;

   attlist->extend("ExtendedL1ID","unsigned int");
   attlist->extend("L1TriggerType","unsigned int");
   (*attlist)["ExtendedL1ID"].data<unsigned int>() = event->lvl1_id();
   (*attlist)["L1TriggerType"].data<unsigned int>() = event->lvl1_trigger_type();

   // Grab L1 words
   event->lvl1_trigger_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nlvl1_trigger_info(); ++iT1) {
     std::stringstream name;
     name << "L1TriggerInfo_" << iT1;
     //ATH_MSG_DEBUG("Adding L1 info " << name.str());
     attlist->extend(name.str(),"unsigned int");
     (*attlist)[name.str()].data<unsigned int>() = *buffer;
     ++buffer;
   } 

   // Grab L2 words
   event->lvl2_trigger_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nlvl2_trigger_info(); ++iT1) {
     if (*buffer != 0) {
       std::stringstream name;
       name << "L2TriggerInfo_" << iT1;
       attlist->extend(name.str(),"unsigned int");
       (*attlist)[name.str()].data<unsigned int>() = *buffer;
     }
     ++buffer;
   } 

   // Grab EF words
   event->event_filter_info(buffer);
   for (uint32_t iT1 = 0; iT1 < event->nevent_filter_info(); ++iT1) {
     if (*buffer != 0) {
       std::stringstream name;
       name << "EFTriggerInfo_" << iT1;
       attlist->extend(name.str(),"unsigned int");
       (*attlist)[name.str()].data<unsigned int>() = *buffer;
     }
     ++buffer;
   } 

   // Grab stream tags
   event->stream_tag(buffer);
   std::vector<eformat::helper::StreamTag> onl_streamTags;
   eformat::helper::decode(event->nstream_tag(), buffer, onl_streamTags);
   for (std::vector<eformat::helper::StreamTag>::const_iterator itS = onl_streamTags.begin(),
       itSE = onl_streamTags.end(); itS != itSE; ++itS) {
     attlist->extend(itS->name,"string");
     (*attlist)[itS->name].data<std::string>() = itS->type;
   }

   // put result in event store
   if (m_evtStore->record(attlist,"EventInfoAtts").isFailure()) return StatusCode::FAILURE;
   
   return(StatusCode::SUCCESS);
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
         int nev = m_eventSource->getBlockIterator(fileName);
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
      return(StatusCode::FAILURE);
   }
   ATH_MSG_VERBOSE("Called read Event " << maxevt);
   for (int i = 0; i < maxevt || maxevt == -1; ++i) {
      //const RawEvent* pre = m_eventSource->nextEvent();
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
      if (pre == 0) {
         // End of file, wait for last event to be taken
         StatusCode sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
         while (sc.isRecoverable()) {
            usleep(1000);
            sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
         }
         if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Cannot put last Event marker to AthenaSharedMemoryTool");
         }
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
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
StatusCode EventSelectorByteStream::createAddress(const IEvtSelector::Context& /*it*/,
                IOpaqueAddress*& iop) const {
   SG::DataProxy* proxy = m_evtStore->proxy(ClassID_traits<DataHeader>::ID(),"ByteStreamDataHeader");
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
