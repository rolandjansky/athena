/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASHAREDMEMORYTOOL_H
#define ATHENASHAREDMEMORYTOOL_H

/** @file AthenaSharedMemoryTool.h
 *  @brief This file contains the class definition for the AthenaSharedMemoryTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaIPCTool.h"

#include <string>

// Forward declarations.
class IChronoStatSvc;
class IIncidentSvc;

namespace boost {
   namespace interprocess {
      class shared_memory_object;
      class mapped_region;
   }
}

/** @class AthenaSharedMemoryTool
 *  @brief This class provides the IPCTool for SharedMemory objects
 **/
class AthenaSharedMemoryTool : public ::AthAlgTool, virtual public IAthenaIPCTool {
public: 
   /// Standard Service Constructor
   AthenaSharedMemoryTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~AthenaSharedMemoryTool();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   StatusCode makeServer();
   bool isServer() const;
   StatusCode makeClient();
   bool isClient() const;

   StatusCode putEvent(long eventNumber, const void* source, size_t num, unsigned int status);
   StatusCode getLockedEvent(void** source, unsigned int& status);
   StatusCode lockEvent(long eventNumber);
   StatusCode unlockEvent();

   void *retrieveMetadata() const;
   size_t maxMetadataSize() const;
   //long& fileSeqNumber();

private:
   StringProperty m_sharedMemory;
   const size_t m_maxEventSize;
   const size_t m_maxMetadataSize;
   boost::interprocess::mapped_region* m_payload;
   boost::interprocess::mapped_region* m_metadata;
   boost::interprocess::mapped_region* m_status;
   long m_fileSeqNumber;
   bool m_isServer;
   bool m_isClient;
   unsigned int m_sleepUSInterval;
   ServiceHandle<IChronoStatSvc> m_chronoStatSvc;
   ServiceHandle<IIncidentSvc> m_incidentSvc;
};

#endif
