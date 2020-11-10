/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "v5_DataBuffer.h"

#include <string.h>

class offline_EventStorage_v5::DataBuffer::implementation{

public:
  
  implementation():
    m_buffer(0),
    m_buffersize(0){}

  implementation(const uint32_t& size):
    m_buffer(0),
    m_buffersize(0){
    m_buffer = new char[size];
    m_buffersize = size;
  }

  implementation (const implementation&) = delete;
  implementation& operator= (const implementation&) = delete;
 
  ~implementation()
  { delete[] m_buffer; }

  
  void realloc(const uint32_t& newsize)
  {
    delete[] m_buffer;
    m_buffer = new char[newsize];
    m_buffersize = newsize;
  }

  void grow(const uint32_t& newsize)
  {
    if(newsize <=  m_buffersize)
      return;
    
    char * new_buffer = new char[newsize];
    ::memcpy(new_buffer, m_buffer, m_buffersize);
    delete[] m_buffer;
    
    m_buffer =  new_buffer;
    m_buffersize = newsize;
  }

  void * handle()
  { return m_buffer; }

  uint32_t buffersize()
  { return m_buffersize; }

private:
  char* m_buffer;
  uint32_t m_buffersize;

};

offline_EventStorage_v5::DataBuffer::DataBuffer():
  m_rep(new implementation())
{}

offline_EventStorage_v5::DataBuffer::DataBuffer(const uint32_t& size):
  m_rep(new implementation(size))
{}

offline_EventStorage_v5::DataBuffer::~DataBuffer()
{ delete m_rep; }
     
void offline_EventStorage_v5::DataBuffer::realloc(const uint32_t& newsize)
{ return m_rep->realloc(newsize); }

void offline_EventStorage_v5::DataBuffer::grow(const uint32_t& newsize)
{ return m_rep->grow(newsize); }

void* offline_EventStorage_v5::DataBuffer::handle()
{ return m_rep->handle(); }
    
uint32_t offline_EventStorage_v5::DataBuffer::buffersize() const
{ return m_rep->buffersize(); }
