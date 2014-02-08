/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaInterprocess/SharedQueue.h"

#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>

using namespace boost::interprocess;


namespace AthenaInterprocess {   

//- construction/destruction -------------------------------------------------
SharedQueue::SharedQueue() : m_queue( 0 ), m_name( 0 ), m_count( 0 )
{
/* empty */
}

  SharedQueue::SharedQueue( const std::string& name, int max_msg, std::size_t max_size, bool do_unlink ) :
      m_queue( 0 ), m_name( 0 ), m_count( 0 )
{
   m_queue = new message_queue( open_or_create, name.c_str(), max_msg, max_size );
   if ( do_unlink )
      message_queue::remove( name.c_str() );

   m_count = new int( 1 );
   m_name = new std::string( name );
}

SharedQueue::SharedQueue( const SharedQueue& other )
{
   copy( other );
}

SharedQueue& SharedQueue::operator=( const SharedQueue& other )
{
   if ( &other != this ) {
      destroy();
      copy( other );
   }

   return *this;
}

SharedQueue::~SharedQueue()
{
   destroy();
}


void SharedQueue::copy( const SharedQueue& other )
{
   if ( other.m_count ) {
      *other.m_count += 1;
      m_count = other.m_count;
      m_queue = other.m_queue;
      m_name = other.m_name;
   } else {
      m_count = 0;
      m_queue = 0;
      m_name = 0;
   }
}

void SharedQueue::destroy()
{
   if ( m_count && --*m_count <= 0 ) {
      delete m_queue; m_queue = 0;
   //      message_queue::remove( m_name->c_str() );
      delete m_name; m_name = 0;
      delete m_count; m_count = 0;
   }
}

//- public member functions --------------------------------------------------
std::string SharedQueue::name() const
{
   if ( m_name )
      return *m_name;
   return "";
}


static inline bool do_send( message_queue* mq, const std::string& buf, bool block )
{
   bool send_ok = true;

   try {
      if ( block )
         mq->send( buf.data(), buf.size(), 0 );
      else
         send_ok = mq->try_send( buf.data(), buf.size(), 0 );
   } catch ( interprocess_exception& e ) {
      send_ok = false;
   }

   return send_ok;
}

bool SharedQueue::try_send( const std::string& buf )
{
   return do_send( m_queue, buf, false );
}

bool SharedQueue::send( const std::string& buf )
{
   return do_send( m_queue, buf, true );
}

static inline std::string do_receive( message_queue* mq, bool block )
{
   char buf[ MAX_MSG_SIZE ];

   try {
      std::size_t recvd_size = 0;
      unsigned int priority = 0;

      bool result = true;
      if ( block ) 
         mq->receive( buf, MAX_MSG_SIZE, recvd_size, priority );
      else
         result = mq->try_receive( buf, MAX_MSG_SIZE, recvd_size, priority );

      if ( result ) {
         if ( recvd_size > MAX_MSG_SIZE )
            recvd_size = MAX_MSG_SIZE;      // this is debatable, but send
                                            // should have failed already, so
                                            // "can not happen" applies
         return std::string( buf, recvd_size );
      }
   } catch ( interprocess_exception& e ) {
   /* silent for now: should return a callable error function */
   }

   return "";
}

std::string SharedQueue::try_receive()
{
  return do_receive( m_queue, false );
}

std::string SharedQueue::receive()
{
   return do_receive( m_queue, true );
}

} // namespace AthenaInterprocess
