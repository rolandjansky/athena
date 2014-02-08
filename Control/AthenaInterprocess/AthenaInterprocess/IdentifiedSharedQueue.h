/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_IDENTIFIEDSHAREDQUEUE_H
#define ATHENAINTERPROCESS_IDENTIFIEDSHAREDQUEUE_H

#include "AthenaInterprocess/SharedQueue.h"

#include <unistd.h>

namespace AthenaInterprocess {

class IdentifiedSharedQueue : public SharedQueue {
public:

public:
   IdentifiedSharedQueue();
   IdentifiedSharedQueue( const std::string& name,
			  int max_msg = SHAREDQUEUE_MAX_MSG, 
			  std::size_t max_size = MAX_MSG_SIZE,
			  bool do_unlink = true );

public:
   virtual bool try_send( const std::string& );     // non-blocking
   virtual bool send( const std::string& );

   virtual std::string try_receive();                // non-blocking
   virtual std::string try_receive( pid_t& id );     // id.
   virtual std::string receive();
   virtual std::string receive( pid_t& id );
};

} // namespace AthenaInterprocess

#endif // !ATHENAINTERPROCESS_IDENTIFIEDSHAREDQUEUE_H
