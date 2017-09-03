/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/DbBlob.h,v 1.14 2010/08/19 14:02:41 trentad Exp $
#ifndef POOL_STORAGESVC_DBBLOB_H
#define POOL_STORAGESVC_DBBLOB_H 1

// STL include files
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <typeinfo>

#if !(defined(WIN32) || defined(_WIN32))
#include <unistd.h>
#define _swab(source, target, radix) swab(source, target, radix)
#endif  /* WIN32 */


/*  
 *   POOL namespace declaration
 */
namespace pool    {

  /** @class DbBlob DbBlob.h pool/DbBlob.h
    *
    * Description:
    *
    * The stream buffer is a small object collecting object data.
    * The basic idea behind the DbBlob is generic object conversion.
    * The DbBlob acts as a byte stream (hence inheriting from a
    * std::string) and stores any information streamed to the buffer.
    * Since the information must be represented in a generic way
    * on the fly byte swapping is performed. However, not only primitive
    * data can be stored in the buffer, but also pointers to DataObjects
    * (symbolic links) and pointers to contained objects. Automatically during
    * serialization the persistent references to the corresponding objects
    * and containers must be stored. These objects are accessible 
    * from the DbBlob object.
    *
    * @author   M.Frank
    * @version  1.0
    */
  class DbBlob  {
  public:
    /// Streamer mode
    enum Mode       {UNINITIALIZED, READING, WRITING};
    /// Data Sawp actions
    enum SwapAction {SINGLE_BYTE, SWAP, NOSWAP};
    /// Link state defintions
    enum    State   {INVALID=-1, VALID };
    /// Definition of the buffer analyzer
    typedef void (*Analyzer)(const void* data, int siz, const std::type_info& type);

  protected:
    /// Boolean indicating wether the stream is in read or write mode
    Mode     m_mode;
    /// Current buffer pointer
    int      m_pointer;
    /// Total buffer length
    int      m_length;
    /// Pointer to heap buffer
    char*    m_buffer;
    /// Flag indicating swapping
    bool     m_swapEnabled;
    /// Hook function for analysis of data to the stream
    Analyzer m_analyzer;

    /// Check for byte swapping
    SwapAction swapBuffer(int siz)  const;

  public:
    /// Standard constructor
    DbBlob(bool do_swap=true) : 
      m_mode(UNINITIALIZED), 
      m_pointer(0), 
      m_length(0), 
      m_buffer(0),
      m_swapEnabled(do_swap)      {      m_analyzer = 0;          }
    /// Standard destructor
    virtual ~DbBlob();

    DbBlob (const DbBlob&) = delete;
    DbBlob& operator= (const DbBlob&) = delete;

    /// Read access to data buffer
    const char* data()    const   {      return m_buffer;         }
    /// write access to data buffer
    char* data()                  {      return m_buffer;         }
    /// Reset the buffer
    void erase()                  {      m_pointer = 0;           }
    /// Total buffer size
    int size ()  const            {      return m_length;         }
    /// Get stream buffer state
    bool isReading()    const     {      return m_mode == READING;}

    /// Get stream buffer state
    bool isWriting()    const     {      return m_mode == WRITING;}
    /// Retrieve current buffer pointer
    int buffPointer() const       {      return m_pointer;        }
    /// Retrieve current buffer pointer
    void setBuffPointer(int ptr)  {      m_pointer = ptr;         }
    /// Enable user analysis function
    void setAnalyzer(Analyzer fun=0) {   m_analyzer = fun;        }
    /// Swap buffers: int, long, short, float and double
    inline void swapToBuffer(const void* source, int siz);
    /// Swap buffers: int, long, short, float and double
    inline void swapFromBuffer(void* target, int siz);
    /// Adopt an external memory buffer
    void adopt(char* memory, int len);
    /// Reserve buffer space; Default: 16 k buffer size
    void reserve(int len);
    /// Extend the buffer
    void extend(int len)    {
      if ( len + m_pointer > m_length )    {
        // We have to be a bit generous here in order not to run too often
        // into ::realloc().
        int new_len = (m_length < 16384) ? 16384 : 2*m_length;
        if ( m_length < len ) new_len += len;
        reserve(new_len);
      }
    }
    /// Set mode of the stream and allocate buffer
    void setMode(Mode m)  {
      m_mode = m;
      m_pointer = 0;
    }
    /// Write string to output stream
    DbBlob& writeBytes  (const char* str, int len);

  #ifdef USE_STREAM_ANALYSER
    #define STREAM_ANALYSE(data, len)   if ( 0 != m_analyzer ) m_analyzer(&data, len, typeid(data))
  #else
    #define STREAM_ANALYSE(data, len)
  #endif

