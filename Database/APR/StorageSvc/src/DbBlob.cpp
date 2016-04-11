/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbBlob.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbBlob class definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#define POOL_STORAGESVC_DBBLOB_CPP

// include files
#include <StorageSvc/DbBlob.h>
#include <cstdlib>
using namespace std;
using namespace pool;

#ifdef USE_STREAM_ANALYSER
  #define STREAM_ANALYSE(data, len)   if ( 0 != m_analyzer ) m_analyzer(&data, len, typeid(data))
#else
  #define STREAM_ANALYSE(data, len)
#endif

/// Standard destructor
DbBlob::~DbBlob()             {
  ::free(m_buffer);
}

/// Adopt an external memory buffer
void DbBlob::adopt(char* memory, int len)    {
  if ( 0 != memory )  {
    ::free( m_buffer );
    m_buffer = memory;
    m_length = len;
    m_pointer = 0;
  }
}
/// Reserve buffer space; Default: 16 k buffer size
void DbBlob::reserve(int len)   {
  if ( len > m_length )   {
    m_length = (len < 16384) ? 16384 : len; 
    m_buffer = (char*)::realloc (m_buffer,m_length);
  }
}

/// Write string to output stream
DbBlob& DbBlob::writeBytes  (const char* str, int len)    {
  extend( m_pointer+len+4 );
  *this << len;
  ::memcpy(data()+buffPointer(), str, len);
  m_pointer += len;
  return *this;
}

/// Streamer to read strings in (char*) format
DbBlob& DbBlob::operator>>(char* data)    {
  int i, len;
  *this >> len;
  for ( i = 0, data[0]=0; i < len; i++ )    {
    *data = m_buffer[m_pointer++];
  }
  return *this;
}
/// Streamer to write strings in (char*) format
DbBlob& DbBlob::operator<<(const char *data)     {
  const char* ptr = 0 == data ? "" : data;
  int len = strlen(ptr)+1;
  if ( 0 == m_analyzer ) 
    writeBytes(ptr, len);
  else  {
    STREAM_ANALYSE(data, len);
  }
  return *this;
}

/// Streamer to read strings in (string) format
DbBlob& DbBlob::operator>>(string& data)   {
  int i, len;
  *this >> len;
  for ( i = 0, data = ""; i < len; i++ )    {
    data.append( 1, m_buffer[m_pointer++] );
  }
  return *this;
}
/// Streamer to write strings in (string) format
DbBlob& DbBlob::operator<<(const string& data)   {
  if ( 0 == m_analyzer)   {
    const char* ptr = data.c_str();
    int len = data.length();
    writeBytes(ptr, len);
  }
  else    {
    STREAM_ANALYSE(data, sizeof(data));
  }
  return *this;
}
#undef STREAM_ANALYSE

