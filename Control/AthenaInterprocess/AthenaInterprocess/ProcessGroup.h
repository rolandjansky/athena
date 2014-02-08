/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_PROCESSGROUP_H
#define ATHENAINTERPROCESS_PROCESSGROUP_H

#include "AthenaInterprocess/Process.h"
#include "AthenaInterprocess/IdentifiedSharedQueue.h"
#include <vector>

namespace AthenaInterprocess {

class IMessageDecoder;

struct ProcessStatus {
   pid_t pid;
   int   exitcode;
};

struct ProcessResult {
   pid_t pid;
   ScheduledWork output;
};

class ProcessGroup {
 public:
   explicit ProcessGroup(int nprocs = -1);

   virtual ~ProcessGroup();

   pid_t launchProcess();  // Add one new process to the group

   int map_async(const IMessageDecoder* func, const ScheduledWork* args, pid_t pid=0); // If pid=0 map on the entire group
   int wait(int options = 0); 
   pid_t wait_once(bool& flag); // flag=true if process succeeded, flag=false otherwise
   ProcessResult* pullOneResult(); // The caller takes ownership on the result 

   pid_t getGroupID() const;
   const std::vector<Process>& getChildren() const;
   const std::vector<ProcessStatus>& getStatuses() const;

 private:
   bool create();

   std::vector<Process> m_processes;
   std::vector<ProcessStatus> m_statuses;
   IdentifiedSharedQueue m_inbox;
   int m_nprocs;
   pid_t m_pgid;
   int m_processesCreated;  // Keep track of total number of created processes
                            // This can be more than m_nprocs if some processes have been launched later on
                            // (restarting failed processes)
};

} // namespace AthenaInterprocess

#endif // !ATHENAINTERPROCESS_PROCESSGROUP_H
