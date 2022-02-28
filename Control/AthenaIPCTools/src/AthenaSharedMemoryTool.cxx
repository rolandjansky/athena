/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaSharedMemoryTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaSharedMemoryTool.h"

#include "GaudiKernel/FileIncident.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <sstream>

const std::size_t maxTokenLength = 512;

namespace {
struct ShareEventHeader {
   enum ProcessStatus { CLEARED, FILLED, LOCKED, UNLOCKED, PARTIAL, SHARED, UNKNOWN };
   ProcessStatus evtProcessStatus;
   long evtSeqNumber;
   long fileSeqNumber;
   std::size_t evtSize;
   std::size_t evtOffset;
   std::size_t evtCursor;
   unsigned int evtCoreStatusFlag;
   char token[maxTokenLength];
};
} // anonymous namespace

//___________________________________________________________________________
AthenaSharedMemoryTool::AthenaSharedMemoryTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent),
		m_maxSize(64 * 1024 * 1024),
		m_maxDataClients(256),
		m_num(-1),
		m_dataClients(),
		m_payload(nullptr),
		m_status(nullptr),
		m_fileSeqNumber(0),
		m_isServer(false),
		m_isClient(false),
		m_incidentSvc("IncidentSvc", name) {
   declareProperty("SharedMemoryName", m_sharedMemory = name);
   declareProperty("UseMultipleSegments", m_multipleSegments = false);
   declareInterface<IAthenaIPCTool>(this);
}

