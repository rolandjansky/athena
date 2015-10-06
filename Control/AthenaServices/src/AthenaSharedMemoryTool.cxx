/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaSharedMemoryTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaSharedMemoryTool.h"

#include "GaudiKernel/FileIncident.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

const std::size_t maxTokenLength = 512;

struct ShareEventHeader {
   enum ProcessStatus { CLEARED, FILLED, LOCKED, UNLOCKED, UNKNOWN };
   ProcessStatus evtProcessStatus;
   long evtSeqNumber;
   long fileSeqNumber;
   std::size_t evtSize;
   std::size_t evtOffset;
   unsigned int evtCoreStatusFlag;
   uint32_t evtTerm1;
   uint32_t evtTerm2;
   char token[maxTokenLength];
};

//___________________________________________________________________________
AthenaSharedMemoryTool::AthenaSharedMemoryTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent),
		m_maxEventSize(16 * 1024 * 1024),
		m_maxMetadataSize(4 * 1024),
		m_maxDataClients(16),
		m_payload(0),
		m_metadata(0),
		m_status(0),
		m_fileSeqNumber(0),
		m_isServer(false),
		m_isClient(false),
		m_incidentSvc("IncidentSvc", name) {
   declareProperty("SharedMemoryName", m_sharedMemory = name);
   declareInterface<IAthenaIPCTool>(this);
}

