/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileCalibDrawerBase.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include <time.h>
#include <algorithm>

//
//_____________________________________________________________
TileCalibDrawerBase::TileCalibDrawerBase(const coral::Blob& blob) : 
  m_blob_nc(nullptr),
  m_blob(&blob),
  m_blobStart32(static_cast<const uint32_t*>(m_blob->startingAddress())),
  m_blobStart16(static_cast<const uint16_t*>(m_blob->startingAddress())),
  m_blobSize32(m_blob->size()/sizeof(uint32_t)),
  m_isBlobOwner(false)
{
}

//
//_____________________________________________________________
TileCalibDrawerBase::TileCalibDrawerBase(coral::Blob& blob) : 
  m_blob_nc(&blob),
  m_blob(&blob),
  m_blobStart32(static_cast<const uint32_t*>(m_blob->startingAddress())),
  m_blobStart16(static_cast<const uint16_t*>(m_blob->startingAddress())),
  m_blobSize32(m_blob->size()/sizeof(uint32_t)),
  m_isBlobOwner(false)
{
}

//
//_____________________________________________________________
TileCalibDrawerBase::~TileCalibDrawerBase()
{
  //=== delete the coral::Blob if owner
  if(m_isBlobOwner) {delete m_blob;}
}

//
//_____________________________________________________________
TileCalibDrawerBase::TileCalibDrawerBase(const TileCalibDrawerBase& other) : 
  m_blob_nc(new coral::Blob(*other.m_blob)),
  m_blob(m_blob_nc),
  m_blobStart32(static_cast<const uint32_t*>(m_blob->startingAddress())),
  m_blobStart16(static_cast<const uint16_t*>(m_blob->startingAddress())),
  m_blobSize32(m_blob->size()/sizeof(uint32_t)),
  m_isBlobOwner(true)
{
}

//
//_____________________________________________________________
TileCalibDrawerBase& 
TileCalibDrawerBase::operator=(const TileCalibDrawerBase& other)
{
  //=== catch self-assignment
  if(&other == this) {return *this;}
  if (m_isBlobOwner) delete m_blob;
  m_blob_nc     = other.m_blob_nc;
  m_blob        = other.m_blob;
  m_blobStart32 = other.m_blobStart32;
  m_blobStart16 = other.m_blobStart16;
  m_blobSize32  = other.m_blobSize32;
  m_isBlobOwner = other.m_isBlobOwner; 
  return *this;
}

//
//_____________________________________________________________
void 
TileCalibDrawerBase::clone(const TileCalibDrawerBase& other)
{
  //=== copy content of other blob
  *m_blob_nc = *other.m_blob;
  //=== and reset cached attributes
  m_blobStart32 = static_cast<const uint32_t*>(m_blob->startingAddress());
  m_blobStart16 = static_cast<const uint16_t*>(m_blob->startingAddress());
  m_blobSize32  = m_blob->size()/sizeof(uint32_t);
}