//___________________________________________________________________________
AthenaSharedMemoryTool::~AthenaSharedMemoryTool() {
   delete m_payload; m_payload = nullptr;
   delete m_status; m_status = nullptr;
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::initialize() {
   ATH_MSG_INFO("Initializing " << name());
   if (!::AthAlgTool::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthAlgTool base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::stop() {
   ATH_MSG_INFO("in stop()");
   if (m_isClient && m_num > 0) {
      ATH_MSG_INFO("Client stop() inform Server: " << m_num);
      m_num = -1;
      while (lockObject("stop").isRecoverable()) {
         usleep(100);
      }
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      while (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
         usleep(100);
      }
   }
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
StatusCode AthenaSharedMemoryTool::makeServer(int num, const std::string& streamPortSuffix) {
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Server.");
      return(StatusCode::FAILURE);
   }
   if (num > m_maxDataClients) {
      ATH_MSG_ERROR("Too many clients for AthenaSharedMemoryTool Server.");
      return(StatusCode::FAILURE);
   }
   m_num = num;
   m_isServer = true;
   ATH_MSG_DEBUG("Creating shared memory object with name \"" << m_sharedMemory.value() << "\"");
   boost::interprocess::shared_memory_object::remove(m_sharedMemory.value().c_str());
   boost::interprocess::shared_memory_object shm(boost::interprocess::create_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   int numSegments = m_multipleSegments.value() ? num : 1;
   shm.truncate(numSegments * m_maxSize);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, 0, numSegments * m_maxSize);
   const std::string statusName = m_sharedMemory.value() + "_status";
   boost::interprocess::shared_memory_object::remove(statusName.c_str());
   boost::interprocess::shared_memory_object shm_status(boost::interprocess::create_only,
	   statusName.c_str(),
	   boost::interprocess::read_write);
   shm_status.truncate(num * sizeof(ShareEventHeader));
   m_status = new boost::interprocess::mapped_region(shm_status, boost::interprocess::read_write, 0, num * sizeof(ShareEventHeader));
   ShareEventHeader evtH = { ShareEventHeader::UNLOCKED, -1, -1, 0, 0, 0, 0, "" };
   std::memcpy(evtH.token, streamPortSuffix.c_str(), maxTokenLength - 1);
   evtH.token[maxTokenLength - 1] = 0;
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
StatusCode AthenaSharedMemoryTool::makeClient(int num, std::string& streamPortSuffix) {
   if (m_isServer) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Client.");
      return(StatusCode::FAILURE);
   }
   if (num >= m_maxDataClients) {
      ATH_MSG_ERROR("Too many clients for AthenaSharedMemoryTool.");
      return(StatusCode::FAILURE);
   }
   if (m_num > 0 && num <= 0) {// stop running client
      ATH_MSG_DEBUG("Stop AthenaSharedMemoryTool Client.");
      m_num = -1;
      while (lockObject("stop").isRecoverable()) {
         usleep(100);
      }
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      while (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
         usleep(100);
      }
      delete m_payload ; m_payload = nullptr;
      delete m_status ; m_status = nullptr;
      m_isClient = false;
      return(StatusCode::SUCCESS);
   }
   while (!m_isClient) {
      try { // Check whether Server created shared memory object
         const std::string statusName = m_sharedMemory.value() + "_status";
         boost::interprocess::shared_memory_object shm_status(boost::interprocess::open_only,
	         statusName.c_str(),
	         boost::interprocess::read_write);
         m_status = new boost::interprocess::mapped_region(shm_status, boost::interprocess::read_write, num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
         boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	         m_sharedMemory.value().c_str(),
	         boost::interprocess::read_write);
         int numSegments = m_multipleSegments.value() ? num : 0;
         m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, numSegments * m_maxSize, m_maxSize);
         m_isClient = true;
      } catch (boost::interprocess::interprocess_exception& e) {
         usleep(100000);
      }
   }
   if (m_num <= 0 && num > 0) {
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      streamPortSuffix.assign(evtH->token);
      while (lockObject("start").isRecoverable()) {
         usleep(100);
      }
      while (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
         usleep(100);
      }
      m_num = num;
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaSharedMemoryTool::isClient() const {
   return(m_isClient);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putEvent(long eventNumber, const void* source, std::size_t nbytes, unsigned int status) const {
   if (nbytes > m_maxSize) {
      ATH_MSG_ERROR("Event size = " << nbytes << " greater than maximum for eventNumber = " << eventNumber);
      return(StatusCode::FAILURE);
   }
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
      ATH_MSG_DEBUG("Waiting for putEvent, eventNumber = " << eventNumber);
      return(StatusCode::RECOVERABLE);
   }
   if (source == nullptr && nbytes == 0) {
      ATH_MSG_DEBUG("putEvent got last Event marker");
      evtH->evtSeqNumber = 0;
      evtH->fileSeqNumber = 0;
      evtH->evtSize = 0;
      evtH->evtOffset = 0;
      m_status->flush(0, sizeof(ShareEventHeader));
      return(StatusCode::SUCCESS);
   }
   evtH->evtProcessStatus = ShareEventHeader::CLEARED;
   if (source != nullptr && source != m_payload->get_address()) {
      std::memcpy(m_payload->get_address(), source, nbytes);
      m_payload->flush(0, nbytes);
   }
   evtH->evtSeqNumber = eventNumber;
   evtH->fileSeqNumber = m_fileSeqNumber;
   evtH->evtSize = nbytes;
   evtH->evtOffset = 0;
   evtH->evtCoreStatusFlag = status;
   m_status->flush(0, sizeof(ShareEventHeader));
   evtH->evtProcessStatus = ShareEventHeader::FILLED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getLockedEvent(void** target, unsigned int& status) const {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
      const std::size_t nbytes = evtH->evtSize;
      if (target != nullptr) {
         char* buf = new char[nbytes];
         std::memcpy(buf, static_cast<char*>(m_payload->get_address()) + evtH->evtOffset, nbytes);
         *target = buf;
      }
      status = evtH->evtCoreStatusFlag;
   } else {
      ATH_MSG_ERROR("No event locked");
      return(StatusCode::FAILURE);
   }
   evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::lockEvent(long eventNumber) const {
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtSeqNumber > eventNumber) {
      ATH_MSG_ERROR("eventNumber = " << eventNumber << ", already processed");
      return(StatusCode::FAILURE);
   }
   if (evtH->evtSeqNumber == 0 && evtH->fileSeqNumber == 0 && evtH->evtSize == 0) {
      ATH_MSG_DEBUG("Server has been terminated");
      return(StatusCode::FAILURE);
   }
   if (evtH->evtSeqNumber != eventNumber || evtH->evtProcessStatus != ShareEventHeader::FILLED) {
      ATH_MSG_DEBUG("Waiting for lockEvent, eventNumber = " << eventNumber);
      return(StatusCode::RECOVERABLE);
   }
   if (evtH->fileSeqNumber != m_fileSeqNumber && m_fileSeqNumber > 0) {
      FileIncident endFileIncident(name(), "EndInputFile", "SHM");
      m_incidentSvc->fireIncident(endFileIncident);
      const_cast<AthenaSharedMemoryTool*>(this)->m_fileSeqNumber = evtH->fileSeqNumber;
      FileIncident beginFileIncident(name(), "BeginInputFile", "SHM");
      m_incidentSvc->fireIncident(beginFileIncident);
   }
   ATH_MSG_DEBUG("Locking eventNumber = " << eventNumber);
   evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putObject(const void* source, size_t nbytes, int num) const {
   if (nbytes > m_maxSize) {
      ATH_MSG_ERROR("Object size = " << nbytes << " greater than maximum for client = " << num);
      return(StatusCode::FAILURE);
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   ShareEventHeader::ProcessStatus evtStatus = evtH->evtProcessStatus; // read only once
   if (evtStatus != ShareEventHeader::CLEARED && evtStatus != ShareEventHeader::PARTIAL) {
      ATH_MSG_DEBUG("Waiting for CLEARED putObject, client = " << num << ", in state " << evtStatus);
      return(StatusCode::RECOVERABLE);
   }
   int numSegments = m_multipleSegments.value() ? num : 0;
   if (source == nullptr) {
      evtH->evtSize = evtH->evtOffset;
      evtH->evtOffset = 0;
      m_payload->flush(numSegments * m_maxSize + evtH->evtOffset + evtH->evtCursor, evtH->evtSize - evtH->evtCursor);
      m_status->flush(num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
      evtH->evtProcessStatus = ShareEventHeader::FILLED;
   } else {
      if (evtH->evtOffset + sizeof(size_t) + nbytes > m_maxSize) {
         ATH_MSG_ERROR("Object location = " << evtH->evtOffset << " greater than maximum for client = " << num);
         return(StatusCode::FAILURE);
      }
      bool first = (evtH->evtOffset == 0);
      std::memcpy(static_cast<char*>(m_payload->get_address()) + numSegments * m_maxSize + evtH->evtOffset, &nbytes, sizeof(size_t));
      evtH->evtOffset += sizeof(size_t);
      std::memcpy(static_cast<char*>(m_payload->get_address()) + numSegments * m_maxSize + evtH->evtOffset, source, nbytes);
      evtH->evtOffset += nbytes;
      if (evtH->evtSize == m_maxSize) {
         evtH->evtSize = evtH->evtOffset;
         evtH->evtProcessStatus = ShareEventHeader::PARTIAL;
      }
      if (first) {
         evtH->evtSize = m_maxSize;
         m_payload->flush(numSegments * m_maxSize, evtH->evtOffset);
         m_status->flush(num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
      }
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getObject(void** target, size_t& nbytes, int num) const {
   if (target == nullptr) {
      return(StatusCode::SUCCESS);
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   ShareEventHeader::ProcessStatus evtStatus = evtH->evtProcessStatus; // read only once
   size_t evtSize = evtH->evtSize; // read only once
   if (evtStatus == ShareEventHeader::PARTIAL && evtH->evtCursor > 0) {
      ATH_MSG_DEBUG("Waiting for UNPARTIAL getObject, client = " << num);
      nbytes = evtH->evtCursor;
      return(StatusCode::RECOVERABLE);
   }
   if (evtStatus != ShareEventHeader::FILLED &&
	   evtStatus != ShareEventHeader::SHARED &&
	   evtStatus != ShareEventHeader::PARTIAL) {
      ATH_MSG_DEBUG("Waiting for FILLED getObject, client = " << num);
      nbytes = 0;
      return(StatusCode::RECOVERABLE);
   }
   if (evtH->evtCursor < evtSize) {
      int numSegments = m_multipleSegments.value() ? num : 0;
      std::memcpy(&nbytes, static_cast<char*>(m_payload->get_address()) + numSegments * m_maxSize + evtH->evtCursor, sizeof(size_t));
      evtH->evtCursor += sizeof(size_t);
      *target = static_cast<char*>(m_payload->get_address()) + numSegments * m_maxSize + evtH->evtCursor;
      if (evtStatus != ShareEventHeader::PARTIAL) {
         evtH->evtProcessStatus = ShareEventHeader::SHARED;
      }
      evtH->evtCursor += nbytes;
   } else {
      nbytes = 0;
   }
   if (evtH->evtCursor == evtSize) {
      if (evtH->evtProcessStatus == ShareEventHeader::SHARED) {
         evtH->evtProcessStatus = ShareEventHeader::FILLED;
      } else {
         evtH->evtCursor = 0;
         m_status->flush(num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
         while (evtH->evtProcessStatus != ShareEventHeader::FILLED) {
            usleep(10);
         }
         evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
      }
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::clearObject(const char** tokenString, int& num) const {
   if (m_isClient) {
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      if (evtH->evtProcessStatus != ShareEventHeader::CLEARED) {
         ATH_MSG_DEBUG("Waiting for FILL clearObject");
         return(StatusCode::RECOVERABLE);
      }
      *tokenString = evtH->token;
      evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
      return(StatusCode::SUCCESS);
   }

   if (!m_multipleSegments.value()) {
      for (int i = 1; i <= m_num; i++) { // FIXME: PvG, do round robin
         void* status = static_cast<char*>(m_status->get_address()) + i * sizeof(ShareEventHeader);
         ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
         ShareEventHeader::ProcessStatus evtStatus = evtH->evtProcessStatus; // read only once
         if (evtStatus == ShareEventHeader::FILLED || evtStatus == ShareEventHeader::SHARED) {
            ATH_MSG_DEBUG("Waiting for UNFILL clearObject, client = " << i);
            return(StatusCode::RECOVERABLE);
         } else if (i == num && evtStatus != ShareEventHeader::LOCKED) {
            ATH_MSG_DEBUG("Waiting for LOCK clearObject, client = " << i);
            return(StatusCode::RECOVERABLE);
         } else if ((i == num || num < 0) && evtStatus == ShareEventHeader::LOCKED) {
            *tokenString = evtH->token;
            num = i;
            ATH_MSG_DEBUG("Setting LOCK clearObject, for client = " << num << ": " << evtH->token);
            break;
         }
      }

   } else if (num >= 0) {
      void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
      if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
         *tokenString = evtH->token;
         ATH_MSG_DEBUG("Setting LOCK clearObject, client = " << num << ": " << evtH->token);
      } else {
         ATH_MSG_DEBUG("Waiting for LOCK clearObject, client = " << num);
         return(StatusCode::RECOVERABLE);
      }
   } else {
      for (int i = 1; i <= m_num; i++) { // FIXME: PvG, do round robin
         void* status = static_cast<char*>(m_status->get_address()) + i * sizeof(ShareEventHeader);
         ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
         if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
            *tokenString = evtH->token;
            num = i;
            ATH_MSG_DEBUG("Setting LOCK clearObject, for client = " << num << ": " << evtH->token);
            break;
         }
      }
   }

   if (num > 0 && *tokenString != nullptr) {
      void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
      if (strncmp(*tokenString, "release", 7) == 0) {
         ATH_MSG_INFO("Server clearObject() got release, client = " << num);
         evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
         num = -1;
         return(StatusCode::SUCCESS);
      }
      if (strncmp(*tokenString, "start", 5) == 0) {
         ATH_MSG_INFO("Server clearObject() got start, client = " << num << ", of " << m_num - 1);
         if (m_dataClients.empty()) { // Annouce all workers to prevent early writer termination
            for (int i = 1; i < m_num - 1; i++) m_dataClients.insert(i);
         }
         m_dataClients.insert(num);
         evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
         num = -1;
         return(StatusCode::SUCCESS);
      }
      if (strncmp(*tokenString, "stop", 4) == 0) {
         ATH_MSG_INFO("Server clearObject() got stop, client = " << num);
         m_dataClients.erase(num);
         evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
         if (m_dataClients.empty()) {
            ATH_MSG_INFO("Server clearObject() got stop, client ALL: " << m_num);
            if (num == m_num - 1) { // mother process
              return(StatusCode::SUCCESS);
            }
            return(StatusCode::FAILURE);
         }
         num = -1;
         return(StatusCode::SUCCESS);
      }
      evtH->evtSize = 0;
      m_status->flush(0, sizeof(ShareEventHeader));
      evtH->evtProcessStatus = ShareEventHeader::CLEARED;
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::RECOVERABLE);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::lockObject(const char* tokenString, int num) const {
   if (strlen(tokenString) >= maxTokenLength) {
      ATH_MSG_ERROR("Token = " << tokenString << ", too long for AthenaSharedMemoryTool");
      return(StatusCode::FAILURE);
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   if (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
      ATH_MSG_DEBUG("Waiting for UNLOCKED lockObject: " << tokenString);
      return(StatusCode::RECOVERABLE);
   }
   strncpy(evtH->token, tokenString, maxTokenLength - 1); evtH->token[maxTokenLength - 1] = 0;
   if (m_isServer) {
      evtH->evtSize = 0;
      m_status->flush(num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
      evtH->evtProcessStatus = ShareEventHeader::CLEARED;
   } else {
      m_status->flush(num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
      evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   }
   return(StatusCode::SUCCESS);
}
