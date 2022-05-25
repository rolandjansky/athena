/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file OutputStreamSequencerSvc.cxx
 *  @brief This file contains the implementation for the OutputStreamSequencerSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "OutputStreamSequencerSvc.h"
#include "MetaDataSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ConcurrencyFlags.h"

#include <sstream>

//________________________________________________________________________________
OutputStreamSequencerSvc::OutputStreamSequencerSvc(const std::string& name, ISvcLocator* pSvcLocator) : ::AthService(name, pSvcLocator),
	m_metaDataSvc("MetaDataSvc", name),
	m_fileSequenceNumber(-1)
{
}

//__________________________________________________________________________
OutputStreamSequencerSvc::~OutputStreamSequencerSvc() {
}
//__________________________________________________________________________
StatusCode OutputStreamSequencerSvc::initialize() {
   ATH_MSG_DEBUG("Initializing " << name());

   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incsvc("IncidentSvc", this->name());
   if (!incsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   if( !incidentName().empty() ) {
      incsvc->addListener(this, incidentName(), 100);
      incsvc->addListener(this, IncidentType::BeginProcessing, 100);
      ATH_MSG_DEBUG("Listening to " << incidentName() << " incidents" );
      ATH_MSG_DEBUG("Reporting is " << (m_reportingOn.value()? "ON" : "OFF") );
      // Retrieve MetaDataSvc
      if( !m_metaDataSvc.isValid() and !m_metaDataSvc.retrieve().isSuccess() ) {
         ATH_MSG_ERROR("Cannot get MetaDataSvc");
         return StatusCode::FAILURE;
      }
   }

   if( inConcurrentEventsMode() ) {
      ATH_MSG_DEBUG("Concurrent events mode");
   } else {
      ATH_MSG_VERBOSE("Sequential events mode");
   }

   // Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() not set yet
   // m_rangeIDinSlot.resize( );
   m_finishedRange = m_fnToRangeId.end();

   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode OutputStreamSequencerSvc::finalize() {
   // Release MetaDataSvc 
   if (!m_metaDataSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release MetaDataSvc.");
   }
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode OutputStreamSequencerSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (riid == this->interfaceID()) {
      *ppvInterface = this;
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
bool    OutputStreamSequencerSvc::inConcurrentEventsMode() {
   return Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() > 1;
}

//__________________________________________________________________________
bool    OutputStreamSequencerSvc::inUse() const {
   return m_fileSequenceNumber >= 0;
}

//__________________________________________________________________________
void OutputStreamSequencerSvc::handle(const Incident& inc)
{
   ATH_MSG_INFO("handle incident type " << inc.type());

   auto slot = Gaudi::Hive::currentContext().slot();
   if( slot == EventContext::INVALID_CONTEXT_ID )  slot = 0;
   m_lastIncident = inc.type();

   if( inc.type() == incidentName() ) {  // NextEventRange 
      std::string rangeID;
      const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
      if (fileInc != nullptr) {
         rangeID = fileInc->fileName();
         ATH_MSG_DEBUG("Requested (through incident) Next Event Range filename extension: " << rangeID);
      }

      if( rangeID == "dummy" ) {
         if( not inConcurrentEventsMode() ) {
            // finish the previous Range here only in SEQUENTIAL (threads<2) event processing
            // Write metadata on the incident finishing a Range (filename=="dummy") in ES MP
            ATH_MSG_DEBUG("MetaData transition");
            // immediate write and disconnect for ES, otherwise do it after Event write is done
            bool disconnect { true };
            if( !m_metaDataSvc->transitionMetaDataFile( m_lastFileName, disconnect ).isSuccess() ) {
               throw GaudiException("Cannot transition MetaData", name(), StatusCode::FAILURE);
            }
         }
         // exit now, wait for the next (real) incident that will start the next range
         return;
      }
      // start a new range
      std::lock_guard lockg( m_mutex );
      m_fileSequenceNumber++;
      if( rangeID.empty() ) {
         std::ostringstream n;
         n << "_" << std::setw(4) << std::setfill('0') << m_fileSequenceNumber;
         rangeID = n.str();
         ATH_MSG_DEBUG("Default next event range filename extension: " << rangeID);
      } 
      if( slot >= m_rangeIDinSlot.size() ) {
         // MN - late resize, is there a better place for it?
         m_rangeIDinSlot.resize( std::max(slot+1, Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents()) );
      }
      m_rangeIDinSlot[ slot ] = rangeID;
      // remember range ID for next events in the same range
      m_currentRangeID = rangeID;

      if( not inConcurrentEventsMode() ) {
         // non-file incident case (filename=="") in regular SP LoopMgr
         ATH_MSG_DEBUG("MetaData transition");
         bool disconnect { false };
         // MN: may not know the full filename yet, but that is only needed for disconnect==true
         if( !m_metaDataSvc->transitionMetaDataFile( "" /*m_lastFileName*/, disconnect ).isSuccess() ) {
            throw GaudiException("Cannot transition MetaData", name(), StatusCode::FAILURE);
         }
      }
   }
   else if( inc.type() == IncidentType::BeginProcessing ) {
      // new event start - assing current rangeId to its slot
      ATH_MSG_DEBUG("Assigning rangeID = " << m_currentRangeID << " to slot " << slot);
      std::lock_guard lockg( m_mutex );
      // If this service is enabled but not getting NextRange incidents, need to resize here
      if( slot >= m_rangeIDinSlot.size() ) {
         m_rangeIDinSlot.resize( std::max(slot+1, Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents()) );
      }
      m_rangeIDinSlot[ slot ] = m_currentRangeID;
   }

}

//__________________________________________________________________________
std::string OutputStreamSequencerSvc::buildSequenceFileName(const std::string& orgFileName)
{
   if( !inUse() ) {
      // Event sequences not in use, just return the original filename
      return orgFileName;
   }
   std::string rangeID = currentRangeID();
   std::lock_guard lockg( m_mutex );
   // build the full output file name for this event range
   std::string fileNameCore = orgFileName, fileNameExt;
   std::size_t sepPos = orgFileName.find('[');
   if (sepPos != std::string::npos) {
      fileNameCore = orgFileName.substr(0, sepPos);
      fileNameExt = orgFileName.substr(sepPos);
   }
   std::ostringstream n;
   n << fileNameCore << "." << rangeID << fileNameExt;
   m_lastFileName = n.str();

   if( m_reportingOn.value() ) {
      m_fnToRangeId.insert( std::pair(m_lastFileName, rangeID) );
   }

   return m_lastFileName;
}


std::string OutputStreamSequencerSvc::currentRangeID() const
{
   if( !inUse() )  return "";
   auto slot = Gaudi::Hive::currentContext().slot();
   if( slot == EventContext::INVALID_CONTEXT_ID )  slot = 0; 
   std::lock_guard lockg( m_mutex );
   if( slot >= m_rangeIDinSlot.size() ) return "";
   return m_rangeIDinSlot[ slot ];
}


void OutputStreamSequencerSvc::publishRangeReport(const std::string& outputFile)
{
   std::lock_guard lockg( m_mutex );
   m_finishedRange = m_fnToRangeId.find(outputFile);
}

OutputStreamSequencerSvc::RangeReport_ptr OutputStreamSequencerSvc::getRangeReport()
{
  RangeReport_ptr report;
  if( !m_reportingOn.value() ) {
     ATH_MSG_WARNING("Reporting not turned on - set " << m_reportingOn.name() << " to True");
  } else {
     std::lock_guard lockg( m_mutex );
     if(m_finishedRange!=m_fnToRangeId.end()) {
        report = std::make_unique<RangeReport_t>(m_finishedRange->second,m_finishedRange->first);
        m_fnToRangeId.erase(m_finishedRange);
        m_finishedRange=m_fnToRangeId.end();
     }
  }
  return report;
}
