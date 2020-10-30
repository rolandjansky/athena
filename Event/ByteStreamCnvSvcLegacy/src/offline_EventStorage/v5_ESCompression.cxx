/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#define ZLIB_CONST
#include "v5_ESCompression.h"
#include "zlib.h"
#include <string.h>
#include <boost/assert.hpp>
#include "ers/ers.h"

#include <string>

offline_EventStorage_v5::CompressionType
offline_EventStorage_v5::string_to_type(const std::string& type){
  
  if(type=="reserved"){
    return offline_EventStorage_v5::RESERVED;
  }else if(type=="unknown"){
    return offline_EventStorage_v5::UNKNOWN;
  }else if(type=="none"){
    return offline_EventStorage_v5::NONE;
  }else if(type=="zlib"){
    return offline_EventStorage_v5::ZLIB;
  }else{
    return offline_EventStorage_v5::UNKNOWN;
  }
}

std::string 
offline_EventStorage_v5::type_to_string(const CompressionType& type){
  
  switch(type){
  case NONE:
    return "none";
    break;
  case RESERVED:
    return "reserved";
    break;
  case UNKNOWN:
    return "unknown";
    break;
  case ZLIB:
    return "zlib";
    break;
  }

  return "unknown";
}


namespace offline_EventStorage_v5{
  std::string zmsg(const z_stream& strm){
    std::string zmsg("None");
    if(strm.msg){
      zmsg = strm.msg;
    }
    return zmsg;
  }
}

void offline_EventStorage_v5::zlibcompress(offline_EventStorage_v5::CompressionBuffer& compressed,
				uint32_t& compressedsize,
				const uint32_t& entries, 
				const struct iovec_const* iov,
				const uint32_t& totalsize,
				const uint32_t& level){

  if(compressed.buffersize() < totalsize*1.1){
    compressed.realloc(totalsize*1.1);
  }
  
  z_stream strm;
  
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  int ret = ::deflateInit(&strm, level); //level

  if(ret != Z_OK){
      offline_EventStorage_v5::ZlibFailed issue(ERS_HERE, "Zlib init failed", ret, "none");
    throw issue;
  }
 
  uint32_t compsize = 0;
  uint32_t available = compressed.buffersize();
  strm.avail_out = available;
  strm.next_out = reinterpret_cast<Bytef*>(compressed.handle());
  
  for (unsigned int i=0; i < entries; ++i) {

    strm.next_in = static_cast<const Bytef*>(iov[i].iov_base);
    strm.avail_in = iov[i].iov_len;
  
    int flush = ((i+1)==entries) ? Z_FINISH : Z_NO_FLUSH;

    while(true){
      ret = ::deflate(&strm, flush);
      
      if(ret == Z_STREAM_ERROR){
          offline_EventStorage_v5::ZlibFailed issue(ERS_HERE, "Zlib deflate failed",
				       ret, offline_EventStorage_v5::zmsg(strm));
	throw issue;
      }

      if(strm.avail_out){ 
	break;
      }else{
	compsize += available;
	
	//Double available buffer space
	compressed.grow(2*compressed.buffersize());
	available = compressed.buffersize()/2;
	strm.avail_out = available;
	strm.next_out = static_cast<Bytef*>(compressed.handle()) +available;
      }
    }
  }

  compsize += (available-strm.avail_out);
  ret = ::deflateEnd(&strm);

  if(ret != Z_OK){
      offline_EventStorage_v5::ZlibFailed issue(ERS_HERE, "Zlib deflateEnd failed",
				   ret, offline_EventStorage_v5::zmsg(strm));
    throw issue;
  }

  //Pad @4byte with zeros:
  uint32_t tobeadded = sizeof(uint32_t)-(compsize%sizeof(uint32_t));
  if(tobeadded>0 && tobeadded<sizeof(uint32_t)){
    
    if(tobeadded>(compressed.buffersize()-compsize)){
      //Grow buffer
      compressed.grow(compsize+tobeadded);
    }
    
    ::bzero(static_cast<Bytef*>(compressed.handle())+compsize,tobeadded);
    compsize += tobeadded;
  }
  
  compressedsize = compsize;
}

void
offline_EventStorage_v5::zlibdecompress(offline_EventStorage_v5::CompressionBuffer& decompressed,
			     uint32_t& decompressedsize,
			     const void * compressed,
			     const uint32_t& compsize){
  
  uLong bufsize;
  int ret;
    
  if(decompressed.buffersize() < compsize*3){
    decompressed.realloc(compsize*3);
  }

  while(true){
    
    bufsize = decompressed.buffersize();
    
    ret = ::uncompress(reinterpret_cast<Bytef*>(decompressed.handle()), 
		       &bufsize,
		       static_cast<const Bytef*>(compressed), 
		       compsize);

    if(ret == Z_OK){
      decompressedsize = bufsize;
      break;
    }else if(ret == Z_DATA_ERROR){
      ::memcpy(decompressed.handle(), compressed, compsize);
      decompressedsize = compsize;

      offline_EventStorage_v5::ZlibFailed
	issue(ERS_HERE, 
	      "Zlib decompression failed with Z_DATA_ERROR", 
	      ret, "none");
      throw issue;
      
    }else if(ret == Z_MEM_ERROR){
      //Not enough memory. Not much to do here!
      ERS_LOG("Zlib decompressed failed with MEM_ERROR. Code: " << ret);
      ERS_ASSERT(ret != Z_MEM_ERROR);
    }else if(ret == Z_BUF_ERROR){
      //We need a bigger buffer
      decompressed.realloc(decompressed.buffersize()*2);
    }
  }
}

