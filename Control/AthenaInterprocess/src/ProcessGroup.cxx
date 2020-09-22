/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/SharedQueue.h"
#include "AthenaInterprocess/Utilities.h"

#include <boost/interprocess/ipc/message_queue.hpp>

#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sstream>
#include <iostream>

using namespace boost::interprocess;


//- helper -------------------------------------------------------------------
static inline AthenaInterprocess::SharedQueue create_queue( const std::string& owner, int count )
{
   std::ostringstream s;
   s << "athenamp_" << owner << '_' << getpid() << '_' << count << '_' << AthenaInterprocess::randString() << std::ends;
   AthenaInterprocess::SharedQueue queue = AthenaInterprocess::SharedQueue( s.str() );
   return queue;
}

namespace AthenaInterprocess {

//- construction/destruction -------------------------------------------------
ProcessGroup::ProcessGroup(int nprocs) 
  : m_nprocs(nprocs)
  , m_pgid(-1)
  , m_processesCreated(0)
{
   if ( m_nprocs < 0 ) {
   // determine number of cores from system (available ones only)
      m_nprocs = sysconf( _SC_NPROCESSORS_ONLN );
   }
}

ProcessGroup::~ProcessGroup()
{
}

pid_t ProcessGroup::launchProcess ATLAS_NOT_THREAD_SAFE ()
{
  if(m_pgid==-1)
    return -1;

  // Create new process
  SharedQueue queue = create_queue("child", m_processesCreated); 
  Process p = Process::launch();
  p.connectIn(queue);
  p.connectOut(m_inbox);
  pid_t newpid = p.getProcessID();
  if(newpid==0) {
    int status = p.mainloop();
    exit(status);
  }

  if(m_processes.empty()) 
    m_pgid = getpgid(newpid);
  else
    setpgid(newpid, m_pgid);

  m_processes.push_back(p);
  m_processesCreated++;
  return newpid;
}

int ProcessGroup::map_async ATLAS_NOT_THREAD_SAFE (const IMessageDecoder* func, const ScheduledWork* args, pid_t pid) {
  // If pid=0, map the function-object 'func' onto all current child processes. Does
  // not wait for the results, but will return success only if the writing to
  // all the child queues succeeds.
  //
  // If pid!=0 then map only onto the process with given pid
  if(m_processes.empty()) {
    if(!create())
      return -1;
  }

  if(pid==0) { // Map on all processes in the group
    for(std::vector<Process>::iterator iproc = m_processes.begin();
	iproc!=m_processes.end(); ++iproc) {

      if(!iproc->schedule(func, args)) {
	// stopping the scheduling on all other processes is debatable ...
	return -1;
      }
    }
    return 0;
  }
  else { // Map only onto the given pid
    for(std::vector<Process>::iterator iproc=m_processes.begin();
	iproc!=m_processes.end(); ++iproc) {
      if(iproc->getProcessID()==pid)
	return (iproc->schedule(func,args) ? 0 : -1);
    }
    // pid not found in the group
    return -1;
  }
}

int ProcessGroup::wait ATLAS_NOT_THREAD_SAFE (int options)
{
  // Wait for all child processes and store their status codes in m_statuses
  if(m_processes.empty())
      return 0;

  // Schedule an exit if we are to wait forever
  if(!(options & WNOHANG) && map_async(0,0))
    return -1;

   int result = 0;
   while(m_processes.size()) {
     int child_status = 0;
     pid_t child = waitpid(-m_pgid, &child_status, options);
     if(child < 0) {
       result = errno;
       break;
     }
     else if(child==0) {
       continue;
     }

     if(WIFSIGNALED(child_status))
       std::cout << "SIGNAL! " << child << " (" << child_status << "," << WTERMSIG(child_status) << ")" << std::endl;

      child_status = WIFSIGNALED(child_status) ? WTERMSIG(child_status) : WEXITSTATUS(child_status);
      ProcessStatus p = {child, child_status};
      m_statuses.push_back(p);

      // remove the process from m_processes
      for(std::vector<Process>::iterator iproc=m_processes.begin();
	  iproc!=m_processes.end(); ++iproc) {
	if(iproc->getProcessID()==child) {
	  m_processes.erase(iproc);
	  break;
	}
      }
   }
   return result;
}

pid_t ProcessGroup::wait_once(bool& flag)
{
  flag = true;
  if(m_processes.empty()) return 0;
  int child_status = 0;
  pid_t child = waitpid(-m_pgid, &child_status,WNOHANG);
  if(child>0) {
    // Check if the process has failed
    flag = !(WIFSIGNALED(child_status) || WEXITSTATUS(child_status));
    // save the status
    child_status = WIFSIGNALED(child_status) ? WTERMSIG(child_status) : WEXITSTATUS(child_status);
    ProcessStatus p = {child, child_status};
    m_statuses.push_back(p);
    // remove the process from m_processes
    for(std::vector<Process>::iterator iproc=m_processes.begin();
	iproc!=m_processes.end(); ++iproc) {
      if(iproc->getProcessID()==child) {
	m_processes.erase(iproc);
	break;
      }
    }
  }
  else if(child<0) {
    flag = false;
  }
  return child;  
}

ProcessResult* ProcessGroup::pullOneResult()
{
  ProcessResult* result(0);
  pid_t pid = -1;
  std::string buf = m_inbox.try_receive( pid );
  if(pid>0) {
    result = new ProcessResult;
    result->pid = pid;
    result->output.data = malloc(buf.size());
    memcpy(result->output.data,buf.data(),buf.size());
    result->output.size = buf.size();
  }
  return result;
}

pid_t ProcessGroup::getGroupID() const
{
   return m_pgid;
}

const std::vector<Process>& ProcessGroup::getChildren() const
{
// Give access to the processes; this is temporarily needed for the python
// interface, but it is better broken up into the actual needs).
   return m_processes;
}

const std::vector<ProcessStatus>& ProcessGroup::getStatuses() const
{
  return m_statuses;
}

bool ProcessGroup::create ATLAS_NOT_THREAD_SAFE ()
{
// TODO: this code leaves the queues from the previous children visible to all
// their subsequent siblings. This can be helped by creating the queue first
// in the children, but the current code at least requires no synchronization,
// and has a minimum chance of leaving resources on failure.

   if ( m_nprocs <= 0 )
      return false;

// a single queue for posting back onto the mother
   if ( ! m_inbox ) {
      std::ostringstream s;
      s << "athenamp_mother_" << getpid() << '_' << AthenaInterprocess::randString() << std::ends;
      m_inbox = AthenaInterprocess::IdentifiedSharedQueue( s.str() );
   }

// create process group leader
   SharedQueue queue = create_queue( "child", 0 );
   Process leader = Process::launch();
   leader.connectIn( queue );
   leader.connectOut( m_inbox );
   pid_t lpid = leader.getProcessID();
   if ( lpid == 0 ) {
      //m_tool->setRankId(0);
      int status = leader.mainloop();
      exit( status );  
   } else if ( lpid == -1 )
      return false;

   setpgid( lpid, 0 );
   m_processes.push_back( leader );
   m_pgid = getpgid( lpid );
 
// create rest of the group
   for ( int i = 1; i < m_nprocs; ++i ) {
      SharedQueue queue = create_queue( "child", i );
      Process p = Process::launch();
      p.connectIn( queue );
      p.connectOut( m_inbox );
      if ( p.getProcessID() == 0 ) {
         int status = p.mainloop();
         exit( status );
      }
      setpgid( p.getProcessID(), m_pgid );
      m_processes.push_back( p );
   }
   m_processesCreated = m_nprocs;
   return true;
}

} // namespace AthenaInterprocess
