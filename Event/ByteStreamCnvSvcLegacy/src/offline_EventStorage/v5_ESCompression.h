//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef OFFLINE_EVENTSTORAGE_V5_ESCOMPRESSION_H
#define OFFLINE_EVENTSTORAGE_V5_ESCOMPRESSION_H

#include <string>
#include <sys/uio.h>
#include "ers/ers.h"
#include <string>

#include "ByteStreamCnvSvcLegacy/offline_EventStorage/v5_EventStorageRecords.h"

ERS_DECLARE_ISSUE(offline_EventStorage_v5,
		  CompressionIssue,
		  ERS_EMPTY, ERS_EMPTY)

ERS_DECLARE_ISSUE_BASE(offline_EventStorage_v5,
		       ZlibFailed, 
		       offline_EventStorage_v5::CompressionIssue,
		       "" << message << " - Zlib return code: " 
		       << return_code << " Zlib message: " << zlib_message,
		       ERS_EMPTY,
		       ((const char *) message )
		       ((int)return_code)
		       ((std::string) zlib_message)
		       )


namespace offline_EventStorage_v5 {
      
  static const std::string compressiontag("Compression");


  /**
   *  Abstract interface of a *CONTIGUOUS* buffer used for
   *  event compression and decompression
   */
  class CompressionBuffer{
    
  public:
    using iovec_const = offline_EventStorage_v5::iovec_const;

    virtual ~CompressionBuffer() {};

    /**
     *  Returns a pointer to the underlying buffer
     *
     *  @return Pointer to the buffer
     */
    virtual void * handle() = 0;
    
    /**
     *  Resizes the underlying buffer with preserving the 
     *  existing content.
     *
     *  @param newsize The expected updated buffer size in 
     *                 bytes
     */
    virtual void realloc(const uint32_t& newsize) = 0;

    /**
     * Resize the underlying buffer *preserving*
     * the existing content.
     *
     * @param newsize The expected updated buffer size in 
     *                 bytes
     */
    virtual void grow(const uint32_t& newsize) = 0;

    /**
     * Returns size (in byte) of the underlying buffer
     *
     * @return Buffer size in bytes
     */
    virtual uint32_t buffersize() const = 0;
  };
  
  
  CompressionType string_to_type(const std::string& type);
  std::string type_to_string(const CompressionType& type);


  /**
   * Compress a block of data
   *
   * @param compressed A CompressionBuffer object that
   *                   will be used to store the compressed
   *                   data
   * @param compressedsize Will be updated with the compressed
   *                       data size (in bytes)
   * @param entries Input io vector length
   * @param iov Input io vector
   * @param totalsize Total size of data in the io vector (in bytes)
   * @param level Compression level (1<-->9)
   *
   * In case of failure a EventStorage::CompressionIssue exception 
   * (or subclass) is thrown
   */

  void zlibcompress(CompressionBuffer& compressed,
		    uint32_t& compressedsize,
		    const uint32_t& entries, 
		    const struct iovec_const* iov,
		    const uint32_t& totalsize,
		    const uint32_t& level);
  

  /**
   * Decompress a zlib compressed block of data
   *
   * @param decompressed A CompressionBuffer object that
   *                     will be used to store the decompressed
   *                     data
   * @param decompressedsize Will be updated with the decompressed
   *                         data size (in bytes)
   * @param compressed Pointer to the compressed data block
   * @param compsize Size of the compressed data block (in bytes)
   *
   * In case of failure a EventStorage::CompressionIssue exception 
   * (or subclass) is thrown. In the latter case the compressed data 
   * block is simply copied into the decompressed buffer.
   */
  void zlibdecompress(CompressionBuffer& decompressed,
		      uint32_t& decompressedsize,
		      const void * compressed,
		      const uint32_t& compsize);

}

#endif
