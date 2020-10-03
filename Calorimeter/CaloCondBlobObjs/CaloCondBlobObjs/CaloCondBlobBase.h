/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_CALOCONDBLOBBASE_H
#define CALOCONDBLOBOBJS_CALOCONDBLOBBASE_H

/** 
    @brief This class provides the core BLOB infrastructure
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details All data storage classes inherit from this class. It
    provides the infrastructure to store and retrieve units of
    conditions data associated with one channel/gain ("data object")
    by implementing a common BLOB header. This header holds the
    following information:
    - The type of the object stored in the BLOB (see CaloCondType class for known types)
    - The version of the type (allows for schema evolution)
    - The size of a data object in units of uint32_t (4 bytes)
    - The total number of data objects
    - The number of channels stored in the blob
    - The number of gains stored for each channel
    - The number of uint32_t used to store comment information (after the actual blob data)

    A data object contains all conditions data related to one channel/gain 
    (depending on data stored on channel or adc level), for
    example 5 floats. The conditions data units themselves must be
    build from objects of 32 bit length, i.e. available types are
    float, uint32_t and int32_t.
*/


#include <stdint.h>
#include <vector>
#include <string>
#include <ostream>
#include <iostream>
#include "CoralBase/Blob.h"
#include "CaloCondBlobObjs/Exception.h"
#include "CaloCondBlobObjs/CaloCondType.h"

class CaloCondBlobBase{
 public:
  /** @brief Dtor */
  virtual ~CaloCondBlobBase();
  /** @brief Copy Ctor */
  CaloCondBlobBase(const CaloCondBlobBase& other);
  /** @brief Assignment operator */
  CaloCondBlobBase& operator=(const CaloCondBlobBase& other);
  
  /** @brief Returns CaloCondDrawer::BASE. */
  virtual uint16_t getType() const {return CaloCondType::BASE;}

  /** @brief Prints the BLOB header summary information to std::cout. */
  virtual void dump() const { dumpHeader(std::cout); } 
  /** @brief Prints the BLOB header summary information.
      @param stm The output stream to use */
  virtual void dump(std::ostream& stm) const {dumpHeader(stm);} 


  //==================================================================
  //== Accessor methods for the header
  //==================================================================
  /** @brief Returns the BLOB size. */
  long     getBlobSize() const {return m_blob->size(); } 
  /** @brief Returns the BLOB object type. */
  uint16_t getObjType() const;
  /** @brief Returns the BLOB object version. */
  uint16_t getObjVersion() const;
  /** @brief Returns the size of a data object in units of uint32_t. */ 
  uint32_t getObjSizeUint32() const;
  /** @brief Returns the size of a data object in units of bytes. */ 
  uint32_t getObjSizeByte() const {return getObjSizeUint32()*sizeof(uint32_t);}
  /** @brief Returns the number of data objects stored int the BLOB. */
  uint32_t getNObjs() const;
  /** @brief Returns the number of channels stored in the BLOB. */
  uint32_t getNChans() const;
  /** @brief Returns the number of gains stored for each channel. */
  uint32_t getNGains() const;
  /** @brief Returns the space occupied by the comment fields in units of uint32_t. */
  uint32_t getCommentSizeUint32() const;
  /** @brief Returns the space occupied by the comment fields in units of chars. */
  uint32_t getCommentSizeChar() const {return getCommentSizeUint32()*sizeof(uint32_t);}
  /** @brief Returns the size of the header in units of uint32_t. */
  unsigned int getHdrSize() const {return 5;}
  
  //==================================================================
  //== Accessor methods for the comment fields
  //==================================================================
  /** @brief Returns the comment author */
  std::string getAuthor() const;
  /** @brief Returns the actual comment */
  std::string getComment() const;
  /** @brief Returns the unix timestamp of the comment (seconds since 1.1.1970) */
  uint64_t    getTimeStamp() const;
  /** @brief Returns the date of the comment as string (derived from timestamp) */
  std::string getDate() const; 
  /** @brief Returns a formated string build from all comment fields */
  std::string getFullComment() const;
 

  //==================================================================
  //== Blob field access
  //==================================================================
  /** @brief Returns start address of iEle-th basic unit. 
      @param iEle sequential basic unit number */
  const void* getAddress(unsigned int iEle) const;
  void* getAddress(unsigned int iEle);
  
  
 protected:
  /** @brief Ctor for const blob. */
  CaloCondBlobBase(const coral::Blob& blob);

  /** @brief Ctor for non-const blob. */
  CaloCondBlobBase(coral::Blob& blob);

