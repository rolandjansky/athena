/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondBlobObjs/CaloCondBlobBase.h"
#include "CaloCondBlobObjs/CaloCondType.h"
#include <algorithm>
#include <ctime>

//
//_____________________________________________________________
CaloCondBlobBase::CaloCondBlobBase(coral::Blob* blob_nc,
                                   const coral::Blob* blob)
  : m_blob_nc(blob_nc),
    m_blob(blob)
  , m_isBlobOwner(false),m_sizeOfObj(0),m_nChans(0),m_nGains(0) 
{
  if(uint64_t(m_blob->size())>=getHdrSize()*sizeof(uint32_t)) {
    m_sizeOfObj=static_cast<const uint32_t*>(m_blob->startingAddress())[1];
    //m_nChans=static_cast<uint16_t*>(m_blob->startingAddress())[6];
    //m_nGains=static_cast<uint16_t*>(m_blob->startingAddress())[7];
    const CaloCondBlobBase& this_c = *this;
    m_nChans=unpacknChans(this_c.getBlobStart()[3]);
    m_nGains=unpacknGains(this_c.getBlobStart()[3]);
    //std::cout << "CaloCondBlobObj: nChans=" << m_nChans << " nGains=" << m_nGains << std::endl;
  }
  return;
}

CaloCondBlobBase::CaloCondBlobBase(const coral::Blob& blob)
  : CaloCondBlobBase (nullptr, &blob)
{
}

CaloCondBlobBase::CaloCondBlobBase(coral::Blob& blob)
  : CaloCondBlobBase (&blob, &blob)
{
}

//
//_____________________________________________________________
CaloCondBlobBase::~CaloCondBlobBase()
{
  //=== delete the coral::Blob if owner
  if(m_isBlobOwner) {delete m_blob;}
}

//
//_____________________________________________________________
CaloCondBlobBase::CaloCondBlobBase(const CaloCondBlobBase& other)
  : m_blob_nc(new coral::Blob(*other.m_blob))
  , m_blob (m_blob_nc)
  , m_isBlobOwner(true),
    m_sizeOfObj(other.getObjSizeUint32()),
    m_nChans(other.getNChans()),
    m_nGains(other.getNChans())
{
}

//
//_____________________________________________________________
CaloCondBlobBase& 
CaloCondBlobBase::operator=(const CaloCondBlobBase& other)
{
  //=== catch self-assignment
  if(&other == this) {return *this;}
  if (m_isBlobOwner) delete m_blob;
  m_blob = other.m_blob;
  m_blob_nc = other.m_blob_nc;
  m_isBlobOwner = false;
  m_sizeOfObj=static_cast<const uint32_t*>(m_blob->startingAddress())[1];
  m_nChans=unpacknChans(getBlobStart()[3]);
  m_nGains=unpacknGains(getBlobStart()[3]);
  return *this;
}


