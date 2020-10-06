/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_CaloCondBlobObjs
#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "CaloCondBlobObjs/CaloCondBlobBase.h"
#include "CoralBase/Blob.h"
#include <cstdint>
#include <sstream> 
#include <iostream>

//baseclass constructor is protected, so I expose it here
class CaloCondBlobStub: public CaloCondBlobBase{
public:
  CaloCondBlobStub(const coral::Blob & aBlob):CaloCondBlobBase(aBlob){
    //nop
  }
  CaloCondBlobStub(coral::Blob & aBlob):CaloCondBlobBase(aBlob){
    //nop
  }
  uint32_t
  createBlobExposed(uint16_t objType,          
			     uint16_t objVersion,
			     uint32_t objSizeUint32, 
			     uint32_t nObjs,
			     uint32_t nChans,
			     uint16_t nGains,
			     const std::string& author,
			     const std::string& comment,
			     uint64_t timeStamp){return createBlob(objType, objVersion, objSizeUint32,
			       nObjs, nChans, nGains, author, comment, timeStamp);}


};



BOOST_AUTO_TEST_SUITE(CaloCondBlobBaseTest)
BOOST_AUTO_TEST_CASE(Constructors){
  coral::Blob aBlob;
  BOOST_CHECK_NO_THROW(CaloCondBlobStub stub(aBlob));
}
BOOST_AUTO_TEST_CASE(PublicMethodsOnEmptyBlob){
  BOOST_TEST_MESSAGE("Construction of a CaloCondBlobBase with empty coral::Blob");
  coral::Blob emptyBlob;
  CaloCondBlobStub blobStub(emptyBlob);
  uint16_t baseTypeIndex(CaloCondType::BASE);
  BOOST_CHECK(blobStub.getType() == baseTypeIndex);
  BOOST_CHECK(blobStub.getBlobSize() == 0);
  BOOST_CHECK(blobStub.getObjSizeUint32() == 0);
  BOOST_CHECK(blobStub.getObjSizeByte() == 0);
  BOOST_CHECK(blobStub.getNChans() == 0);
  BOOST_CHECK(blobStub.getNGains() == 0);
  BOOST_CHECK(blobStub.getHdrSize() == 5);
  BOOST_TEST_MESSAGE("The remaining tests in this case should throw, as the blob is invalid");
  BOOST_CHECK_THROW(blobStub.getObjType(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getObjVersion(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getNObjs(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getCommentSizeUint32(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getCommentSizeChar(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getAuthor(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getComment(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getTimeStamp(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getDate(), CaloCond::InvalidBlob);
  BOOST_CHECK_THROW(blobStub.getFullComment(), CaloCond::InvalidBlob);
  unsigned int elementIdx{1};
  BOOST_CHECK_THROW(blobStub.getAddress(elementIdx), CaloCond::InvalidBlob);
  std::ostringstream strStream;
  BOOST_CHECK_THROW(blobStub.dump(strStream), CaloCond::InvalidBlob);
  //BOOST_TEST_MESSAGE(strStream.str());
  uint16_t version{0};
  uint32_t size{2};
  uint32_t num{10};
  uint32_t nChans{1};
  uint16_t nGains{1};
  const std::string author{"Shaun"};
  const std::string comment{"Just testing"};
  uint64_t timeStamp{0};
  BOOST_CHECK_NO_THROW(blobStub.createBlobExposed(baseTypeIndex,version,size,num,nChans,nGains,author, comment,timeStamp));
  BOOST_TEST(blobStub.getAuthor() == author);
  BOOST_TEST(blobStub.getBlobSize() != 0);
}

BOOST_AUTO_TEST_CASE(PublicMethods, * utf::expected_failures(1)){
  BOOST_TEST_MESSAGE("Construction of a CaloCondBlobBase with compliant coral::Blob");
  const uint32_t wordSize=sizeof(uint32_t);
  uint16_t version{0};
  uint32_t size{2};
  uint32_t num{10};
  uint32_t nChans{1};
  uint16_t gain{1};
  const std::string author{"Shaun"};
  const std::string comment{"Just testing"};
  //
  const uint32_t headerWordSize{5};
  const uint32_t dataSizeWord = (headerWordSize+(size*num));
  const uint32_t dataSizeByte = dataSizeWord * wordSize; //(5+20) * 4 = 100 bytes
  const uint32_t nullTerminators{2};
  const uint32_t textSize = comment.size() + author.size()+nullTerminators;//12 + 5 + 2 = 19 bytes
  uint32_t textAndTimestampSize = textSize + sizeof(uint64_t);//19+8 = 27 bytes
  //clean up scrappy word alignment
  if (auto alignment=textAndTimestampSize % wordSize;alignment !=0){
    textAndTimestampSize += (wordSize - alignment); //27 % 4 = 3, 27 + 1 = 28
  }
  const uint32_t blobSizeInBytes = dataSizeByte + textAndTimestampSize; //128 bytes
  //prepare a compliant blob
  coral::Blob b(blobSizeInBytes);
  uint32_t* pDestination = static_cast<uint32_t*> (b.startingAddress());
  pDestination[0] = version; 
  pDestination[1] = size;
  pDestination[2] = num;
  pDestination[3] = 0x80000000 | (gain << 24) | nChans;
  pDestination[4] = comment.size() / wordSize;
  uint64_t* pTimeStamp = reinterpret_cast<uint64_t*>(pDestination+dataSizeWord);
  uint64_t timeStamp{0};
  pTimeStamp[0] = timeStamp;
  char* pChar = reinterpret_cast<char*>(++pTimeStamp); 
  strcpy(pChar, author.c_str());
  pChar+=(1+author.size());
  strcpy(pChar, comment.c_str());
  //instantiate the class using this blob
  CaloCondBlobStub blobStub(b);
  //checking...
  uint16_t baseTypeIndex(CaloCondType::BASE);
  BOOST_TEST(blobStub.getType() == baseTypeIndex);
  BOOST_TEST(blobStub.getBlobSize() == 128);
  BOOST_TEST(blobStub.getObjSizeUint32() == 2);
  BOOST_TEST(blobStub.getObjSizeByte() == 8);
  BOOST_TEST(blobStub.getNChans() == 1);
  BOOST_TEST(blobStub.getNGains() == 1);
  BOOST_TEST(blobStub.getHdrSize() == 5);
  BOOST_TEST(blobStub.getAuthor() == author);
  BOOST_TEST(blobStub.getComment() == comment);
  std::ostringstream representation;
  BOOST_CHECK_NO_THROW(blobStub.dump(representation));
  //BOOST_TEST_MESSAGE(representation.str());
  std::string expectedString=R"(This is a CaloCondBlobBase
ObjType        : 0
ObjVersion     : 0
ObjSize [bytes]: 8
NObjs          : 10
NChannels      : 1
NGains         : 1
Author         : Shaun
Date           : Thu Jan  1 01:00:00 1970 (0)
Comment:       : Just testing
)";
  BOOST_TEST(representation.str() == expectedString);
  uint16_t gainTooBig{0x80};//gain must be <=0x7F
  pDestination[3] = 0x80000000 | (gainTooBig << 24) | nChans;
  BOOST_CHECK_THROW(CaloCondBlobStub invalid(b),CaloCond::InvalidBlob);//expect this test to fail
  
}
BOOST_AUTO_TEST_SUITE_END()
