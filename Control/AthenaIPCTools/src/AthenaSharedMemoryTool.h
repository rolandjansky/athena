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

#include <set>
#include <string>

// Forward declarations.
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
   StatusCode stop();
   StatusCode finalize();

   StatusCode makeServer(int num);
   bool isServer() const;
   StatusCode makeClient(int num);
   bool isClient() const;

   StatusCode putEvent(long eventNumber, const void* source, size_t nbytes, unsigned int status) const;
   StatusCode getLockedEvent(void** target, unsigned int& status) const;
   StatusCode lockEvent(long eventNumber) const;

   StatusCode putObject(const void* source, size_t nbytes, int num = 0) const;
   StatusCode getObject(void** target, size_t& nbytes, int num = 0) const;
   StatusCode clearObject(const char** tokenString, int& num) const;
   StatusCode lockObject(const char* tokenString, int num = 0) const;

private:
   StringProperty m_sharedMemory;
   const size_t m_maxSize;
   const int m_maxDataClients;
   int m_num;
   mutable std::set<int> m_dataClients;
   boost::interprocess::mapped_region* m_payload;
   boost::interprocess::mapped_region* m_status;
   long m_fileSeqNumber;
   bool m_isServer;
   bool m_isClient;
   ServiceHandle<IIncidentSvc> m_incidentSvc;
};

#endif
