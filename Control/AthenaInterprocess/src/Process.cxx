/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaInterprocess/Process.h"

#include <boost/interprocess/ipc/message_queue.hpp>

#include <sys/prctl.h>
#include <dlfcn.h>
#include <signal.h>

#include <iostream>

using namespace boost::interprocess;


namespace AthenaInterprocess {   

  Process Process::launch()
{
   std::cout.flush();
   std::cerr.flush();

   pid_t pid = fork();
   if ( pid == 0 )
      prctl( PR_SET_PDEATHSIG, SIGHUP );          // Linux only

   return Process(pid);
}


//- construction/destruction -------------------------------------------------
Process::Process(pid_t pid ) : m_inbox(), m_outbox(), m_pid( pid ) 
{

}

Process::Process( const Process& other ) :
  m_inbox( other.m_inbox ), m_outbox( other.m_outbox ), m_pid( other.m_pid )
{

}

Process& Process::operator=( const Process& other )
{
   if ( this != &other ) {
      m_inbox = other.m_inbox;
      m_outbox = other.m_outbox;
      m_pid = other.m_pid;
   }
   return *this;
}

Process::~Process()
{

}


//- public member functions --------------------------------------------------
pid_t Process::getProcessID() const
{
   return m_pid;
}

bool Process::connectIn( const SharedQueue& queue )
{
   if ( ! queue.name().empty() ) {
      m_inbox = queue;
   /* TODO: check that the queue is valid and can receive messages */
      return true;
   }

   return false;
}

bool Process::connectOut( const IdentifiedSharedQueue& queue )
{
   if ( ! queue.name().empty() ) {
      m_outbox = queue;
   /* TODO: check that the queue is valid and can send messages */
      return true;
   }

   return false;
}

bool Process::schedule(const IMessageDecoder* func, const ScheduledWork* args)
{
  //Send func
  std::string strptr("");
  if(func) {
    size_t ptrsize = sizeof(func);
    char* charptr = new char[ptrsize];
    memcpy(charptr,&func,ptrsize);
    strptr=std::string(charptr,ptrsize);
    delete[] charptr;
  }
  bool send_ok = m_inbox.try_send(strptr);

  //Send params
  if(send_ok) {
    std::string strparam = (args?std::string((char*)args->data,args->size):std::string(""));
    send_ok = m_inbox.try_send(strparam);
  }
  
  return send_ok;
}

int Process::mainloop() {
  // TODO: the exit codes used here continue where the "miscellaneous" exit codes
  // in AthenaCommon/ExitCodes.py end. There's nothing to enforce that, though,
  // and it's not documented either.
  if(m_pid!=0)
    return 1;

  int exit_code = 0;

  while(true) {
    std::string message = m_inbox.receive();

    // One way to stop the loop: From "outside" (i.e. originated by ProcessGroup)
    // Just send an empty string as first message in (ptr,params) message pair
    if(message.empty()) break;

    // Decode first message into pointer
    IMessageDecoder* decoder(0);
    memcpy(&decoder,message.data(),message.size());

    message = m_inbox.receive();
    // Decode second message into params
    ScheduledWork params;
    params.data = (void*)message.data();
    params.size = message.size();

    if(decoder) {
      std::unique_ptr<ScheduledWork> outwork = (*decoder)(params);

      if(outwork) {
	bool posted = m_outbox.try_send(std::string((char*)outwork->data,outwork->size));

	// Convention: first int in the outwork->data buffer is an error flag: 0 - success, 1 - failure
	int errflag = *((int*)outwork->data);
	free(outwork->data);

	if(errflag) {
	  exit_code = 1;  // TODO: what should be the exit code here ????
	  break;
	}

	if(!posted) {
	  exit_code = 0x0A;
	  break;
	}
      }
      else {
        // Another way to stop the loop. From "inside" (i.e. originated by Message Receiver)
	// Return 0 pointer to ScheduledWork
        break;
      }
    }//if(decoder)
  }//loop
  return exit_code;
}

} // namespace AthenaInterprocess