//
//_____________________________________________________________
uint32_t
CaloCondBlobBase::createBlob(uint16_t objType,          
			     uint16_t objVersion,
			     uint32_t objSizeUint32, 
			     uint32_t nObjs,
			     uint32_t nChans,
			     uint16_t nGains,
			     const std::string& author,
			     const std::string& comment,
			     uint64_t timeStamp)
{
  
  //=== blob data length including header in bytes
  const uint32_t dataSizeByte = (getHdrSize()+objSizeUint32*nObjs) * sizeof(uint32_t);
  
  //=== calculate comment length, including two ASCII NULLs to end text fields
  uint32_t commentSizeChar(0);
  if(!author.empty() || !comment.empty() || timeStamp){
    commentSizeChar += author.size()+comment.size()+sizeof(uint64_t) + 2;
    //=== force comment length to end on 4 byte boundary
    commentSizeChar += (commentSizeChar % sizeof(uint32_t)) ? 
      (sizeof(uint32_t)-(commentSizeChar % sizeof(uint32_t))) : 0;
  }
  
  //=== create blob
  const uint32_t blobSizeInBytes = dataSizeByte+commentSizeChar;
  m_blob_nc->resize(blobSizeInBytes);
  
  //=== fill header
  reinterpret_cast<uint16_t*>(getBlobStart())[0] = objType;
  reinterpret_cast<uint16_t*>(getBlobStart())[1] = objVersion;
  getBlobStart()[1] = objSizeUint32;
  m_sizeOfObj=objSizeUint32;
  getBlobStart()[2] = nObjs;
  getBlobStart()[3] = packGainAndNchans(nGains,nChans);
  m_nChans=nChans;
  m_nGains=nGains;
  getBlobStart()[4] = commentSizeChar/sizeof(uint32_t);
  
  //==== fill comment fields
  if(commentSizeChar){
    if(!timeStamp) timeStamp = ::time(nullptr);
    uint64_t* pTimeStamp = reinterpret_cast<uint64_t*>(getBlobStart()+dataSizeByte/sizeof(uint32_t));
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


uint32_t CaloCondBlobBase::packGainAndNchans(const uint32_t gain, const uint32_t nChans) {

  if (gain>0x7F) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::packGainAndNchans: Gain too large");
 
  if (nChans>0xFFFFFF) 
    throw CaloCond::InvalidBlob("CaloCondBlobBase::packGainAndNchans: Number of channels too large");

  return 0x80000000 | (gain << 24) | nChans;
}

uint32_t CaloCondBlobBase::unpacknGains(const uint32_t gainAndnChans) {
  if (gainAndnChans & 0x80000000) { //new version: 24 bytes for the nChans, 7 for the gain
    //std::cout << "CaloCondBlobBase: Reading gain from new version" << std::endl;
    return (gainAndnChans>>24) & 0x7F;
  }
  else //Old version: 16 bigs for gain
    //std::cout << "CaloCondBlobBase: Reading gain from old version" << std::endl;
    return (gainAndnChans>>16) & 0xFFFF;
} 

uint32_t CaloCondBlobBase::unpacknChans(const uint32_t gainAndnChans) {
  if (gainAndnChans & 0x80000000) { //new version: 24 bytes for the nChans, 7 for the gain
    //std::cout << "CaloCondBlobBase: Reading nChans from new version" << std::endl;
    return gainAndnChans & 0xFFFFFF;
  }
  else //Old version: 16 bits for nChans
    //std::cout << "CaloCondBlobBase: Reading nChangs from old version" << std::endl;
    return gainAndnChans & 0xFFFF;
} 




//
//_____________________________________________________________
std::string 
CaloCondBlobBase::getAuthor() const
{
  if(!getCommentSizeUint32()) return std::string("");
  const char* iBeg =
    reinterpret_cast<const char*>(getBlobStart()+getHdrSize()     +
                                  getNObjs()*getObjSizeUint32()   +
                                  sizeof(uint64_t)/sizeof(uint32_t));
  return std::string(iBeg);
}


//
//_____________________________________________________________
std::string 
CaloCondBlobBase::getComment() const
{
  if(!getCommentSizeUint32()) return std::string("");
  const char* iBeg =
    reinterpret_cast<const char*>(getBlobStart()+getHdrSize()     +
                                  getNObjs()*getObjSizeUint32()   +
                                  sizeof(uint64_t)/sizeof(uint32_t));
  const char* iEnd = iBeg + getCommentSizeChar();
  iBeg = std::find(iBeg,iEnd,0);
  return std::string(++iBeg);
}

//
//_____________________________________________________________
std::string 
CaloCondBlobBase::getDate() const
{
  if(!getCommentSizeUint32()) return std::string("");
  ::time_t timeStamp = getTimeStamp();
  char buf[23];
  char* iBeg = ::ctime_r(&timeStamp, buf);
  char* iEnd = iBeg;
  while(*iEnd!='\n'){++iEnd;}
  return std::string(iBeg,iEnd-iBeg);
}

//
//_____________________________________________________________
std::string
CaloCondBlobBase::getFullComment() const
{
  if(!getCommentSizeUint32()) return std::string("");
  return getAuthor()+" ("+getDate()+"): "+getComment();
}


//
//_____________________________________________________________
void
CaloCondBlobBase::dumpHeader(std::ostream& stm) const
{
  stm << "This is a " << CaloCondType::getClassName(getObjType()) << std::endl;
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
