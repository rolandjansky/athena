/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_OFFLOADBUFFER_H
#define TRIGACCELEVENT_OFFLOADBUFFER_H

#include "DataExportBuffer.h"
#include <cstring>

namespace TrigAccel {
  
  class OffloadBuffer {
  public:
  OffloadBuffer(size_t size) : m_bufferSize(size) {
      m_rawBuffer = new unsigned char[m_bufferSize];
    }

    OffloadBuffer(const DATA_EXPORT_BUFFER* pB) {
      const size_t bufferOffset = 256;
      m_bufferSize = pB->m_size - bufferOffset;
      m_rawBuffer = new unsigned char[m_bufferSize];
      memcpy(m_rawBuffer, pB->m_buffer + bufferOffset, m_bufferSize);
    }

    virtual ~OffloadBuffer() {
      delete[] m_rawBuffer;
    }

    unsigned char* get() {
      return m_rawBuffer;
    }

    size_t m_bufferSize;
    unsigned char* m_rawBuffer;
  };
}

#endif
