/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERBASE_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERBASE_H

/** 
    @brief This class provides the core BLOB infrastructure
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details All data storage classes inherit from this class. It
    provides the infrastructure to store and retrieve units of
    conditions data associated with one channel or ADC ("data object")
    by implementing a common BLOB header. This header holds the
    following information:
    - The type of the object stored in the BLOB (see TileCalibType class for known types)
    - The version of the type, allows for schema evolution
    - The size of a data object in units of uint32_t (4 bytes)
    - The total number of data objects
    - The number of channels stored in the blob
    - The number of gains stored for each channel
    - The number of uint32_t used to store comment information (after the actual blob data)

    A data object contains all conditions data related to one channel
    or ADC (depending on data stored on channel or adc level), for
    example 5 floats. The conditions data units themselves must be
    build from objects of 32 bit length, i.e. available types are
    float, uint32_t and int32_t.

    The TileCalibDrawerBase class holds a reference to a coral::Blob
    containing conditions data for one TileCal drawer. A total number
    of 277 TileCalibDrawer objects are stored in each COOL folder to
    hold data for the whole detector. Each TileCalibDrawer is stored
    in its own COOL channel, numbered from 0 to 276. Data related to
    the 4*64 TileCal modules (LBA01, LBA02, ... , LBC01, ... , EBA01,
    ... , EBC63, EBC64) are stored in the channel range [20,275]. The
    channel range [0,19] holds default drawers: [0-3] are global
    detector defaults, [4-7] for LBA, [8-11] for LBC, [12-15] for EBA
    and [16-19] for EBC. 

    Channel number 1000 contains TileCalibDrawerCmt comment classes,
    common for all TileCalibDrawers of the same IOV which do not have
    their own comment fields initialized. Comment fields are not
    initialized if both the author and the comment field are empty in
    initialization.
*/


#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include "CoralBase/Blob.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileCalibBlobObjs/TileCalibType.h"


class TileCalibDrawerBase{
 public:
  /** @brief Dtor */
  virtual ~TileCalibDrawerBase();

  /** @brief Copy Ctor 
      @details In case the copy constructor is called, the underlying coral::Blob
      is copied and the new TileCalibDrawer is the owner of the new coral::Blob. I.e. 
      it will be deleted when the TileCalibDrawer Dtor is called.
      @param other The TileCalibDrawer to copy */
  TileCalibDrawerBase(const TileCalibDrawerBase& other);

  /** @brief Assignment operator */
  TileCalibDrawerBase& operator=(const TileCalibDrawerBase& other);

  /** @brief Initialzes Blob with content of other Blob.
      @param other TileCalibDrawer used for initialization
  */
  virtual void clone(const TileCalibDrawerBase& other);
  
  /** @brief Returns TileCalibDrawer::BASE. */
  virtual uint16_t getType() const {return TileCalibType::BASE;}

  /** @brief Prints the BLOB header summary information to std::cout. */
  virtual void dump() const { dumpHeader(std::cout); } 
  /** @brief Prints the BLOB header summary information.
      @param stm The output stream to use */
  virtual void dump(std::ostream& stm) const {dumpHeader(stm);} 

  //==================================================================
  //== Accessor methods for the header
  //==================================================================
  /** @brief Returns the BLOB size in units of bytes. */
  long     getBlobSize() const { return m_blobSize32 * sizeof(uint32_t); } 
  /** @brief Returns the BLOB size in units of uint32_t. */
  long     getBlobSize32() const { return m_blobSize32; } 
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
  uint16_t getNChans() const;
  /** @brief Returns the number of gains stored for each channel. */
  uint16_t getNGains() const;
  /** @brief Returns the space occupied by the comment fields in units of uint32_t. */
  uint32_t getCommentSizeUint32() const;
  /** @brief Returns the space occupied by the comment fields in units of chars. */
  uint32_t getCommentSizeChar() const {return getCommentSizeUint32()*sizeof(uint32_t);}
  
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
  
  /** @brief The header size in units of uint32_t. */
  static const unsigned int m_hdrSize32 = 5;
  
 protected:
  /** @brief Ctor for const blob. */
  TileCalibDrawerBase(const coral::Blob& blob);

  /** @brief Ctor for non-const blob. */
  TileCalibDrawerBase(coral::Blob& blob);

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
		      uint16_t nChans,
		      uint16_t nGains,
		      const std::string& author="",
		      const std::string& comment="",
		      uint64_t timeStamp=0);

  /** @brief Prints the BLOB header summary information.
      @param stm output stream to use*/
  void dumpHeader(std::ostream& stm) const; 

 private:
  /** @brief Non-const reference to the BLOB.
             (Only present if we were created with a non-const blob.) */
  coral::Blob*  m_blob_nc;
  /** @brief Const reference to the BLOB (always there) */
  const coral::Blob*  m_blob;
  /** @brief Cache blob starting address as uint_32t* */
  const uint32_t*  m_blobStart32;
  /** @brief Cache blob starting address as uint_16t* */
  const uint16_t*  m_blobStart16;
  /** @brief Cache blob size in units of uint32_t */
  uint64_t      m_blobSize32;
    /** @brief Is this TileCalibDrawer owner of the BLOB */
  bool          m_isBlobOwner;
};


//
//_________________________________________________________
__attribute__((always_inline)) inline uint16_t 
TileCalibDrawerBase::getObjType() const 
{
  return m_blobStart16[0];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint16_t 
TileCalibDrawerBase::getObjVersion() const 
{
  return m_blobStart16[1];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint32_t 
TileCalibDrawerBase::getObjSizeUint32() const 
{
  return m_blobStart32[1];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint32_t 
TileCalibDrawerBase::getNObjs() const 
{
  return m_blobStart32[2];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint16_t 
TileCalibDrawerBase::getNChans() const 
{
  return m_blobStart16[6];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint16_t 
TileCalibDrawerBase::getNGains() const 
{
  return m_blobStart16[7];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint32_t 
TileCalibDrawerBase::getCommentSizeUint32() const 
{
  return m_blobStart32[4];
}

//
//_________________________________________________________
__attribute__((always_inline)) inline const void* 
TileCalibDrawerBase::getAddress(unsigned int iEle) const
{
  if(iEle>=getNObjs()){ 
    throw TileCalib::IndexOutOfRange("TileCalibDrawerBase::getAddress", iEle, getNObjs());
  }
  return static_cast<const void*>( m_blobStart32 + m_hdrSize32 + getObjSizeUint32()*iEle );
}

//
//_________________________________________________________
__attribute__((always_inline)) inline void* 
TileCalibDrawerBase::getAddress(unsigned int iEle)
{
  if(iEle>=getNObjs()){ 
    throw TileCalib::IndexOutOfRange("TileCalibDrawerBase::getAddress", iEle, getNObjs());
  }
  uint32_t* blobStart32 = static_cast<uint32_t*>(m_blob_nc->startingAddress());
  return static_cast<void*>( blobStart32 + m_hdrSize32 + getObjSizeUint32()*iEle );
}

//
//_________________________________________________________
__attribute__((always_inline)) inline uint64_t 
TileCalibDrawerBase::getTimeStamp() const 
{
  if(!getCommentSizeUint32()) return 0;
  return *(reinterpret_cast<const uint64_t*>(m_blobStart32 + m_hdrSize32 +
                                             getNObjs()*getObjSizeUint32()));
}

#endif
