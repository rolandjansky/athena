//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef OFFLINE_EVENTSTORAGE_V5_DATABUFFER_H
#define OFFLINE_EVENTSTORAGE_V5_DATABUFFER_H

#include <string>
#include <v5_ESCompression.h>

namespace offline_EventStorage_v5 {
  
  class DataBuffer: public CompressionBuffer{
    
  public:
    DataBuffer();
    DataBuffer(const uint32_t& size);
    virtual ~DataBuffer();
     
    void realloc(const uint32_t& newsize);

    void grow(const uint32_t& newsize);
    
    void* handle();
    
    uint32_t buffersize() const;

  private:
    class implementation;
    implementation * rep;
    
    DataBuffer(const DataBuffer& aBuf);
    DataBuffer& operator= (const offline_EventStorage_v5::DataBuffer & aBuf);
  };
}

#endif