    ///  Output Streamer                              
    DbBlob& operator<<(long long int   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(long long int & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(int   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(int & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(unsigned int   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(unsigned int & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(long   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(long & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(unsigned long   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(unsigned long & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(short   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(short & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(unsigned short   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(unsigned short & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(char   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(char & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(unsigned char   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(unsigned char & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(float   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(float & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    ///  Output Streamer                              
    DbBlob& operator<<(double   data)     {                       
      swapToBuffer(&data, sizeof(data));                              
      STREAM_ANALYSE(data, sizeof(data));                             
      return *this;                                                   
    }                                                                 
    ///  Input Streamer                                   
    DbBlob& operator>>(double & data)     {                       
      swapFromBuffer(&data, sizeof(data));                            
      return *this;                                                   
    }
    /// Streamer to read strings in (char*) format
    DbBlob& operator>>(char* data);
    /// Streamer to write strings in (char*) format
    DbBlob& operator<<(const char *data);
    /// Streamer to read strings in (std::string) format
    DbBlob& operator>>(std::string& data);
    /// Streamer to write strings in (std::string) format
    DbBlob& operator<<(const std::string& data);
  };
  #undef STREAM_ANALYSE

  /// Check for byte swapping
  inline DbBlob::SwapAction DbBlob::swapBuffer(int siz)  const    {
    switch(siz)   {
    case 1:  
      return SINGLE_BYTE;
    default:
  #if defined(__alpha) && !defined(__VMS)
      return NOSWAP;
  #elif defined(__sun) && defined(__SVR4) && defined(__i386)
      return NOSWAP;
  #elif defined(__linux) && !defined(__powerpc)
      return NOSWAP;
  #elif defined(BORLAND) || defined(_WIN32) || defined(WIN32)
      return NOSWAP;
  #elif defined(__APPLE__) && defined(__i386__)
      return NOSWAP;
  #elif defined(__APPLE__)
      return SWAP;
  #else
      return m_swapEnabled ? SWAP : NOSWAP;
  #endif
    }
  }

  /// Swap bytes from a source buffer to the stream buffer with arbitray size
  inline void DbBlob::swapToBuffer(const void* source, int siz)   {
    char buff[8], *tar, *src = (char*)source;
    extend (m_pointer+siz);
    tar = (char*)m_buffer+m_pointer;
    switch ( swapBuffer(siz) )   {
    case SINGLE_BYTE:
      *tar = *src;
      break;
    case SWAP:
#ifdef __APPLE__
      for(int i = 0,j = siz-1;i<siz;i++,j--) tar[j] = src[i];
#else
      ::_swab (src, buff, siz);
#endif
      src = buff;
      // Fall through.
    case NOSWAP:
      memcpy(tar, src, siz);
      break;
    }
    m_pointer += siz;
  }

  /// Swap bytes from the stream buffer to target buffer with arbitray size
  inline void DbBlob::swapFromBuffer(void* target, int siz)   {
    char* tar = (char*)target;
    char* src = (char*)m_buffer+m_pointer;
    switch ( swapBuffer(siz) )   {
    case SINGLE_BYTE:
      *tar = *src;
      break;
    case SWAP:
#ifdef __APPLE__
      for(int i = 0,j = siz-1;i<siz;i++,j--) tar[j] = src[i];
#else
      ::_swab (src, tar, siz);
#endif
      break;
    case NOSWAP:
      ::memcpy(tar, src, siz);
      break;
    }
    m_pointer += siz;
  }

  // Output serialize a vector of items
  template <class T> inline 
  DbBlob& operator << (DbBlob& s, const std::vector<T>& v)  {
    int len = (int)v.size();
    s << len;
    for ( typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); i++ )  {
      s << (*i);
    }
    return s;
  }

  // Input serialize a vector of items
  template <class T> inline
  DbBlob& operator >> (DbBlob& s, std::vector<T>& v)  {
    int i, len;
    s >> len;
    v.clear();
    for ( i = 0; i < len; i++ )  {
      T    temp;
      s >> temp;
      v.push_back(temp);
    }
    return s;
  }

  // Output serialize a list of items
  template <class T> inline 
  DbBlob& operator << (DbBlob& s, const std::list<T>& l)  {
    int len = (int)l.size();
    s << len;
    for ( typename std::list<T>::const_iterator i = l.begin(); i != l.end(); i++ )  {
      s << (*i);
    }
    return s;
  }

  // Input serialize a list of items
  template <class T> inline
  DbBlob& operator >> (DbBlob& s, std::list<T>& l)  {
    int i, len;
    s >> len;
    l.clear();
    for ( i = 0; i < len; i++ )  {
      T    temp;
      s >> temp;
      l.push_back(temp);
    }
    return s;
  }

}       // End namespace pool
#endif  // POOL_STORAGESVC_DBBLOB_H