//
//_____________________________________________________________
uint32_t
TileCalibDrawerBase::createBlob(uint16_t objType,          
				uint16_t objVersion,
				uint32_t objSizeUint32, 
				uint32_t nObjs,
				uint16_t nChans,
				uint16_t nGains,
				const std::string& author,
				const std::string& comment,
				uint64_t timeStamp)
{
  //=== blob data length including header in bytes
  uint32_t dataSizeByte = (m_hdrSize32+objSizeUint32*nObjs) * sizeof(uint32_t);
  
  //=== calculate comment length, including two ASCII NULLs to end text fields
  uint32_t commentSizeChar(0);
  if(author.size() || comment.size() || timeStamp){
    commentSizeChar += author.size()+comment.size()+sizeof(uint64_t) + 2;
    //=== force comment length to end on 4 byte boundary
    commentSizeChar += (commentSizeChar % sizeof(uint32_t)) ? 
      (sizeof(uint32_t)-(commentSizeChar % sizeof(uint32_t))) : 0;
  }
  
  //=== create blob
  uint32_t blobSizeInBytes = dataSizeByte+commentSizeChar;
  m_blob_nc->resize(blobSizeInBytes);
  uint32_t* blobStart32 = static_cast<uint32_t*>(m_blob_nc->startingAddress());
  uint16_t* blobStart16 = static_cast<uint16_t*>(m_blob_nc->startingAddress());
  m_blobSize32  = m_blob_nc->size()/sizeof(uint32_t);

  m_blobStart32 = blobStart32;
  m_blobStart16 = blobStart16;

  //=== fill header
  blobStart16[0] = objType;
  blobStart16[1] = objVersion;
  blobStart32[1] = objSizeUint32;
  blobStart32[2] = nObjs;
  blobStart16[6] = nChans;
  blobStart16[7] = nGains;
  blobStart32[4] = commentSizeChar/sizeof(uint32_t);
  
  //==== fill comment fields
  if(commentSizeChar){
    if(!timeStamp) timeStamp = ::time(0);
    uint64_t* pTimeStamp = reinterpret_cast<uint64_t*>(blobStart32+dataSizeByte/sizeof(uint32_t));
    pTimeStamp[0] = timeStamp;
    char* pChar = reinterpret_cast<char*>(++pTimeStamp); 
    std::string::const_iterator iStr = author.begin();
    for(; iStr!=author.end(); ++iStr){ *pChar = *iStr; ++pChar; }
    *pChar = 0; 
    for(iStr=comment.begin(); iStr!=comment.end(); ++iStr){ *(++pChar) = *iStr; }
    *(++pChar) = 0;
  }

  return (blobSizeInBytes/sizeof(uint32_t));
}

//
//_____________________________________________________________
std::string 
TileCalibDrawerBase::getAuthor() const
{
  if(!getCommentSizeUint32()) return std::string("");
  const char* iBeg =
    reinterpret_cast<const char*>(m_blobStart32 + m_hdrSize32     +
                                  getNObjs()*getObjSizeUint32()   +
                                  sizeof(uint64_t)/sizeof(uint32_t));
  return std::string(iBeg);
}


//
//_____________________________________________________________
std::string 
TileCalibDrawerBase::getComment() const
{
  if(!getCommentSizeUint32()) return std::string("");
  const char* iBeg =
    reinterpret_cast<const char*>(m_blobStart32 + m_hdrSize32     +
                                  getNObjs()*getObjSizeUint32()   +
                                  sizeof(uint64_t)/sizeof(uint32_t));
  const char* iEnd = iBeg + getCommentSizeChar();
  iBeg = std::find(iBeg,iEnd,0);
  return std::string(++iBeg);
}

//
//_____________________________________________________________
std::string 
TileCalibDrawerBase::getDate() const
{
  if(!getCommentSizeUint32()) return std::string("");
  ::time_t timeStamp = getTimeStamp();
  char buf[32];
  char* iBeg = ::ctime_r(&timeStamp, buf);
  char* iEnd = iBeg;
  while(*iEnd!='\n'){++iEnd;}
  return std::string(iBeg,iEnd-iBeg);
}

//
//_____________________________________________________________
std::string
TileCalibDrawerBase::getFullComment() const
{
  if(!getCommentSizeUint32()) return std::string("");
  return getAuthor()+" ("+getDate()+"): "+getComment();
}


//
//_____________________________________________________________
void
TileCalibDrawerBase::dumpHeader(std::ostream& stm) const
{
  stm << "This is a " << TileCalibType::getClassName(getObjType()) << std::endl;
  stm << "ObjType        : " << getObjType()     << std::endl;
  stm << "ObjVersion     : " << getObjVersion()  << std::endl;
  stm << "ObjSize [bytes]: " << getObjSizeByte() << std::endl;
  stm << "NObjs          : " << getNObjs()       << std::endl;
  stm << "NChannels      : " << getNChans()      << std::endl;
  stm << "NGains         : " << getNGains()      << std::endl;
  if(!getCommentSizeUint32()){
    stm << "=== No comment available ===" << std::endl;
  }
  else{
    stm << "Author         : " << getAuthor()      << std::endl;
    stm << "Date           : " << getDate() << " ("<< getTimeStamp() << ")" << std::endl;
    stm << "Comment:       : " << getComment()     << std::endl;
  }  
}
