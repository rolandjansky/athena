/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <random>
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "../src/FTKByteStreamDecoderEncoder.h"
#include "../src/FTKByteStreamDecoderEncoder.cxx"

//int main() { return 0; }

class FTKEncodeDecodeTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE (FTKEncodeDecodeTest); 
  CPPUNIT_TEST (emptyCollection);
  CPPUNIT_TEST (filledCollection);
  CPPUNIT_TEST (hugeCollection);
  //  CPPUNIT_TEST (truncatedData);
  CPPUNIT_TEST_SUITE_END();

private:
  std::random_device m_rd;
  std::mt19937 m_gen;
  std::uniform_int_distribution<uint32_t> m_urng;
  


public:
  FTKEncodeDecodeTest() 
    : m_gen(m_rd()), m_urng(0, 0xffffffff) {}

  void setUp() {    
  }
  void tearDown(){}
 
  // no return as assertions will raise error when the tracks are different
  // realy they shoudl be quality comparable
  void compareTracks(const FTK_RawTrack* lhs, const FTK_RawTrack* rhs) {
    CPPUNIT_ASSERT_EQUAL(lhs->getTH1(), rhs->getTH1());
    CPPUNIT_ASSERT_EQUAL(lhs->getTH2(), rhs->getTH2());    
    CPPUNIT_ASSERT_EQUAL(lhs->getTH3(), rhs->getTH3());    
    CPPUNIT_ASSERT_EQUAL(lhs->getTH4(), rhs->getTH4());    
    CPPUNIT_ASSERT_EQUAL(lhs->getTH5(), rhs->getTH5());    
    
    for( size_t i = 0; i < 4; ++i ) {      
      CPPUNIT_ASSERT_EQUAL(lhs->getPixelCluster(i).getWordA(),  rhs->getPixelCluster(i).getWordA());    
      CPPUNIT_ASSERT_EQUAL(lhs->getPixelCluster(i).getWordB(),  rhs->getPixelCluster(i).getWordB());    
    }
    
    for( size_t i = 0; i < 8; ++i ) {      
      CPPUNIT_ASSERT_EQUAL(lhs->getSCTCluster(i).getWord(),  rhs->getSCTCluster(i).getWord());    
    }


    
  }

  uint32_t rnd() {
    return m_urng(m_gen);
  }

  FTK_RawTrack* generateTrack() {
    FTK_RawTrack* trk = new FTK_RawTrack(rnd(),rnd(),rnd(),rnd(),rnd());

    trk->getPixelClusters().resize(4);
    for ( auto& cluster: trk->getPixelClusters() ) {
      cluster.setWordA(rnd());
      cluster.setWordB(rnd());
    }

    trk->getSCTClusters().resize(8);
    for ( auto& cluster: trk->getSCTClusters() ) {
      cluster.setWord(rnd());
    }
    return trk;
  }
  // one function testing various sizes
  void createStreamUnstreamCheck(size_t ntrk) {
    MsgStream log(Athena::getMessageSvc(), "FTKEncodeDecodeTest");
    log.setLevel(1);
    using namespace FTKByteStreamDecoderEncoder;
    FTK_RawTrackContainer cont;
    
    for ( size_t i = 0 ; i < ntrk; ++i ) {
      cont.push_back(generateTrack());
    }
    log << MSG::DEBUG << "prepareed collection of size " << cont.size() << endreq;
    std::vector<uint32_t> payload;
    {
      StatusCode sc = encode(&cont, payload, log);
      CPPUNIT_ASSERT( sc.isSuccess() );
    }
    log << MSG::DEBUG << "Serialized payload to size " << payload.size() << endreq;
    // copy payload to plain array
    uint32_t * raw = new uint32_t[payload.size()];
    std::copy(payload.begin(), payload.end(), raw);

    CPPUNIT_ASSERT_EQUAL(ntrk,  decodeNumberOfTracks(raw) );
    FTK_RawTrackContainer* newcont = new FTK_RawTrackContainer();
    {
      StatusCode sc = decode(raw, newcont, log);
      CPPUNIT_ASSERT( sc.isSuccess() );
    }
    CPPUNIT_ASSERT_EQUAL(ntrk, newcont->size());
    delete [] raw;

    for ( size_t i = 0; i < ntrk; ++i ) {
      compareTracks(cont.at(i), newcont->at(i)); 
    }
    delete newcont;
  }

  void emptyCollection() {
    createStreamUnstreamCheck(0);
  }

  void filledCollection() {
    createStreamUnstreamCheck(10);
  }

  void hugeCollection() {
    createStreamUnstreamCheck(10000);
  }


private:



};
CPPUNIT_TEST_SUITE_REGISTRATION (FTKEncodeDecodeTest);
#include <TestPolicy/CppUnit_testdriver.cxx>
