/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_PROCESS_H
#define ATHENAINTERPROCESS_PROCESS_H

#include "AthenaInterprocess/SharedQueue.h"
#include "AthenaInterprocess/IdentifiedSharedQueue.h"
#include "AthenaInterprocess/IMessageDecoder.h"

#include <string>
#include <unistd.h>

namespace AthenaInterprocess {

class Process {
public:
   static Process launch();

public:
   explicit Process(pid_t pid);
   Process( const Process& other );
   Process& operator=( const Process& other );
   virtual ~Process();

   pid_t getProcessID() const;

   bool connectIn( const SharedQueue& queue );
   bool connectOut( const IdentifiedSharedQueue& queue );
   bool schedule(const IMessageDecoder* func, const ScheduledWork* args);

   int mainloop();

private:
   SharedQueue           m_inbox;
   IdentifiedSharedQueue m_outbox;
   pid_t m_pid;
};

} // namespace AthenaInterprocess

#endif // !ATHENAINTERPROCESS_PROCESS_H
