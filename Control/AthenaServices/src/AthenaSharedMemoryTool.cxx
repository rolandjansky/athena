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
   char token[maxTokenLength];
};

//___________________________________________________________________________
AthenaSharedMemoryTool::AthenaSharedMemoryTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent),
		m_maxSize(16 * 1024 * 1024),
		m_maxDataClients(256),
		m_num(-1),
		m_dataClients(),
		m_payload(0),
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
   shm.truncate(m_maxSize + m_maxDataClients * sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::stop() {
   ATH_MSG_INFO("in stop()");
   if (m_isClient && m_num > 0) {
      ATH_MSG_INFO("Client stop() inform Server: " << m_num);
      m_num = -1;
      lockObject("stop").ignore();
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
   m_num = num;
   m_isServer = true;
   boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, 0, m_maxSize);
   m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxSize, num * sizeof(ShareEventHeader));
   ShareEventHeader evtH = { ShareEventHeader::UNLOCKED, -1, -1, 0, 0, 0, "" };
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
   if (m_isServer) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Client.");
      return(StatusCode::FAILURE);
   }
   if (num >= m_maxDataClients) {
      ATH_MSG_ERROR("Too many clients for AthenaSharedMemoryTool.");
      return(StatusCode::FAILURE);
   }
   if (m_num > 0 && num <= 0) {
      m_num = -1;
      lockObject("stop").ignore();
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      while (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
         usleep(100);
      }
   }
   if (!m_isClient) {
      m_isClient = true;
      boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	      m_sharedMemory.value().c_str(),
	      boost::interprocess::read_write);
      if (m_payload == 0) {
         m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, 0, m_maxSize);
         m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxSize + num * sizeof(ShareEventHeader), sizeof(ShareEventHeader));
      }
   }
   if (m_num <= 0 && num > 0) {
      lockObject("start").ignore();
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
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
StatusCode AthenaSharedMemoryTool::putEvent(long eventNumber, const void* source, std::size_t nbytes, unsigned int status) {
   if (nbytes > m_maxSize) {
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
      evtH->evtSeqNumber = 0;
      evtH->fileSeqNumber = 0;
      evtH->evtSize = 0;
      evtH->evtOffset = 0;
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
StatusCode AthenaSharedMemoryTool::lockEvent(long eventNumber) {
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
      m_fileSeqNumber = evtH->fileSeqNumber;
      FileIncident beginFileIncident(name(), "BeginInputFile", "SHM");
      m_incidentSvc->fireIncident(beginFileIncident);
   }
   ATH_MSG_DEBUG("Locking eventNumber = " << eventNumber);
   evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putObject(const void* source, size_t nbytes, int num) {
   if (nbytes > m_maxSize) {
      ATH_MSG_ERROR("Object size = " << nbytes << " greater than maximum for client = " << num);
      return(StatusCode::FAILURE);
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   if (evtH->evtProcessStatus != ShareEventHeader::CLEARED) {
      ATH_MSG_DEBUG("Waiting for CLEARED putObject, client = " << num);
      return(StatusCode::RECOVERABLE);
   }
   if (source != 0 && source != m_payload->get_address()) {
      std::memcpy(m_payload->get_address(), source, nbytes);
      evtH->evtSize = nbytes;
   } else {
      evtH->evtSize = 0;
   }
   evtH->evtProcessStatus = ShareEventHeader::FILLED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getObject(void** target, size_t& nbytes, int num) {
   if (target == 0) {
      return(StatusCode::SUCCESS);
   }
   void* status = static_cast<char*>(m_status->get_address()) + num * sizeof(ShareEventHeader);
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
   if (evtH->evtProcessStatus != ShareEventHeader::FILLED) {
      ATH_MSG_DEBUG("Waiting for FILLED getObject, client = " << num);
      return(StatusCode::RECOVERABLE);
   }
   nbytes = evtH->evtSize;
   if (nbytes > 0) {
      char* buf = new char[nbytes];
      std::memcpy(buf, static_cast<char*>(m_payload->get_address()), nbytes);
      *target = buf;
   } else {
      *target = m_payload->get_address();
   }
   evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::clearObject(char** tokenString, int& num) {
   if (m_isClient) {
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
      if (evtH->evtProcessStatus != ShareEventHeader::CLEARED) {
         ATH_MSG_DEBUG("Waiting for FILL clearObject");
         return(StatusCode::RECOVERABLE);
      }
      if (*tokenString == 0) {
         *tokenString = new char[maxTokenLength];
      }
      ATH_MSG_DEBUG("Getting Token in clearObject");
      strncpy(*tokenString, evtH->token, maxTokenLength - 1); (*tokenString)[maxTokenLength - 1] = 0;
      evtH->evtProcessStatus = ShareEventHeader::UNLOCKED;
      return(StatusCode::SUCCESS);
   }
   for (int i = 1; i <= m_num; i++) { // FIXME: PvG, do round robin
      void* status = static_cast<char*>(m_status->get_address()) + i * sizeof(ShareEventHeader);
      ShareEventHeader* evtH = static_cast<ShareEventHeader*>(status);
      if (evtH->evtProcessStatus == ShareEventHeader::FILLED) {
         ATH_MSG_DEBUG("Waiting for UNFILL clearObject, client = " << i);
         return(StatusCode::RECOVERABLE);
      } else if (i == num && evtH->evtProcessStatus != ShareEventHeader::LOCKED) {
         ATH_MSG_DEBUG("Waiting for LOCK clearObject, client = " << i);
         return(StatusCode::RECOVERABLE);
      } else if ((i == num || num < 0) && evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
         if (*tokenString == 0) {
            *tokenString = new char[maxTokenLength];
         }
         ATH_MSG_DEBUG("Getting Token in clearObject, client = " << i);
         strncpy(*tokenString, evtH->token, maxTokenLength - 1); (*tokenString)[maxTokenLength - 1] = 0;
         num = i;
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
         ATH_MSG_INFO("Server clearObject() got start, client = " << num);
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
            ATH_MSG_INFO("Server clearObject() got stop, client ALL");
            return(StatusCode::FAILURE);
         }
         num = -1;
         return(StatusCode::SUCCESS);
      }
      evtH->evtProcessStatus = ShareEventHeader::CLEARED;
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::RECOVERABLE);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::lockObject(const char* tokenString, int num) {
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
      evtH->evtProcessStatus = ShareEventHeader::CLEARED;
   } else {
      evtH->evtProcessStatus = ShareEventHeader::LOCKED;
   }
   return(StatusCode::SUCCESS);
}