  /** @brief (re-)creation of the referenced BLOB object.
      @param objType Object type
      @param objVersion Object version 
      @param objSizeUint32 Size of a data object (in uint32_t)
      @param nObjs Total number of data objects
      @param nChans number of stored channels
      @param nGains number of stored gains
      @param author comment author
      @param comment actual comment
      @param timeStamp unix time stamp (if 0, current time is used) */
  uint32_t createBlob(uint16_t objType,          
		      uint16_t objVersion,
		      uint32_t objSizeUint32, 
		      uint32_t nObjs,
		      uint32_t nChans,
		      uint16_t nGains,
		      const std::string& author="",
		      const std::string& comment="",
		      uint64_t timeStamp=0);

  /** @brief Prints the BLOB header summary information.
      @param stm output stream to use*/
  void dumpHeader(std::ostream& stm) const; 

 private:
  /** @brief Internal ctor. */
  CaloCondBlobBase(coral::Blob* blob_nc, const coral::Blob* blob);

  uint32_t packGainAndNchans(const uint32_t gain, const uint32_t nChans);
  uint32_t unpacknGains(const uint32_t gainAndnChans);
  uint32_t unpacknChans(const uint32_t gainAndnChans);

  /** @brief Returns the BLOB start address as uint32_t pointer. */
  const uint32_t* getBlobStart() const;
  /** @brief Returns the BLOB start address as uint32_t pointer. */
  uint32_t* getBlobStart();

  /** @brief Non-const reference to the BLOB
      (Only present if we were created with a non-const blob. */
  coral::Blob*  m_blob_nc;

  /** @brief Const reference to the BLOB (always there) */
  const coral::Blob*  m_blob;

  /** Do I own the BLOB? */
  bool          m_isBlobOwner;

  /** Chache of some frequently-used numbers */
  unsigned m_sizeOfObj;
  unsigned m_nChans;
  unsigned m_nGains;
};


//
//_________________________________________________________
inline const uint32_t*
CaloCondBlobBase::getBlobStart() const
{
  return static_cast<const uint32_t*>(m_blob->startingAddress());
}

//
//_________________________________________________________
inline uint32_t*
CaloCondBlobBase::getBlobStart() 
{
  return static_cast<uint32_t*>(m_blob_nc->startingAddress());
}

//
//_________________________________________________________
inline uint16_t 
CaloCondBlobBase::getObjType() const 
{
  if(uint64_t(m_blob->size())<getHdrSize()*sizeof(uint32_t)) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::getObjType");
  return static_cast<const uint16_t*>(m_blob->startingAddress())[0];
}

//
//_________________________________________________________
inline uint16_t 
CaloCondBlobBase::getObjVersion() const 
{
  if(uint64_t(m_blob->size())<getHdrSize()*sizeof(uint32_t)) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::getObjVersion");
  return static_cast<const uint16_t*>(m_blob->startingAddress())[1];
}

//
//_________________________________________________________
inline uint32_t 
CaloCondBlobBase::getObjSizeUint32() const 
{
  return m_sizeOfObj;
}

//
//_________________________________________________________
inline uint32_t 
CaloCondBlobBase::getNObjs() const 
{
  if(uint64_t(m_blob->size())<getHdrSize()*sizeof(uint32_t)) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::getNObjs");
  return static_cast<const uint32_t*>(m_blob->startingAddress())[2];
}

//
//_________________________________________________________
inline uint32_t 
CaloCondBlobBase::getNChans() const 
{
  return m_nChans;
}

//
//_________________________________________________________
inline uint32_t 
CaloCondBlobBase::getNGains() const 
{
  return m_nGains;


}

//
//_________________________________________________________
inline uint32_t 
CaloCondBlobBase::getCommentSizeUint32() const 
{
  if(uint64_t(m_blob->size())<getHdrSize()*sizeof(uint32_t)) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::getCommentSizeUint32");
  return static_cast<const uint32_t*>(m_blob->startingAddress())[4];
}

//
//_________________________________________________________
inline const void* 
CaloCondBlobBase::getAddress(unsigned int iEle) const
{
  if(iEle>=getNObjs()){ 
    throw CaloCond::IndexOutOfRange("CaloCondBlobBase::getAddress", iEle, getNObjs());
  }
  return static_cast<const void*>
    ( getBlobStart() + getHdrSize() + getObjSizeUint32()*iEle );
}

//
//_________________________________________________________
inline void* 
CaloCondBlobBase::getAddress(unsigned int iEle) 
{
  if(iEle>=getNObjs()){ 
    throw CaloCond::IndexOutOfRange("CaloCondBlobBase::getAddress", iEle, getNObjs());
  }
  return static_cast<void*>
    ( getBlobStart() + getHdrSize() + getObjSizeUint32()*iEle );
}

//
//_________________________________________________________
inline uint64_t 
CaloCondBlobBase::getTimeStamp() const 
{
  if(!getCommentSizeUint32()) return 0;
  return *(reinterpret_cast<const uint64_t*>(getBlobStart()+getHdrSize() +
                                             getNObjs()*getObjSizeUint32()));
}

#endif