//___________________________________________________________________________
AthenaSharedMemoryTool::~AthenaSharedMemoryTool() {
   delete m_payload; m_payload = 0;
   delete m_metadata; m_metadata = 0;
   delete m_status; m_status = 0;
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthAlgTool::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthAlgTool base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc");
      return(StatusCode::FAILURE);
   }
   boost::interprocess::shared_memory_object::remove(m_sharedMemory.value().c_str());
   ATH_MSG_DEBUG("creating shared memory object with name \"" << m_sharedMemory.value() << "\"");
   boost::interprocess::shared_memory_object shm(boost::interprocess::create_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   shm.truncate(m_maxEventSize + m_maxMetadataSize + m_maxDataClients * sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   if (m_isServer) {
      boost::interprocess::shared_memory_object::remove(m_sharedMemory.value().c_str());
   }
   // Release IncidentSvc
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release IncidentSvc.");
   }
   return(::AthAlgTool::finalize());
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::makeServer(int num) {
   ATH_MSG_DEBUG("Creating shared memory object for writer.");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Server.");
      return(StatusCode::FAILURE);
   }
   if (num > m_maxDataClients) {
      ATH_MSG_ERROR("Too many clients for AthenaSharedMemoryTool Server.");
      return(StatusCode::FAILURE);
   }
   m_isServer = true;
   boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, 0, m_maxEventSize);
   m_metadata = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize, m_maxMetadataSize);
   m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize + m_maxMetadataSize, num * sizeof(ShareEventHeader));
   ShareEventHeader evtH = { ShareEventHeader::UNLOCKED, -1, -1, 0, 0, 0, 0, 0, "" };
   for (int i = 0; i < num; i++) {
      std::memcpy(static_cast<char*>(m_status->get_address()) + i * sizeof(ShareEventHeader), &evtH, sizeof(ShareEventHeader));
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaSharedMemoryTool::isServer() const {
   return(m_isServer);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::makeClient(int num) {
   ATH_MSG_DEBUG("Creating shared memory object for reader.");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Client.");
      return(StatusCode::FAILURE);
   }
   if (num >= m_maxDataClients) {
      ATH_MSG_ERROR("Too many clients for AthenaSharedMemoryTool.");
      return(StatusCode::FAILURE);
   }
   m_isClient = true;
   boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_only, 0, m_maxEventSize);
   m_metadata = new boost::interprocess::mapped_region(shm, boost::interprocess::read_only, m_maxEventSize, m_maxMetadataSize);
   m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize + m_maxMetadataSize + num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaSharedMemoryTool::isClient() const {
   return(m_isClient);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putEvent(long eventNumber, const void* source, std::size_t nbytes, unsigned int status) {
   if (nbytes > m_maxEventSize) {
      ATH_MSG_ERROR("Event size = " << nbytes << " greater than maximum for eventNumber = " << eventNumber);
      return(StatusCode::FAILURE);
   }
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
      ATH_MSG_DEBUG("Waiting for putEvent, eventNumber = " << eventNumber);
      return(StatusCode::RECOVERABLE);
   }
   if (source == 0 && nbytes == 0) {
      ATH_MSG_DEBUG("putEvent got last Event marker");
      return(StatusCode::SUCCESS);
   }
   evtH->evtProcessStatus = ShareEventHeader::CLEARED;
   if (source != 0 && source != m_payload->get_address()) {
      std::memcpy(m_payload->get_address(), source, nbytes);
   }
   evtH->evtSeqNumber = eventNumber;
   evtH->fileSeqNumber = m_fileSeqNumber;
   evtH->evtSize = nbytes;
   evtH->evtOffset = 0;
   evtH->evtCoreStatusFlag = status;
   if (source) {
     evtH->evtTerm1 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 1);
     evtH->evtTerm2 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 2);
   }
   else {
     evtH -> evtTerm1 = 0;
     evtH -> evtTerm2 = 0;
   }
   evtH->evtProcessStatus = ShareEventHeader::FILLED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getLockedEvent(void** target, unsigned int& status) {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
      const std::size_t nbytes = evtH->evtSize;
      if (target != 0) {
         char* buf = new char[nbytes];
         std::memcpy(buf, static_cast<char*>(m_payload->get_address()) + evtH->evtOffset, nbytes);
         *target = buf;
         if (evtH->evtTerm1 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 1) ||
	         evtH->evtTerm2 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 2)) {
            ATH_MSG_ERROR("Event corrupted by AthenaSharedMemoryTool");
            return(StatusCode::FAILURE);
         }
      }
      status = evtH->evtCoreStatusFlag;
   } else {
      ATH_MSG_ERROR("No event locked");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::lockEvent(long eventNumber) {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtSeqNumber > eventNumber) {
      ATH_MSG_ERROR("eventNumber = " << eventNumber << ", already processed");
      return(StatusCode::FAILURE);
   }
   if (evtH->evtSeqNumber != eventNumber || evtH->evtProcessStatus != ShareEventHeader::FILLED) {
      ATH_MSG_DEBUG("Waiting for lockEvent, eventNumber = " << eventNumber);
      return(StatusCode::RECOVERABLE);
   }
   evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   if (evtH->fileSeqNumber != m_fileSeqNumber && m_fileSeqNumber > 0) {
      FileIncident endFileIncident(name(), "EndInputFile", "SHM");
      m_incidentSvc->fireIncident(endFileIncident);
      m_fileSeqNumber = evtH->fileSeqNumber;
      FileIncident beginFileIncident(name(), "BeginInputFile", "SHM");
      m_incidentSvc->fireIncident(beginFileIncident);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::unlockEvent() {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putObject(const void* source, size_t nbytes, int num) {
   for (std::size_t i = 0; i < m_status->get_size() ; i = i + sizeof(ShareEventHeader)) {
      void* status = static_cast<char*>(m_status->get_address()) + i;
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
      if (evtH->evtProcessStatus == ShareEventHeader::FILLED) {
         ATH_MSG_DEBUG("Waiting for UNFILL putObject, client = " << i/sizeof(ShareEventHeader));
         return(StatusCode::RECOVERABLE);
      }
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   if (evtH->evtProcessStatus != ShareEventHeader::CLEARED) {
      ATH_MSG_DEBUG("Waiting for CLEAR putObject, client = " << num);
      return(StatusCode::RECOVERABLE);
   }
   if (source != 0 && source != m_payload->get_address()) {
      std::memcpy(m_payload->get_address(), source, nbytes);
      evtH->evtSize = nbytes;
      evtH->evtTerm1 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 1);
      evtH->evtTerm2 = *(static_cast<const uint32_t*>(source) + nbytes / sizeof(uint32_t) - 2);
   } else {
      evtH->evtSize = 0;
   }
   evtH->evtProcessStatus = ShareEventHeader::FILLED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getObject(const char* tokenString, void** target, size_t& nbytes) {
   if (strlen(tokenString) >= maxTokenLength) {
      ATH_MSG_ERROR("Token = " << tokenString << ", too long for AthenaSharedMemoryTool");
      return(StatusCode::FAILURE);
   }
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
   strncpy(evtH->token, tokenString, maxTokenLength);
   evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   while (evtH->evtProcessStatus != ShareEventHeader::FILLED) {
      usleep(10000);
   } // FIXME: PvG, don't wait here?
   nbytes = evtH->evtSize;
   if (target != 0 && nbytes > 0) {
      char* buf = new char[nbytes];
      std::memcpy(buf, static_cast<char*>(m_payload->get_address()), nbytes);
      *target = buf;
      if (evtH->evtTerm1 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 1) ||
             evtH->evtTerm2 != *(static_cast<const uint32_t*>(*target) + nbytes / sizeof(uint32_t) - 2)) {
         ATH_MSG_ERROR("Object corrupted by AthenaSharedMemoryTool");
         return(StatusCode::FAILURE);
      }
   } else if (target != 0) {
      *target = m_payload->get_address();
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::lockObject(char** tokenString, int& num) {
   num = 0; // FIXME: PvG, do round robin
   for (std::size_t i = 0; i < m_status->get_size() ; i = i + sizeof(ShareEventHeader)) {
      void* status = static_cast<char*>(m_status->get_address()) + i;
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
      if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
         *tokenString = new char[maxTokenLength];
         strncpy(*tokenString, evtH->token, maxTokenLength);
         evtH->evtProcessStatus = ShareEventHeader::CLEARED;
         return(StatusCode::SUCCESS);
      }
      num++;
   }
   return(StatusCode::RECOVERABLE);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::unlockObject() {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putMetadata(const char* metadataString) {
   strncpy(static_cast<char*>(m_metadata->get_address()), metadataString, m_maxMetadataSize);
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getMetadata(char** metadataString) {
   strncpy(*metadataString, static_cast<char*>(m_metadata->get_address()), m_maxMetadataSize);
   return(StatusCode::SUCCESS);
}
