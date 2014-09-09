/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaSharedMemoryTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaSharedMemoryTool.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

struct ShareEventHeader {
   enum ProcessStatus { CLEARED, FILLED, LOCKED, UNLOCKED, UNKNOWN};
   ProcessStatus evtProcessStatus;
   long evtSeqNumber;
   long fileSeqNumber;
   std::size_t evtSize;
   std::size_t evtOffset;
   unsigned int evtCoreStatusFlag;
   uint32_t evtTerm1;
   uint32_t evtTerm2;
};

//___________________________________________________________________________
AthenaSharedMemoryTool::AthenaSharedMemoryTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent),
		m_maxEventSize(4 * 1024 * 1024),
		m_maxMetadataSize(4 * 1024),
		m_payload(0),
		m_metadata(0),
		m_status(0),
		m_fileSeqNumber(0),
		m_isServer(false),
		m_isClient(false),
		m_sleepUSInterval(100000),
		m_chronoStatSvc("ChronoStatSvc", name),
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
   // Retrieve ChronoStatSvc
   if (!m_chronoStatSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ChronoStatSvc.");
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
   shm.truncate(m_maxEventSize + m_maxMetadataSize + sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   boost::interprocess::shared_memory_object::remove(m_sharedMemory.value().c_str());
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ChronoStatSvc.");
   }
   return(::AthAlgTool::finalize());
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::makeServer() {
   ATH_MSG_DEBUG("Creating shared memory object for writer.");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Server.");
      return(StatusCode::FAILURE);
   }
   m_isServer = true;
   boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, 0, m_maxEventSize);
   m_metadata = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize, m_maxMetadataSize);
   m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize + m_maxMetadataSize, sizeof(ShareEventHeader));
   ShareEventHeader evtH = { ShareEventHeader::UNLOCKED, -1, -1, 0, 0, 0, 0, 0 };
   std::memcpy(m_status->get_address(), &evtH, sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaSharedMemoryTool::isServer() const {
   return(m_isServer);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::makeClient() {
   ATH_MSG_DEBUG("Creating shared memory object for reader.");
   if (m_isServer || m_isClient) {
      ATH_MSG_ERROR("Cannot make AthenaSharedMemoryTool a Client.");
      return(StatusCode::FAILURE);
   }
   m_isClient = true;
   boost::interprocess::shared_memory_object shm(boost::interprocess::open_only,
	   m_sharedMemory.value().c_str(),
	   boost::interprocess::read_write);
   m_payload = new boost::interprocess::mapped_region(shm, boost::interprocess::read_only, 0, m_maxEventSize);
   m_metadata = new boost::interprocess::mapped_region(shm, boost::interprocess::read_only, m_maxEventSize, m_maxMetadataSize);
   m_status = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write, m_maxEventSize + m_maxMetadataSize, sizeof(ShareEventHeader));
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaSharedMemoryTool::isClient() const {
   return(m_isClient);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::putEvent(long eventNumber, const void* source, std::size_t num, unsigned int status) {
   if (num > m_maxEventSize) {
      ATH_MSG_ERROR("Event size = " << num << " greater than maximum for eventNumber = " << eventNumber);
      return(StatusCode::FAILURE);
   }
   ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   double waitTime = 0.0;
   while (evtH->evtProcessStatus != ShareEventHeader::UNLOCKED) {
      waitTime += static_cast<double>(m_sleepUSInterval) / 1000000;
      int result = usleep(m_sleepUSInterval);
      ATH_MSG_DEBUG("Waiting for putEvent, eventNumber = " << eventNumber << ", result = " << result);
   }
   m_chronoStatSvc->stat("BS_putEvent", waitTime);
   if (source == 0 && num == 0) {
      ATH_MSG_DEBUG("putEvent got last Event marker");
      return(StatusCode::SUCCESS);
   }
   evtH->evtProcessStatus = ShareEventHeader::CLEARED;
   std::memcpy(m_payload->get_address(), source, num);
   evtH->evtSeqNumber = eventNumber;
   evtH->fileSeqNumber = m_fileSeqNumber;
   evtH->evtSize = num;
   evtH->evtOffset = 0;
   evtH->evtCoreStatusFlag = status;
   evtH->evtTerm1 = *(static_cast<const uint32_t*>(source) + num / sizeof(uint32_t) - 1);
   evtH->evtTerm2 = *(static_cast<const uint32_t*>(source) + num / sizeof(uint32_t) - 2);
   evtH->evtProcessStatus = ShareEventHeader::FILLED;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaSharedMemoryTool::getLockedEvent(void** source, unsigned int& status) {
   const ShareEventHeader* evtH = static_cast<ShareEventHeader*>(m_status->get_address());
   if (evtH->evtProcessStatus == ShareEventHeader::LOCKED) {
      const std::size_t num = evtH->evtSize;
      char* buf = new char[num];
      std::memcpy(buf, static_cast<char*>(m_payload->get_address()) + evtH->evtOffset, num);
      *source = buf;
      if (evtH->evtTerm1 != *(static_cast<const uint32_t*>(*source) + num / sizeof(uint32_t) - 1) ||
	      evtH->evtTerm2 != *(static_cast<const uint32_t*>(*source) + num / sizeof(uint32_t) - 2)) {
         ATH_MSG_ERROR("Event corrupted by AthenaSharedMemoryTool");
         return(StatusCode::FAILURE);
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
   double waitTime = 0.0;
   while (evtH->evtSeqNumber != eventNumber || evtH->evtProcessStatus != ShareEventHeader::FILLED) {
      waitTime += static_cast<double>(m_sleepUSInterval) / 1000000;
      int result = usleep(m_sleepUSInterval);
      ATH_MSG_DEBUG("Waiting for lockEvent, eventNumber = " << eventNumber << ", result = " << result);
   }
   m_chronoStatSvc->stat("BS_lockEvent", waitTime);
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

void *AthenaSharedMemoryTool::retrieveMetadata() const {
    return m_metadata->get_address();
}

size_t AthenaSharedMemoryTool::maxMetadataSize() const {
    return m_maxMetadataSize;
}
/*
long& AthenaSharedMemoryTool::fileSeqNumber() {
    return m_fileSeqNumber;
}
*/
