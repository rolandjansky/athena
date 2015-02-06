/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaInterprocess/IdentifiedSharedQueue.h"

#include <boost/interprocess/ipc/message_queue.hpp>

#include <sstream>

using namespace boost::interprocess;


//const std::size_t MAX_MSG_SIZE = 4096;

namespace AthenaInterprocess {   

//- construction/destruction -------------------------------------------------
IdentifiedSharedQueue::IdentifiedSharedQueue() : SharedQueue()
{
/* empty */
}

  IdentifiedSharedQueue::IdentifiedSharedQueue( const std::string& name, int max_msg, std::size_t max_size, bool do_unlink ) :
    SharedQueue( name, max_msg, max_size, do_unlink )
{
/* empty */
}

//- public member functions --------------------------------------------------
inline std::string add_pid( const std::string& buf )
{
   std::ostringstream s;
   s << (long)getpid() << '\0' << buf;
   return s.str();
}

bool IdentifiedSharedQueue::try_send( const std::string& buf )
{
   return this->SharedQueue::try_send( add_pid( buf ) );
}

bool IdentifiedSharedQueue::send( const std::string& buf )
{
   return this->SharedQueue::send( add_pid( buf ) );
}

inline std::string get_pid( const std::string& buf, pid_t &pid )
{
   std::istringstream s( buf );
   long id = -1;
   s >> id;
   pid = (pid_t)id;
   std::string::size_type pos = s.tellg();
   if ( (pos+1) <= buf.size() )
      return buf.substr( pos+1 );
   return "";
}
  

std::string IdentifiedSharedQueue::try_receive()
{
   pid_t pid;
   return try_receive( pid );
}

std::string IdentifiedSharedQueue::try_receive( pid_t& id )
{
   const std::string& buf = this->SharedQueue::try_receive();
   return get_pid( buf, id );
}

std::string IdentifiedSharedQueue::receive()
{
   pid_t pid;
   return receive( pid );
}

std::string IdentifiedSharedQueue::receive( pid_t& id )
{
   const std::string& buf = this->SharedQueue::receive();
   return get_pid( buf, id );
}

} // namespace AthenaInterprocess
