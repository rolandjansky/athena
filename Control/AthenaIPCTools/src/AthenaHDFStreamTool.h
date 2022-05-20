/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAHDFSTREAMTOOL_H
#define ATHENAHDFSTREAMTOOL_H

/** @file AthenaHDFStreamTool.h
 *  @brief This file contains the class definition for the AthenaHDFStreamTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaIPCTool.h"

#include <string>

// Forward declarations.
class IIncidentSvc;

namespace H5 {
   class H5File;
   class Group;
}

/** @class AthenaHDFStreamTool
 *  @brief This class provides the IPCTool for HDF Stream objects
 **/
/* To use this tool to
 * write data to HDF5 add the following jobOption fragment:
 *
 * from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 * import AthenaPoolCnvSvc.AthenaPool
 * from AthenaIPCTools.AthenaIPCToolsConf import AthenaHDFStreamTool
 * svcMgr.AthenaPoolCnvSvc.OutputStreamingTool += [ AthenaHDFStreamTool("OutputStreamingTool") ]
 * svcMgr.AthenaPoolCnvSvc.MakeStreamingToolClient = 1
 * svcMgr.AthenaPoolCnvSvc.ParallelCompression=False
 *
 * To read it back use:
 * from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 * import AthenaPoolCnvSvc.ReadAthenaPool
 * from AthenaIPCTools.AthenaIPCToolsConf import AthenaHDFStreamTool
 * svcMgr.EventSelector.SharedMemoryTool = AthenaHDFStreamTool("EventStreamingTool")
 * svcMgr.EventSelector.MakeStreamingToolClient = -1
 * svcMgr.AthenaPoolCnvSvc.InputStreamingTool = AthenaHDFStreamTool("InputStreamingTool")
 * svcMgr.AthenaPoolCnvSvc.MakeStreamingToolClient = 1
 * svcMgr.AthenaPoolCnvSvc.StreamingTechnology = 1025
 *
 **/

class AthenaHDFStreamTool : public ::AthAlgTool, virtual public IAthenaIPCTool {
public: 
   /// Standard Service Constructor
   AthenaHDFStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~AthenaHDFStreamTool();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   StatusCode makeServer(int num, const std::string& streamPortSuffix);
   bool isServer() const;
   StatusCode makeClient(int num, std::string& streamPortSuffix);
   bool isClient() const;

   StatusCode putEvent(long eventNumber, const void* source, std::size_t nbytes, unsigned int status) const;
   StatusCode getLockedEvent(void** target, unsigned int& status) const;
   StatusCode lockEvent(long eventNumber) const;

   StatusCode putObject(const void* source, std::size_t nbytes, int num = 0) const;
   StatusCode getObject(void** target, std::size_t& nbytes, int num = 0) const;
   StatusCode clearObject(const char** tokenString, int& num) const;
   StatusCode lockObject(const char* tokenString, int num = 0) const;

private:
   H5::H5File* m_file;
   H5::Group* m_group;
   mutable std::string m_token;
   mutable char* m_read_data;
   mutable std::size_t  m_read_size;
   mutable std::size_t  m_read_position;
   mutable long long unsigned int m_event_iter;
   bool m_isClient;
   ServiceHandle<IIncidentSvc> m_incidentSvc;
};

#endif
