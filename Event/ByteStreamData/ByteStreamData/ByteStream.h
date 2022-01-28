/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMDATA_BYTESTREAM_H
#define BYTESTREAMDATA_BYTESTREAM_H
/** @class ByteStream
    @brief Transient data object, holding BS content of the event


    Initial version:  July 14, 2002
        typedef for Transient ByteStream data object
*/

#include "ByteStreamData/RawEvent.h"

class ByteStream {
public:
  /// default constructor
  ByteStream() : m_nWord(0), m_start(0) {}

  /// constructor with nWord
  ByteStream(int nWord) : m_nWord(nWord) {
    m_start = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[m_nWord];
  }

  ByteStream(const ByteStream&) = delete;
  ByteStream& operator=(const ByteStream&) = delete;

  /// destructor
  virtual ~ByteStream() {
    if (m_start != 0) {
      delete [] m_start; m_start = 0;
    }
  }

  /// get internal buffer
  OFFLINE_FRAGMENTS_NAMESPACE::DataType* getBuffer() { return m_start; }

private:
  /// number of words
  int m_nWord;
  /// representation
  OFFLINE_FRAGMENTS_NAMESPACE::DataType* m_start;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(ByteStream, 7001, 1)
#endif
