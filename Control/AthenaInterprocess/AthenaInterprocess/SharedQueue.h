/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_SHAREDQUEUE_H
#define ATHENAINTERPROCESS_SHAREDQUEUE_H

#include "CLIDSvc/CLASS_DEF.h"

// full include needed, as message_queue has changed into a
// template for more recent versions of boost
#include <boost/interprocess/ipc/message_queue.hpp>

#include <string>

namespace AthenaInterprocess {

static const int SHAREDQUEUE_MAX_MSG = 1000;
static const std::size_t MAX_MSG_SIZE = 256;

class SharedQueue {
public:

public:
   SharedQueue();
   SharedQueue( const std::string& name,
                int max_msg = SHAREDQUEUE_MAX_MSG,
		std::size_t max_size = MAX_MSG_SIZE,
		bool do_unlink = true );
   SharedQueue( const SharedQueue& other );
   SharedQueue& operator=( const SharedQueue& other );
   virtual ~SharedQueue();

public:
   std::string name() const;

   virtual bool try_send( const std::string& );     // non-blocking
   virtual bool send( const std::string& );

   virtual std::string try_receive();               // non-blocking
   virtual std::string receive();

   template<typename T> bool try_send_basic(T);
   template<typename T> bool send_basic(T);

   template<typename T> bool try_receive_basic(T&); 
   template<typename T> bool receive_basic(T&); 

   operator bool() const {
      return (bool)m_queue;
   }

protected:
   boost::interprocess::message_queue* operator->() {
      return m_queue;
   }

private:
   void copy( const SharedQueue& other );
   void destroy();

   template<typename T> bool do_send_basic(T,bool);
   template<typename T> bool do_receive_basic(T&,bool);

private:
   boost::interprocess::message_queue* m_queue;
   std::string* m_name;
   int* m_count;
};

template<typename T> bool SharedQueue::do_send_basic(T data,bool block)
{
  bool send_ok = true;

  try {
    if(block)
      m_queue->send(&data, sizeof(data), 0);
    else
      send_ok = m_queue->try_send(&data, sizeof(data), 0);
  } 
  catch(boost::interprocess::interprocess_exception&) {
    send_ok = false;
  }

  return send_ok;
}

template<typename T> bool SharedQueue::try_send_basic(T data)
{
  return do_send_basic<T>(data,false);
}

template<typename T> bool SharedQueue::send_basic(T data)
{
  return do_send_basic<T>(data,true);
}

template<typename T> bool SharedQueue::do_receive_basic(T& data,bool block)
{
  unsigned int priority = 0;
  std::size_t recvd_size = 0;
  bool receive_ok = true;

  try {
    if(block) {
      m_queue->receive(&data,sizeof(T),recvd_size,priority);
    }
    else {
      receive_ok = m_queue->try_receive(&data,sizeof(T),recvd_size,priority);
    }
  }
  catch(boost::interprocess::interprocess_exception&) {
    return false; // TODO: perhaps add a printout or something
  }

  return receive_ok;
}

template<typename T> bool SharedQueue::try_receive_basic(T& data)
{
  return do_receive_basic(data,false);
}

template<typename T> bool SharedQueue::receive_basic(T& data)
{
  return do_receive_basic(data,true);
}

} // namespace AthenaInterprocess

CLASS_DEF(AthenaInterprocess::SharedQueue, 173112507, 1)

#endif // !ATHENAINTERPROCESS_SHAREDQUEUE_H
